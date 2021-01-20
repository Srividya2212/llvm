// RUN: %clang_cc1 -fsycl -fsycl-is-device -fsyntax-only -Wno-sycl-2017-compat -verify %s

#include "Inputs/sycl.hpp"

 struct nested {
  struct name {};
};

struct Parent1 { using A = struct { struct Child1{struct Child2{};}; }; }; 

struct MyWrapper {

public:
  void test() {
    cl::sycl::queue q;

    // Should not throw an error but it does now
    q.submit([&](cl::sycl::handler &h) {
      h.single_task<nested::name>([] {});
    });

    // Throws "kernel name not globally visible" error
    q.submit([&](cl::sycl::handler &h) {
      h.single_task<Parent1::A::Child1::Child2>([] {});
    });

  }
};

int main() {
  cl::sycl::queue q;

  return 0;
}
