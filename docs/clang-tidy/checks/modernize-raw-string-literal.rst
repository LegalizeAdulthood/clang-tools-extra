.. title:: clang-tidy - modernize-raw-string-literal

modernize-raw-string-literal
============================

This check replaces string literals containing escaped characters with
raw string literals.

Example:

.. code-blocK:: c++

  const char *const quotes{"embedded \"quotes\""};

becomes

.. code-block:: c++

  const char *const quotes{R"(embedded "quotes")"};

The presence of any of the following escapes cause the string to be
converted to a raw string literal: ``\\``, ``\'``, ``\"``, ``\?``,
and octal or hexadecimal escapes for printable ASCII characters.

If an escaped newline is present in a converted string, it is
replaced with a physical newline.  If an escaped tab or vertical
tab is present in a string, it prevents the string from being
converted.  Unlike a physical newline, the presence of a physical
tab or vertical tab in source code is not visually obvious.
