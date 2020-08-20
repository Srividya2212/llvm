// RUN: %clang_cc1 -I %S/Inputs -fsycl -fsycl-is-device -fsycl-int-header=%t.h -fsyntax-only -verify %s

// expected-note@+1 {{'unscoped_enum_no_type_set' declared here}}
enum unscoped_enum_no_type_set {
  val_3,
  val_4
};


template <unscoped_enum_no_type_set EnumType>
class dummy_functor_2 {
public:
  void operator()() {}
};

dummy_functor_2<val_3> f2;

template <typename Name, typename F>
__attribute__((sycl_kernel)) void kernel(F kernelFunc) {
  kernelFunc();
}

template <typename Name, typename F>
void uses_kernel(F kernelFunc) {
  // expected-error@+1{{kernel name is invalid. Unscoped enum requires fixed underlying type}}
  kernel<Name>(kernelFunc);
}

template <typename Name>
void kernel_wrapper() {
  // expected-note@+1{{in instantiation of function template specialization}}
  uses_kernel<Name>(f2);
}

void use() {
  // expected-note@+1{{in instantiation of function template specialization}}
  kernel_wrapper<class Foo>();
}
