//===--- DuplicateIncludeCheck.h - clang-tidy--------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_READABILITY_DUPLICATE_INCLUDE_CHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_READABILITY_DUPLICATE_INCLUDE_CHECK_H

#include <clang/Frontend/CompilerInstance.h>
#include "../ClangTidy.h"

namespace clang {
namespace tidy {
namespace readability {

/// \brief Find and remove duplicate #include directives.
///
/// Only consecutive include directives without any other preprocessor
/// directives between them are analyzed.
class DuplicateIncludeCheck : public ClangTidyCheck {
public:
  DuplicateIncludeCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}

  void registerPPCallbacks(CompilerInstance &Compiler) override;
};

} // namespace readability
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_READABILITY_DUPLICATE_INCLUDE_CHECK_H
