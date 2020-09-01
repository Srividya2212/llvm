// RUN: %clang_cc1 -I %S/Inputs -fsycl -fsycl-is-device -fsycl-int-header=%t.h -fsyntax-only -Wno-sycl-2017-compat %s

#include "sycl.hpp"
enum unscoped_enum_no_type_set {
  val_3,
  val_4
};

template <unscoped_enum_no_type_set EnumType>
class dummy_functor_2 {
public:
  void operator()() const {}
};

int main() {
  dummy_functor_2<val_3> f2;
  cl::sycl::queue q;
  q.submit([&](cl::sycl::handler &cgh) {
    cgh.single_task(f2);
  });
  return 0;
}
