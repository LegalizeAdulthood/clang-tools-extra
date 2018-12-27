// RUN: clang-tidy -checks='-*,readability-simplify-boolean-expr' -header-filter='.*' %s -- std=c++11 | FileCheck c:/Code/clang/llvm/tools/clang/tools/extra/test/clang-tidy/readability-simplify-bool-expr.h

#include "readability-simplify-bool-expr.h"
// CHECK-MESSAGES
