// RUN: %clang_cc1 -fsycl -fsycl-is-device -internal-isystem %S/Inputs -fsyntax-only -Wno-sycl-2017-compat -sycl-std=2020 -verify %s

#include "sycl.hpp"

using namespace cl::sycl;

queue q;

 

int main() {

constexpr unsigned V = 16;
struct Id {
  int x;

  Id(int x) : x(x) {}

  Id operator *(int &val) {
    return Id(x * val);
  }

  int get() { return x; }
};

  Id i{5};

  q.submit([&](handler &h) {
    h.single_task<class Test>(
    [=]() {
       int A = i.get() * V * sizeof(int);
    });
  });
  return 0;
}
