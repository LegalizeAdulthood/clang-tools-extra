// RUN: %check_clang_tidy %s readability-redundant-fwd-decl %t

class Foo;
class Foo;
// CHECK-MESSAGES: :[[@LINE-1]]:1: warning: redundant forward declaration [readability-redundant-fwd-decl]

class Bar {
};

class Bar;
// CHECK-MESSAGES: :[[@LINE-1]]:1: warning: redundant forward declaration
