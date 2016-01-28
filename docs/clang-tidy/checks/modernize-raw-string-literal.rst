.. title:: clang-tidy - modernize-raw-string-literal

modernize-raw-string-literal
============================

This check selectively replaces string literals containing escaped characters
with raw string literals.

Example:

.. code-blocK:: c++

  const char *const Quotes{"embedded \"quotes\""};
  const char *const Paragraph{"Line one.\nLine two.\nLine three.\n"};
  const char *const SingleLine{"Single line.\n"};
  const char *const TrailingSpace{"Look here -> \n"};
  const char *const Tab{"One\tTwo\n"};
  const char *const Bell{"Hello!\a  And welcome!"};
  const char *const Path{"C:\\Program Files\\Vendor\\Application.exe"};
  const char *const RegEx{"\\w\\([a-z]\\)"};

becomes

.. code-block:: c++

  const char *const Quotes{R"(embedded "quotes")"};
  const char *const Paragraph{R"(Line one.
  Line two.
  Line three.
  )"};
  const char *const SingleLine{"Single line.\n"};
  const char *const TrailingSpace{"Look here -> \n"};
  const char *const Tab{"One\tTwo\n"};
  const char *const Bell{"Hello!\a  And welcome!"};
  const char *const Path{R"(C:\Program Files\Vendor\Application.exe)"};
  const char *const RegEx{R"(\w\([a-z]\))"};

The presence of any of the following escapes can cause the string to be
converted to a raw string literal: ``\\``, ``\'``, ``\"``, ``\?``,
and octal or hexadecimal escapes for printable ASCII characters.

If an escaped newline is present in the middle a string literal, it is
replaced with a physical newline, unless the newline is preceded by a space.
Trailing whitespace at the end of a source code line is not visible in an
editor.  Trailing whitespace is likely to be stripped by editors and other
tools, changing the meaning of the literal.

A string literal containing only a single escaped newline at the end is a
common way of describing a line of text output.  Introducing physical newlines
with raw string literals in this case is likely to impede readability.  These
string literals are left unchanged.

An escaped horizontal tab, form feed, or vertical tab prevents the string
literal from being converted.  Unlike a physical newline, the presence of a
horizontal tab, form feed or vertical tab in source code is not visually
obvious.
