// RUN: rm -fr %t.promo.prof
// RUN: rm -fr %t.nopromo.prof
// RUN: %clang_pgogen=%t.promo.prof/ -o %t.promo.gen -O2 %s -fno-slp-vectorize
// RUN: %clang_pgogen=%t.promo.prof/ -o %t.promo.gen.ll -emit-llvm -S -O2 %s -fno-slp-vectorize
// RUN: cat %t.promo.gen.ll | FileCheck --check-prefix=PROMO %s
// RUN: %run %t.promo.gen
// RUN: llvm-profdata merge -o %t.promo.profdata %t.promo.prof/
// RUN: llvm-profdata show --counts --all-functions %t.promo.profdata  > %t.promo.dump
// RUN: %clang_pgogen=%t.nopromo.prof/ -mllvm -do-counter-promotion=false -mllvm -simplifycfg-sink-common=false -o %t.nopromo.gen -O2 %s -fno-slp-vectorize
// RUN: %clang_pgogen=%t.nopromo.prof/ -mllvm -do-counter-promotion=false -mllvm -simplifycfg-sink-common=false -o %t.nopromo.gen.ll -emit-llvm -S -O2 %s -fno-slp-vectorize
// RUN: cat %t.nopromo.gen.ll | FileCheck --check-prefix=NOPROMO %s
// RUN: %run %t.nopromo.gen
// RUN: llvm-profdata merge -o %t.nopromo.profdata %t.nopromo.prof/
// RUN: llvm-profdata show --counts --all-functions %t.nopromo.profdata  > %t.nopromo.dump
// RUN: llvm-profdata show %t.promo.profdata > %t.promo.dump
// RUN: llvm-profdata show %t.nopromo.profdata > %t.nopromo.dump
// RUN: diff %t.promo.dump %t.nopromo.dump
int g;
__attribute__((noinline)) void bar(int i) { g += i; }
__attribute__((noinline)) void foo(int n, int N) {
// PROMO-LABEL: @foo
// PROMO:  load{{.*}}@__profc_foo, align
// PROMO-NEXT: add
// PROMO-NEXT: store{{.*}}@__profc_foo, align
// PROMO-NEXT: load{{.*}}@__profc_foo{{.*}} 8){{.*}}
// PROMO-NEXT: add
// PROMO-NEXT: store{{.*}}@__profc_foo{{.*}} 8){{.*}}
// PROMO-NEXT: load{{.*}}@__profc_foo{{.*}} 16){{.*}}
// PROMO-NEXT: add
// PROMO-NEXT: store{{.*}}@__profc_foo{{.*}} 16){{.*}}
//
// NOPROMO-LABEL: @foo
// NOPROMO: load{{.*}}@__profc_foo, align
// NOPROMO-NEXT: add
// NOPROMO-NEXT: store{{.*}}@__profc_foo, align
// NOPROMO: load{{.*}}@__profc_foo{{.*}} 8){{.*}}
// NOPROMO-NEXT: add
// NOPROMO-NEXT: store{{.*}}@__profc_foo{{.*}} 8){{.*}}
// NOPROMO: load{{.*}}@__profc_foo{{.*}} 16){{.*}}
// NOPROMO-NEXT: add
// NOPROMO-NEXT: store{{.*}}@__profc_foo{{.*}} 16){{.*}}
  int i = 0;
  while (i < N) {
    if (i < n + 1)
      bar(1);
    else if (i == n - 1)
      bar(2);
    else
      bar(3);
    i++;
  }
}

int main() {
  foo(10, 20);
  return 0;
}
