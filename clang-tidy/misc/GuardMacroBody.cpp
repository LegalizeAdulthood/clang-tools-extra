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
  bool isGuardNeeded(const MacroInfo *) const;

  GuardMacroBody &Check_;
  SourceManager &SM_;
};

void GuardMacroBodyCallbacks::MacroDefined(const Token &MacroNameTok,
                                           const MacroDirective *MD) {
  const auto MacroNameLoc = MacroNameTok.getLocation();
  if (SM_.isInMainFile(MacroNameLoc)) {
    if (isGuardNeeded(MD->getMacroInfo())) {
      Check_.diag(MacroNameLoc, "unguarded macro body");
    }
  }
}

bool GuardMacroBodyCallbacks::isGuardNeeded(const MacroInfo *Info) const {
  for (auto Token : Info) {
    if (Token.getKind() == tok::kw_if) {
      return true;
    }
  }
  return false;
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
