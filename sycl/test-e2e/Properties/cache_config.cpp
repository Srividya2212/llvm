// REQUIRES: gpu, level_zero

// TODO: Currently using the -Wno-deprecated-declarations flag due to issue
// https://github.com/intel/llvm/issues/16320. Remove the flag once the issue is
// resolved.
// RUN: %{build} -o %t.out -Wno-deprecated-declarations
// RUN: env UR_L0_DEBUG=1 %{run} %t.out 2>&1 | FileCheck %s

#include <numeric>
#include <sycl/detail/core.hpp>
#include <sycl/properties/all_properties.hpp>
#include <sycl/reduction.hpp>

using namespace sycl;
using namespace sycl::ext::intel::experimental;
using namespace sycl::ext::oneapi::experimental;

struct KernelFunctor {

  KernelFunctor() {}

  void operator()() const {}
  auto get(properties_tag) const { return properties{cache_config(large_slm)}; }
};

struct KernelFunctorND {

  KernelFunctorND() {}

  void operator()(nd_item<2> i) const {}
  auto get(properties_tag) const { return properties{cache_config(large_slm)}; }
};

struct NegativeKernelFunctor {

  NegativeKernelFunctor() {}

  void operator()(nd_item<2> i) const {}
  auto get(properties_tag) const { return properties{}; }
};

struct RangeKernelFunctor {

  RangeKernelFunctor() {}

  void operator()(id<2> i) const {}
  auto get(properties_tag) const { return properties{cache_config(large_slm)}; }
};

int main() {
  sycl::property_list q_prop{sycl::property::queue::in_order()};
  queue q{q_prop};

  sycl::ext::oneapi::experimental::properties properties{
      cache_config(large_slm)};

  // CHECK: parallel_for with sycl::range
  // CHECK: zeKernelSetCacheConfig
  std::cout << "parallel_for with sycl::range" << std::endl;
  q.parallel_for(range<2>{16, 16}, RangeKernelFunctor{}).wait();

  // CHECK: parallel_for_work_group(range, func)
  // CHECK: zeKernelSetCacheConfig
  std::cout << "parallel_for_work_group(range, func)" << std::endl;
  q.submit([&](handler &cgh) {
    cgh.parallel_for_work_group<class hpar_range>(
        range<1>(8), properties,
        [=](group<1> g) { g.parallel_for_work_item([&](h_item<1> i) {}); });
  });

  // CHECK: parallel_for_work_group(range, range, func)
  // CHECK: zeKernelSetCacheConfig
  std::cout << "parallel_for_work_group(range, range, func)" << std::endl;
  q.submit([&](handler &cgh) {
    cgh.parallel_for_work_group<class hpar_range_range>(
        range<1>(8), range<1>(4), properties,
        [=](group<1> g) { g.parallel_for_work_item([&](h_item<1> i) {}); });
  });

  buffer<int> values_buf{1024};
  {
    host_accessor a{values_buf};
    std::iota(a.begin(), a.end(), 0);
  }

  int sum_result = 0;
  buffer<int> sum_buf{&sum_result, 1};

  // CHECK: parallel_for with reduction
  // CHECK: zeKernelSetCacheConfig
  std::cout << "parallel_for with reduction" << std::endl;
  q.submit([&](handler &cgh) {
    auto input_values = values_buf.get_access<access_mode::read>(cgh);
    auto sum_reduction = reduction(sum_buf, cgh, plus<>());
    cgh.parallel_for(range<1>{1024}, properties, sum_reduction,
                     [=](id<1> idx, auto &sum) { sum += input_values[idx]; });
  });

  // CHECK: KernelFunctor single_task
  // CHECK: zeKernelSetCacheConfig
  std::cout << "KernelFunctor single_task" << std::endl;
  q.single_task(KernelFunctor{}).wait();

  // CHECK: KernelFunctor parallel_for
  // CHECK: zeKernelSetCacheConfig
  std::cout << "KernelFunctor parallel_for" << std::endl;
  q.parallel_for(nd_range<2>{range<2>(4, 4), range<2>(2, 2)}, KernelFunctorND{})
      .wait();

  // CHECK: negative parallel_for with sycl::nd_range
  // CHECK-NOT: zeKernelSetCacheConfig
  std::cout << "negative parallel_for with sycl::nd_range" << std::endl;
  q.parallel_for(nd_range<2>{range<2>(4, 4), range<2>(2, 2)},
                 [=](nd_item<2> i) {})
      .wait();

  // CHECK: negative parallel_for with KernelFunctor
  // CHECK-NOT: zeKernelSetCacheConfig
  std::cout << "negative parallel_for with KernelFunctor" << std::endl;
  q.parallel_for(nd_range<2>{range<2>(4, 4), range<2>(2, 2)},
                 NegativeKernelFunctor{})
      .wait();

  return 0;
}
