// RUN: %clang_cc1 -I %S/Inputs -fsycl -fsycl-is-device -verify -fsyntax-only %s
#include "big_constant.h"

template <typename Name, typename Func>
__attribute__((sycl_kernel)) void kernel_single_task(Func kernelFunc) {

  kernelFunc(); // expected-note {{called by 'kernel_single_task<fake_kernel, (lambda}}
}

int main() {
  double res;
  kernel_single_task<class fake_kernel>([=]() mutable {
    res = big[0]; // expected-error {{Global variable has a constant array with number of elements greater than OpConstantComposite can have (65532).Can the array be split?}}
  });

  return 0;
}
