// RUN: $(dirname %s)/check_clang_tidy.sh %s readability-redundant-include %t -- -std=c++11 -I$(dirname %s)
// REQUIRES: shell

int a;
#include <string.h>
int b;
#include <string.h>
int c;
// CHECK-MESSAGES: :[[@LINE-2]]:1: warning: redundant include [readability-redundant-include]
// CHECK-FIXES:      {{^int a;$}}
// CHECK-FIXES-NEXT: {{^#include <string.h>$}}
// CHECK-FIXES-NEXT: {{^int b;$}}
// CHECK-FIXES-NEXT: {{^int c;$}}

int d;
#include <iostream>
int e;
#include <iostream> // extra stuff that will also be removed
int f;
// CHECK-MESSAGES: :[[@LINE-2]]:1: warning: {{.*}}
// CHECK-FIXES:      {{^int d;$}}
// CHECK-FIXES-NEXT: {{^#include <iostream>$}}
// CHECK-FIXES-NEXT: {{^int e;$}}
// CHECK-FIXES-NEXT: {{^int f;$}}

int g;
#include "readability-redundant-include.h"
int h;
#include "readability-redundant-include.h"
int i;
// CHECK-MESSAGES: :[[@LINE-2]]:1: warning: {{.*}}
// CHECK-FIXES:      {{^int g;$}}
// CHECK-FIXES-NEXT: {{^#include "readability-redundant-include.h"$}}
// CHECK-FIXES-NEXT: {{^int h;$}}
// CHECK-FIXES-NEXT: {{^int i;$}}

#include "types.h"

int j;
#include <sys/types.h>
int k;
#include <sys/types.h>
int l;
// CHECK-MESSAGES: :[[@LINE-2]]:1: warning: {{.*}}
// CHECK-FIXES:      {{^int j;$}}
// CHECK-FIXES-NEXT: {{^#include <sys/types.h>$}}
// CHECK-FIXES-NEXT: {{^int k;$}}
// CHECK-FIXES-NEXT: {{^int l;$}}

int m;
        #          include             <string.h>  // lots of space
int n;
// CHECK-MESSAGES: :[[@LINE-2]]:9: warning: {{.*}}
// CHECK-FIXES:      {{^int m;$}}
// CHECK-FIXES-NEXT: {{^int n;$}}
