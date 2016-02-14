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
  Finder->addMatcher(recordDecl().bind("decl"), this);
}

void
RedundantFwdDeclCheck::check(const MatchFinder::MatchResult &Result) {
  auto Record = Result.Nodes.getDeclAs<RecordDecl>("decl");
  const std::string Name = Record->getName().str();
  const SourceLocation Loc = Record->getLocStart();
  const auto NameLoc = NameLocs.find(Name);
  if (NameLoc != NameLocs.end())
  {
    if (NameLoc->second != Loc)
      diag(Record->getLocStart(), "redundant forward declaration");
  } else {
    NameLocs[Name] = Loc;
  }
}

} // namespace readability
} // namespace tidy
} // namespace clang
