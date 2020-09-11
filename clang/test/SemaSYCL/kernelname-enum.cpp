// RUN: %clang_cc1 -fsycl -fsycl-is-device -fsycl-int-header=%t.h -fsyntax-only -Wno-sycl-2017-compat -verify %s

//#include "Inputs/sycl.hpp"
#ifndef SYCL_HPP
#define SYCL_HPP

class auto_name {};
template <typename Name, typename Type>
struct get_kernel_name_t {
  using name = Name;
};
template <typename Type>
struct get_kernel_name_t<auto_name, Type> {
  using name = Type;
};
#define ATTR_SYCL_KERNEL __attribute__((sycl_kernel))
template <typename KernelName = auto_name, typename KernelType>
ATTR_SYCL_KERNEL void kernel_single_task(const KernelType &kernelFunc) {
  kernelFunc();
}
  template <typename KernelName = auto_name, typename KernelType>
  void single_task(const KernelType &kernelFunc) {
    using NameT = typename get_kernel_name_t<KernelName, KernelType>::name;
#ifdef __SYCL_DEVICE_ONLY__
// expected-error@+1 1+{{kernel name is invalid. Unscoped enum requires fixed underlying type}}
    kernel_single_task<NameT>(kernelFunc);
#else
    kernelFunc();
#endif
  }

#endif

enum unscoped_enum_int : int {
  val_1,
  val_2
};

// expected-note@+1 1+ {{'unscoped_enum_no_type_set' declared here}}
enum unscoped_enum_no_type_set {
  val_3,
  val_4
};

enum class scoped_enum_int : int {
  val_1,
  val_2
};

enum class scoped_enum_no_type_set {
  val_3,
  val_4
};

template <unscoped_enum_int EnumType>
class dummy_functor_1 {
public:
  void operator()() const {}
};


template <unscoped_enum_no_type_set EnumType>
class dummy_functor_2 {
public:
  void operator()() const {}
};

template <template <unscoped_enum_no_type_set EnumType> class C>
class templated_functor {
public:
  void operator()() const {}
};

template <scoped_enum_int EnumType>
class dummy_functor_3 {
public:
  void operator()() const {}
};

template <scoped_enum_no_type_set EnumType>
class dummy_functor_4 {
public:
  void operator()() const {}
};

int main() {

  dummy_functor_1<val_1> f1;
  dummy_functor_2<val_3> f2;
  dummy_functor_3<scoped_enum_int::val_2> f3;
  dummy_functor_4<scoped_enum_no_type_set::val_4> f4;
  templated_functor<dummy_functor_2> f5;

// expected-note@+1{{in instantiation of function template specialization}}
  single_task(f2);
// expected-note@+1{{in instantiation of function template specialization}}  
  single_task(f5);

  return 0;
}