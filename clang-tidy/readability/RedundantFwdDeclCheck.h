//===--- RedundantFwdDecl.h - clang-tidy-------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_READABILITY_REDUNDANT_FWD_DECL_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_READABILITY_REDUNDANT_FWD_DECL_H

#include "../ClangTidy.h"
#include <map>
#include <string>
#include <vector>

namespace clang {
namespace tidy {
namespace readability {

/// \brief Find and remove redundant forward declarations.
class RedundantFwdDeclCheck : public ClangTidyCheck {
public:
  RedundantFwdDeclCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
  std::map<std::string, SourceLocation> NameLocs;
};

} // namespace readability
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_READABILITY_REDUNDANT_FWD_DECL_H
