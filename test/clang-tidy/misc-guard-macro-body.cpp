// RUN: $(dirname %s)/check_clang_tidy.sh %s misc-guard-macro-body %t
// REQUIRES: shell

#define SIMPLE_MACRO 1

#define SIMPLE_FUNCTION_MACRO(x_) x_

#define COMPLEX_FUNCTION_MACRO(x_)                                             \
  if (x_ > 0)                                                                  \
    return true
// CHECK-MESSAGES: :[[@LINE-3]]:9: warning: unguarded macro body [misc-guard-macro-body]
