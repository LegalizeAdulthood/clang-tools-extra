//===--- RedundantFwdDecl.cpp - clang-tidy --------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "RedundantFwdDeclCheck.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace readability {

void RedundantFwdDeclCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(recordDecl(isExpansionInMainFile()).bind("decl"), this);
}

void
RedundantFwdDeclCheck::check(const MatchFinder::MatchResult &Result) {
  if (auto Record = Result.Nodes.getDeclAs<RecordDecl>("decl")) {
    if (std::find(Names_.begin(), Names_.end(), Record->getName()) != Names_.end()) {
      diag(Record->getLocStart(), "redundant forward declaration");
    } else {
      Names_.push_back(Record->getName());
    }
  }
}

} // namespace readability
} // namespace tidy
} // namespace clang
