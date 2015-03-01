//===--- RedundantInclude.cpp - clang-tidy-----------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "RedundantInclude.h"
#include "../ClangTidy.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Preprocessor.h"
#include <algorithm>
#include <vector>
#include <sstream>

namespace clang {
namespace tidy {
namespace readability {

namespace {

SourceLocation AdvanceBeyondCurrentLine(SourceManager &SM, SourceLocation Start,
                                        int Offset) {
  const auto Id = SM.getFileID(Start);
  const auto LineNumber = SM.getSpellingLineNumber(Start);
  while (SM.getFileID(Start) == Id &&
         SM.getSpellingLineNumber(Start.getLocWithOffset(Offset)) ==
             LineNumber) {
    Start = Start.getLocWithOffset(Offset);
  }
  return Start;
}

class RedundantIncludeCallbacks : public PPCallbacks {
public:
  RedundantIncludeCallbacks(RedundantInclude &Check, SourceManager &SM)
      : Check_(Check), SM_(SM) {}

  void InclusionDirective(SourceLocation HashLoc, const Token &IncludeTok,
                          StringRef FileName, bool IsAngled,
                          CharSourceRange FilenameRange, const FileEntry *File,
                          StringRef SearchPath, StringRef RelativePath,
                          const Module *Imported) override;

private:
  std::vector<StringRef> Files_;
  RedundantInclude &Check_;
  SourceManager &SM_;
};

void RedundantIncludeCallbacks::InclusionDirective(
    SourceLocation HashLoc, const Token &IncludeTok, StringRef FileName,
    bool IsAngled, CharSourceRange FilenameRange, const FileEntry *File,
    StringRef SearchPath, StringRef RelativePath, const Module *Imported) {
  if (!SM_.isInMainFile(HashLoc)) {
    return;
  }

  if (std::find(Files_.cbegin(), Files_.cend(), FileName) != Files_.end()) {
    const auto Start =
        AdvanceBeyondCurrentLine(SM_, HashLoc, -1).getLocWithOffset(-1);
    const auto End = AdvanceBeyondCurrentLine(SM_, FilenameRange.getEnd(), 1);
    Check_.diag(HashLoc, "redundant include")
        << FixItHint::CreateRemoval(SourceRange(Start, End));
  } else {
    Files_.push_back(FileName);
  }
}

} // namespace

void RedundantInclude::registerPPCallbacks(CompilerInstance &Compiler) {
  Compiler.getPreprocessor().addPPCallbacks(
      llvm::make_unique<RedundantIncludeCallbacks>(
          *this, Compiler.getSourceManager()));
}

} // namespace readability
} // namespace tidy
} // namespace clang
