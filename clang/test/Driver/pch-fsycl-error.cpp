// This test checks that an error is emitted when 
// PCH(Pre-Compiled Header) file generation is forced in -fsycl mode.

// RUN: touch %t.h

// Check if the deafult PCH file generated is a .pchi file.
// RUN: %clang -c -fsycl -x c++-header %t.h -###  %s 2> %t1.txt
// RUN: FileCheck %s -input-file=%t1.txt
// CHECK: Pre-compiled header generation is not supported with '-fsycl'

