//===--- IncludeOrderCheck.h - clang-tidy -----------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_INCLUDEORDERCHECK_H
#define LLVM_INCLUDEORDERCHECK_H

#include "../ClangTidy.h"

namespace clang {
namespace tidy {
namespace llvm {

/// \brief Checks the correct order of \c #includes.
///
/// see: http://llvm.org/docs/CodingStandards.html#include-style
class IncludeOrderCheck : public ClangTidyCheck {
public:
  IncludeOrderCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerPPCallbacks(CompilerInstance &Compiler) override;
};

} // namespace llvm
} // namespace tidy
} // namespace clang

#endif // LLVM_INCLUDEORDERCHECK_H
