//===--- RedundantInclude.h - clang-tidy-------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_READABILITY_REDUNDANT_INCLUDE_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_READABILITY_REDUNDANT_INCLUDE_H

#include <clang/Frontend/CompilerInstance.h>
#include "../ClangTidy.h"

namespace clang {
namespace tidy {
namespace readability {

/// \brief Find and remove redundant #include directives.
///
/// Any change in the defined macros in the main file between \c #include
/// directives resets the list of includes used to check for duplicates.
class RedundantInclude : public ClangTidyCheck {
public:
  RedundantInclude(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}

  void registerPPCallbacks(CompilerInstance &Compiler) override;
};

} // namespace readability
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_READABILITY_REDUNDANT_INCLUDE_H
