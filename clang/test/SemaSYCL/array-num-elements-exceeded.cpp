// RUN: %clang_cc1 -I %S/Inputs -fsycl -fsycl-is-device -verify -fsyntax-only %s
#include "big_constant.h"

template <typename Name, typename Func>
__attribute__((sycl_kernel)) void kernel_single_task(Func kernelFunc) {

  kernelFunc(); // expected-note {{called by 'kernel_single_task<fake_kernel, (lambda}}
}

int main() {
  kernel_single_task<class fake_kernel>([]() {
    double res = big[0]; // expected-warning {{Due to SPIR-V intermediate format limitations, constant arrays with number of elements more than 65532 cannot be used in SYCL.You can workaround this limitation by splitting the array into a several ones}}
  });
  return 0;
}
