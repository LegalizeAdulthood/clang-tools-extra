//===--- RawStringLiteralCheck.cpp - clang-tidy----------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "RawStringLiteralCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Lex/Lexer.h"
#include <iostream>
#include <sstream>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace modernize {

namespace {

bool endsWithNewLine(StringRef Text, size_t NewLinePos) {
  return NewLinePos == Text.size() - 3;
}

bool spaceBeforeNewLine(StringRef Text, size_t NewLinePos) {
  return NewLinePos > 0 && Text[NewLinePos - 1] == ' ';
}

bool contains(StringRef HayStack, StringRef Needle) {
  return HayStack.find(Needle) != StringRef::npos;
}

bool containsEscapedCharacters(const MatchFinder::MatchResult &Result,
                               const StringLiteral *Literal) {
  if (!Literal->isAscii())
    return false;

  StringRef Bytes = Literal->getBytes();
  // Non-printing escapes except newline disqualify this literal:
  // \007 = \a bell
  // \010 = \b backspace
  // \011 = \t horizontal tab
  // \012 = \n new line
  // \013 = \v vertical tab
  // \014 = \f form feed
  // \015 = \r carriage return
  // \177 = delete
  if (Bytes.find_first_of("\001\002\003\004\005\006\a"
                          "\b\t\v\f\r\016\017"
                          "\020\021\022\023\024\025\026\027"
                          "\030\031\032\033\034\035\036\037"
                          "\177") != StringRef::npos)
    return false;

  // \000 = NUL
  if (Bytes.find_first_of(StringRef("\000", 1)) != StringRef::npos)
    return false;

  CharSourceRange CharRange = Lexer::makeFileCharRange(
      CharSourceRange::getTokenRange(Literal->getSourceRange()),
      *Result.SourceManager, Result.Context->getLangOpts());
  StringRef Text = Lexer::getSourceText(CharRange, *Result.SourceManager,
                                        Result.Context->getLangOpts());

  // Already a raw string literal if R comes before ".
  if (Text.find('R') < Text.find('"'))
    return false;

  const size_t NewLinePos = Text.find(R"(\n)");
  if (NewLinePos != StringRef::npos)
    return !endsWithNewLine(Text, NewLinePos) &&
           !spaceBeforeNewLine(Text, NewLinePos);

  return contains(Text, R"(\\)") || contains(Text, R"(\')") ||
         contains(Text, R"(\")") || contains(Text, R"(\?)");
}

bool containsDelimiter(StringRef Bytes, const std::string &Delimiter) {
  return Bytes.find(")" + Delimiter + R"quote(")quote") != StringRef::npos;
}

std::string asRawStringLiteral(const StringLiteral *Literal) {
  const StringRef Bytes = Literal->getBytes();
  std::string Delimiter;
  for (int Counter = 0; containsDelimiter(Bytes, Delimiter); ++Counter) {
    if (Counter == 0) {
      Delimiter = "lit";
    } else {
      std::ostringstream Str;
      Str << "lit" << Counter;
      Delimiter = Str.str();
    }
  }

  return (R"(R")" + Delimiter + "(" + Bytes + ")" + Delimiter + R"(")").str();
}

} // namespace

void RawStringLiteralCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(stringLiteral().bind("lit"), this);
}

void RawStringLiteralCheck::check(const MatchFinder::MatchResult &Result) {
  const LangOptions &Options = Result.Context->getLangOpts();
  // Raw string literals require C++11 or later.
  if (!Options.CPlusPlus && !Options.CPlusPlus11)
    return;

  if (const auto *Literal = Result.Nodes.getNodeAs<StringLiteral>("lit"))
    preferRawStringLiterals(Result, Literal);
}

void RawStringLiteralCheck::preferRawStringLiterals(
    const MatchFinder::MatchResult &Result, const StringLiteral *Literal) {
  if (containsEscapedCharacters(Result, Literal)) {
    replaceWithRawStringLiteral(Result, Literal);
  }
}

void RawStringLiteralCheck::replaceWithRawStringLiteral(
    const MatchFinder::MatchResult &Result, const StringLiteral *Literal) {
  SourceRange ReplacementRange = Literal->getSourceRange();
  CharSourceRange CharRange = Lexer::makeFileCharRange(
      CharSourceRange::getTokenRange(ReplacementRange), *Result.SourceManager,
      Result.Context->getLangOpts());
  StringRef Replacement = asRawStringLiteral(Literal);
  diag(Literal->getLocStart(),
       "escaped string literal can be written as a raw string literal")
      << FixItHint::CreateReplacement(CharRange, Replacement);
}

} // namespace modernize
} // namespace tidy
} // namespace clang
