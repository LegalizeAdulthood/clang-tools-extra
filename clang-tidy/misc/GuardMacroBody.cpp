//===--- GuardMacroBody.cpp - clang-tidy-------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "GuardMacroBody.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/MacroInfo.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/Token.h"
#include <algorithm>
#include <utility>

namespace clang {

MacroInfo::tokens_iterator begin(const MacroInfo &Info) {
  return Info.tokens_begin();
}

MacroInfo::tokens_iterator end(const MacroInfo &Info) {
  return Info.tokens_end();
}

namespace tidy {
namespace misc {

namespace {

class GuardMacroBodyCallbacks : public PPCallbacks {
public:
  GuardMacroBodyCallbacks(GuardMacroBody &Check, SourceManager &SM)
      : Check_(Check), SM_(SM) {}

  void MacroDefined(const Token &MacroNameTok,
                    const MacroDirective *MD) override;

private:
  GuardMacroBody &Check_;
  SourceManager &SM_;
};

/// \brief Returns true if the given macro is too simple to contain an if
/// statement.
///
/// The minimum number of tokens present in a macro that has a complete if
/// statement is 4: if, (, expr, ).  Therefore any macro with fewer tokens
/// cannot contain an if statement that must be guarded.
bool isSimpleMacro(const MacroInfo &Info) { return Info.getNumTokens() < 4; }

tok::TokenKind getTokenKind(const MacroInfo &Info, unsigned Tok) {
  return Info.getReplacementToken(Tok).getKind();
}

bool isLastTokenSemiColon(const MacroInfo &Info) {
  return Info.getReplacementToken(Info.getNumTokens() - 1).getKind() ==
         tok::semi;
}

bool hasMultipleStatements(const MacroInfo &Info) {
  return std::count_if(
             Info.tokens_begin(), Info.tokens_end(),
             [](Token const &T) { return T.getKind() == tok::semi; }) > 1;
}

/// \brief Returns true for macros containing a do/while (false) guard block,
/// those enclosed in double braces, or those simple enough not to consider
/// needing a guard.
///
bool isGuarded(const MacroInfo &Info) {
  const auto LastIndex =
      Info.getNumTokens() - (isLastTokenSemiColon(Info) ? 2 : 1);
  if (getTokenKind(Info, 0) == tok::kw_do &&
      getTokenKind(Info, LastIndex - 3) == tok::kw_while &&
      getTokenKind(Info, LastIndex - 2) == tok::l_paren &&
      getTokenKind(Info, LastIndex - 1) == tok::kw_false &&
      getTokenKind(Info, LastIndex - 0) == tok::r_paren) {
    return true;
  } else if (getTokenKind(Info, 0) == tok::l_brace &&
             getTokenKind(Info, LastIndex) == tok::r_brace) {
    return true;
  } else if (getTokenKind(Info, 0) == tok::kw_if ||
             hasMultipleStatements(Info)) {
    return false;
  }

  // anything else we assume doesn't need a guard
  return true;
}

SourceLocation tokenEndLoc(const Token &T) {
  return T.getLocation().getLocWithOffset(T.getLength());
}

std::pair<SourceLocation, SourceLocation>
getInsertionLocations(const MacroInfo &Info) {
  const auto Start = Info.getReplacementToken(0).getLocation();
  const auto EndTokenIndex =
      Info.getNumTokens() - (isLastTokenSemiColon(Info) ? 2 : 1);
  auto EndToken = Info.getReplacementToken(EndTokenIndex);
  return std::make_pair(Start, tokenEndLoc(EndToken));
}

void GuardMacroBodyCallbacks::MacroDefined(const Token &MacroNameTok,
                                           const MacroDirective *MD) {
  const auto MacroNameLoc = MacroNameTok.getLocation();
  if (SM_.getMainFileID() != SM_.getDecomposedLoc(MacroNameLoc).first) {
    return;
  }

  const auto &Info = *MD->getMacroInfo();
  if (Info.isUsedForHeaderGuard() || isSimpleMacro(Info) || isGuarded(Info)) {
    return;
  }

  const auto InsertionLocs = getInsertionLocations(Info);
  Check_.diag(MacroNameLoc, "macro guard needed")
      << FixItHint::CreateInsertion(InsertionLocs.first, "do { ")
      << FixItHint::CreateInsertion(InsertionLocs.second, "; } while (false)");
}

} // namespace

void GuardMacroBody::registerPPCallbacks(CompilerInstance &Compiler) {
  Compiler.getPreprocessor().addPPCallbacks(
      llvm::make_unique<GuardMacroBodyCallbacks>(*this,
                                                 Compiler.getSourceManager()));
}

} // namespace misc
} // namespace tidy
} // namespace clang
