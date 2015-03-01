//===--- GuardMacroBody.h - clang-tidy---------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_GUARD_MACRO_BODY_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_GUARD_MACRO_BODY_H

#include "../ClangTidy.h"

namespace clang {
namespace tidy {
namespace misc {

/// \brief Checks for macro bodies containing statements that need to be guarded
/// in a do/while loop.
///
class GuardMacroBody : public ClangTidyCheck {
public:
  GuardMacroBody(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}

  void registerPPCallbacks(CompilerInstance &Compiler) override;
};

} // namespace misc
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_MISC_GUARD_MACRO_BODY_H
