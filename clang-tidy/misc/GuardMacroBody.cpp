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

namespace clang {

MacroInfo::tokens_iterator begin(const MacroInfo *Info) {
  return Info->tokens_begin();
}

MacroInfo::tokens_iterator end(const MacroInfo *Info) {
  return Info->tokens_end();
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
  bool isGuarded(const MacroInfo *Info) const;

  GuardMacroBody &Check_;
  SourceManager &SM_;
};

/// \brief Returns true if the given macro is too simple to contain an if
/// statement.
///
/// The minimum number of tokens present in a macro that has a complete if
/// statement is 4: if, (, expr, ).  Therefore any macro with fewer tokens
/// cannot contain an if statement that must be guarded.
bool isSimpleMacro(const MacroInfo *Info) { return Info->getNumTokens() < 4; }

void GuardMacroBodyCallbacks::MacroDefined(const Token &MacroNameTok,
                                           const MacroDirective *MD) {
  const auto MacroNameLoc = MacroNameTok.getLocation();
  if (SM_.getMainFileID() != SM_.getDecomposedLoc(MacroNameLoc).first) {
    return;
  }

  const auto Info = MD->getMacroInfo();
  if (Info->isUsedForHeaderGuard() || isSimpleMacro(Info) || isGuarded(Info)) {
    return;
  }

  Check_.diag(MacroNameLoc, "macro guard needed");
}

/// \brief Returns true for macros containing a do/while (false) guard block.
///
bool GuardMacroBodyCallbacks::isGuarded(const MacroInfo *Info) const {
  enum TokenState {
    NothingYet,
    SeenDo,
    SeenOpenBrace,
    SeenSimpleStatement,
    SeenCloseBrace,
    SeenWhile,
    SeenOpenParen,
    SeenFalse,
    SeenCloseParen,
    Looking
  };
  auto State = TokenState::NothingYet;
  bool SimpleStatementBody = false;
  for (auto T : Info) {
    const auto Kind = T.getKind();
    switch (State) {
    case NothingYet:
      if (Kind == tok::kw_do) {
        State = SeenDo;
      } else if (Kind == tok::kw_if) {
        return false;
      } else {
        State = Looking;
      }
      break;

    case SeenDo:
      if (Kind == tok::l_brace) {
        State = SeenOpenBrace;
      } else {
        State = SeenSimpleStatement;
      }
      break;

    case SeenSimpleStatement:
      SimpleStatementBody = true;
      if (Kind == tok::kw_while) {
        State = SeenWhile;
      }
      break;

    case SeenOpenBrace:
      if (Kind == tok::r_brace) {
        State = SeenCloseBrace;
      }
      break;

    case SeenCloseBrace:
      if (Kind == tok::kw_while) {
        State = SeenWhile;
      } else {
        State = SimpleStatementBody ? SeenSimpleStatement : SeenOpenBrace;
      }
      break;

    case SeenWhile:
      if (Kind == tok::l_paren) {
        State = SeenOpenParen;
      } else {
        State = SimpleStatementBody ? SeenSimpleStatement : SeenOpenBrace;
      }
      break;

    case SeenOpenParen:
      if (Kind == tok::kw_false) {
        State = SeenFalse;
      } else {
        State = SimpleStatementBody ? SeenSimpleStatement : SeenOpenBrace;
      }
      break;

    case SeenFalse:
      if (Kind == tok::r_paren) {
        State = SeenCloseParen;
      } else {
        State = SimpleStatementBody ? SeenSimpleStatement : SeenOpenBrace;
      }
      break;

    case SeenCloseParen:
      if (Kind != tok::semi) {
        State = SimpleStatementBody ? SeenSimpleStatement : SeenOpenBrace;
      }
      break;

    case Looking:
      if (Kind == tok::kw_if) {
        return false;
      }
      break;
    }
  }
  return State == SeenCloseParen || State == Looking;
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
