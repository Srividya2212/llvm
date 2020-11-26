// RUN: %clang_cc1 -fsycl -fsycl-is-device -internal-isystem %S/Inputs -fsyntax-only -Wno-sycl-2017-compat -sycl-std=2020 -verify %s

#include "Inputs/sycl.hpp"

using namespace cl::sycl;

int main(void) {
  constexpr unsigned VL = 16;

  int a;
  buffer<int, 1> buf(&a, range<1>(1));
  queue q;

  auto e = q.submit([&](handler &cgh) {
    auto Acc = buf.get_access<access::mode::write>(cgh);

    cgh.parallel_for<class LambdaKernel>(
      range<1>{1}, [=](id<1> i) {
          int x = i * VL * sizeof(int);
        });
  });
  return 0;
}