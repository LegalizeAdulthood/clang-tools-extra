// RUN: $(dirname %s)/check_clang_tidy.sh %s misc-guard-macro-body %t
// REQUIRES: shell

#define SIMPLE_OBJECT 1

#define SIMPLE_FUNCTION(x_) x_

#define UNGUARDED_IF(x_) if (x_ > 0) return true
// CHECK-MESSAGES: :[[@LINE-1]]:9: warning: macro guard needed [misc-guard-macro-body]
// CHECK-FIXES: {{^}}#define UNGUARDED_IF(x_) do { if (x_ > 0) return true; } while (false)

#define GUARDED_IF(x_)                                                         \
  do {                                                                         \
    if (x_ > 0)                                                                \
      return true;                                                             \
  } while (false)

#define GUARDED_SIMPLE_IF(x_)  \
  do                           \
    if (x_ > 0)                \
      return true;             \
    else                       \
      return false;            \
  while (false)

bool use_some_macros()
{
  int i = SIMPLE_OBJECT;
  if (SIMPLE_FUNCTION(i) > 1) {
    UNGUARDED_IF(i);
  }
  if (SIMPLE_FUNCTION(i) > 1) {
    GUARDED_IF(i);
  } else {
    GUARDED_SIMPLE_IF(i);
  }
  return false;
}
