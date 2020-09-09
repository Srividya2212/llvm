// RUN: %clang_cc1 -I %S/Inputs -fsycl -fsycl-is-device -fsycl-int-header=%t.h -fsyntax-only -Wno-sycl-2017-compat -verify %s
// RUN: %clang_cc1 -I %S/Inputs -fsycl -fsycl-is-device -fsycl-int-header=%t.h -fsycl-unnamed-lambda -fsyntax-only -Wno-sycl-2017-compat -verify %s

#include <sycl.hpp>

#ifdef __SYCL_UNNAMED_LAMBDA__
// expected-no-diagnostics
#endif

namespace namespace1 {
template <typename T>
class KernelName;
}

struct MyWrapper {
private:
  class InvalidKernelName0 {};
  class InvalidKernelName3 {};
  class InvalidKernelName4 {};
  class InvalidKernelName5 {};

public:
  void test() {
    cl::sycl::queue q;
#ifndef __SYCL_UNNAMED_LAMBDA__
    
    // expected-note@+2 {{InvalidKernelName1 declared here}}
#endif
    class InvalidKernelName1 {};
    q.submit([&](cl::sycl::handler &h) {
      // expected-note@+1{{in instantiation of function template specialization}}
      h.single_task<InvalidKernelName1>([] {});
    });
}
};

int main() {
  cl::sycl::queue q;
#ifndef __SYCL_UNNAMED_LAMBDA__
  
#endif
// expected-note@+1{{in instantiation of function template specialization}}
  q.submit([&](cl::sycl::handler &h) { h.single_task([] {}); });

  return 0;
}
