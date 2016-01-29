// RUN: %check_clang_tidy %s modernize-raw-string-literal %t

char const *const BackSlash{"goink\\frob"};
// CHECK-MESSAGES: :[[@LINE-1]]:29: warning: escaped string literal can be written as a raw string literal [modernize-raw-string-literal]
// CHECK-FIXES: {{^}}char const *const BackSlash{R"(goink\frob)"};{{$}}

char const *const Bell{"goink\\\afrob"};
char const *const BackSpace{"goink\\\bfrob"};
char const *const FormFeed{"goink\\\ffrob"};
char const *const CarraigeReturn{"goink\\\rfrob"};
char const *const HorizontalTab{"goink\\\tfrob"};
char const *const VerticalTab{"goink\\\vfrob"};
char const *const OctalNonPrintable{"\\\003"};
char const *const HexNonPrintable{"\\\x03"};
char const *const Delete{"\\\177"};
char const *const TrailingSpace{"A line \\with space. \n"};
char const *const TrailingNewLine{"A single \\line.\n"};
char const *const AlreadyRaw{R"(foobie\\bletch)"};
char const *const UTF8Literal{u8"foobie\\bletch"};
char const *const UTF8RawLiteral{u8R"(foobie\\bletch)"};
char16_t const *const UTF16Literal{u"foobie\\bletch"};
char16_t const *const UTF16RawLiteral{uR"(foobie\\bletch)"};
char32_t const *const UTF32Literal{U"foobie\\bletch"};
char32_t const *const UTF32RawLiteral{UR"(foobie\\bletch)"};
wchar_t const *const WideLiteral{L"foobie\\bletch"};
wchar_t const *const WideRawLiteral{LR"(foobie\\bletch)"};

char const *const NewLine{"goink\nfrob\n"};
// CHECK-MESSAGES: :[[@LINE-1]]:27: warning: {{.*}} can be written as a raw string literal
// CHECK-FIXES: {{^}}char const *const NewLine{R"(goink{{$}}
// CHECK-FIXES-NEXT: {{^}}frob{{$}}
// CHECK-FIXES-NEXT: {{^}})"};{{$}}

char const *const SingleQuote{"goink\'frob"};
// CHECK-MESSAGES: :[[@LINE-1]]:31: warning: {{.*}} can be written as a raw string literal
// CHECK-XFIXES: {{^}}char const *const SingleQuote{R"(goink'frob)"};{{$}}

char const *const DoubleQuote{"goink\"frob"};
// CHECK-MESSAGES: :[[@LINE-1]]:31: warning: {{.*}} can be written as a raw string literal
// CHECK-FIXES: {{^}}char const *const DoubleQuote{R"(goink"frob)"};{{$}}

char const *const QuestionMark{"goink\?frob"};
// CHECK-MESSAGES: :[[@LINE-1]]:32: warning: {{.*}} can be written as a raw string literal
// CHECK-FIXES: {{^}}char const *const QuestionMark{R"(goink?frob)"};{{$}}

char const *const RegEx{"goink\\(one|two\\)\\\\\\?.*\\nfrob"};
// CHECK-MESSAGES: :[[@LINE-1]]:25: warning: {{.*}} can be written as a raw string literal
// CHECK-FIXES: {{^}}char const *const RegEx{R"(goink\(one|two\)\\\?.*\nfrob)"};{{$}}

char const *const Path{"C:\\Program Files\\Vendor\\Application\\Application.exe"};
// CHECK-MESSAGES: :[[@LINE-1]]:24: warning: {{.*}} can be written as a raw string literal
// CHECK-FIXES: {{^}}char const *const Path{R"(C:\Program Files\Vendor\Application\Application.exe)"};{{$}}

char const *const ContainsSentinel{"who\\ops)\""};
// CHECK-MESSAGES: :[[@LINE-1]]:36: warning: {{.*}} can be written as a raw string literal
// CHECK-FIXES: {{^}}char const *const ContainsSentinel{R"lit(who\ops)")lit"};{{$}}

char const *const ContainsDelim{"whoops)\")lit\""};
// CHECK-MESSAGES: :[[@LINE-1]]:33: warning: {{.*}} can be written as a raw string literal
// CHECK-FIXES: {{^}}char const *const ContainsDelim{R"lit1(whoops)")lit")lit1"};{{$}}

char const *const OctalPrintable{"\100\\"};
// CHECK-MESSAGES: :[[@LINE-1]]:34: warning: {{.*}} can be written as a raw string literal
// CHECK-FIXES: {{^}}char const *const OctalPrintable{R"(@\)"};{{$}}

char const *const HexPrintable{"\x40\\"};
// CHECK-MESSAGES: :[[@LINE-1]]:32: warning: {{.*}} can be written as a raw string literal
// CHECK-FIXES: {{^}}char const *const HexPrintable{R"(@\)"};{{$}}
