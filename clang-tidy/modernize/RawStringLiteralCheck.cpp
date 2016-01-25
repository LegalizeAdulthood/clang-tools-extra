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
#include <sstream>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace modernize {

namespace {

bool containsEscapedCharacters(const MatchFinder::MatchResult &Result,
                               const StringLiteral *Literal) {
  if (Literal->getBytes().find_first_of("\a\b\f\r\t\v") != StringRef::npos)
    return false;

  CharSourceRange CharRange = Lexer::makeFileCharRange(
      CharSourceRange::getTokenRange(Literal->getSourceRange()),
      *Result.SourceManager, Result.Context->getLangOpts());
  StringRef Text = Lexer::getSourceText(CharRange, *Result.SourceManager,
                                        Result.Context->getLangOpts());
  const bool HasBackSlash = Text.find(R"(\\)") != StringRef::npos;
  const bool HasNewLine = Text.find(R"(\n)") != StringRef::npos;
  const bool HasQuote = Text.find(R"(\')") != StringRef::npos;
  const bool HasDoubleQuote = Text.find(R"(\")") != StringRef::npos;
  const bool HasQuestion = Text.find(R"(\?)") != StringRef::npos;
  return HasBackSlash || HasNewLine || HasQuote || HasDoubleQuote ||
         HasQuestion;
}

bool containsDelimiter(StringRef Bytes, const std::string &Delimiter) {
  return Bytes.find(")" + Delimiter + R"(")") != StringRef::npos;
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
  if (const auto *Literal = Result.Nodes.getNodeAs<StringLiteral>("lit")) {
    preferRawStringLiterals(Result, Literal);
  }
}

void RawStringLiteralCheck::preferRawStringLiterals(
    const MatchFinder::MatchResult &Result, const StringLiteral *Literal) {
  if (containsEscapedCharacters(Result, Literal)) {
    SourceRange ReplacementRange = Literal->getSourceRange();
    CharSourceRange CharRange = Lexer::makeFileCharRange(
        CharSourceRange::getTokenRange(ReplacementRange), *Result.SourceManager,
        Result.Context->getLangOpts());
    StringRef Replacement = asRawStringLiteral(Literal);
    diag(Literal->getLocStart(),
         "escaped string literal can be written as a raw string literal")
        << FixItHint::CreateReplacement(CharRange, Replacement);
  }
}

} // namespace modernize
} // namespace tidy
} // namespace clang
