// RUN: %clang_cc1 -fsyntax-only -verify %s
// RUN: %clang_cc1 -fsyntax-only -verify -std=c++98 %s
// RUN: %clang_cc1 -fsyntax-only -verify -std=c++11 %s



template <typename T> int f0(void *, const T&); // expected-note{{candidate template ignored: substitution failure}}
enum {e};
#if __cplusplus <= 199711L
// expected-note@-2 {{unnamed type used in template argument was declared here}}
#endif

void test_f0(int n) {
  int i = f0(0, e);
#if __cplusplus <= 199711L
  // expected-warning@-2 {{template argument uses unnamed type}}
#endif

  int vla[n];
  f0(0, vla); // expected-error{{no matching function for call to 'f0'}}
}

