// RUN: $(dirname %s)/check_clang_tidy.sh %s readability-redundant-fwd-decl %t
// REQUIRES: shell

class Foo;
class Foo;
// CHECK-MESSAGES: :[[@LINE-1]]:1: warning: redundant forward declaration [readability-redundant-fwd-decl]
