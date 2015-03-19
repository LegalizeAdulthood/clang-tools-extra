//===--- DuplicateIncludeCheck.cpp - clang-tidy------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "DuplicateIncludeCheck.h"
#include "../ClangTidy.h"
#include "../utils/IncludeBlockPPCallbacks.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Preprocessor.h"
#include <algorithm>
#include <vector>

namespace clang {
namespace tidy {
namespace readability {

namespace {

SourceLocation advanceBeyondCurrentLine(SourceManager &SM, SourceLocation Start,
                                        int Offset) {
  const FileID Id = SM.getFileID(Start);
  const unsigned LineNumber = SM.getSpellingLineNumber(Start);
  while (SM.getFileID(Start) == Id &&
         SM.getSpellingLineNumber(Start.getLocWithOffset(Offset)) ==
             LineNumber) {
    Start = Start.getLocWithOffset(Offset);
  }
  return Start;
}

class DuplicateIncludeCallbacks : public PPCallbacks {
public:
  DuplicateIncludeCallbacks(DuplicateIncludeCheck &Check, SourceManager &SM)
      : Check(Check), SM(SM) {}

  void InclusionDirective(SourceLocation HashLoc, const Token &IncludeTok,
                          StringRef FileName, bool IsAngled,
                          CharSourceRange FilenameRange, const FileEntry *File,
                          StringRef SearchPath, StringRef RelativePath,
                          const Module *Imported) override;

  void MacroDefined(const Token &MacroNameTok,
                    const MacroDirective *MD) override;

  void MacroUndefined(const Token &MacroNameTok,
                      const MacroDirective *MD) override;

private:
  std::vector<StringRef> Files;
  DuplicateIncludeCheck &Check;
  SourceManager &SM;
};

void DuplicateIncludeCallbacks::InclusionDirective(
    SourceLocation HashLoc, const Token &IncludeTok, StringRef FileName,
    bool IsAngled, CharSourceRange FilenameRange, const FileEntry *File,
    StringRef SearchPath, StringRef RelativePath, const Module *Imported) {
  if (!SM.isInMainFile(HashLoc)) {
    return;
  }

  if (std::find(Files.cbegin(), Files.cend(), FileName) != Files.end()) {
    const SourceLocation Start =
        advanceBeyondCurrentLine(SM, HashLoc, -1).getLocWithOffset(-1);
    const SourceLocation End =
        advanceBeyondCurrentLine(SM, FilenameRange.getEnd(), 1);
    Check.diag(HashLoc, "duplicate include")
        << FixItHint::CreateRemoval(SourceRange(Start, End));
  } else {
    Files.push_back(FileName);
  }
}

void DuplicateIncludeCallbacks::MacroDefined(const Token &MacroNameTok,
                                             const MacroDirective *MD) {
  if (SM.isInMainFile(MacroNameTok.getLocation())) {
    Files.clear();
  }
}

void DuplicateIncludeCallbacks::MacroUndefined(const Token &MacroNameTok,
                                               const MacroDirective *MD) {
  if (SM.isInMainFile(MacroNameTok.getLocation())) {
    Files.clear();
  }
}

} // namespace

void DuplicateIncludeCheck::registerPPCallbacks(CompilerInstance &Compiler) {
  Compiler.getPreprocessor().addPPCallbacks(
      llvm::make_unique<DuplicateIncludeCallbacks>(
          *this, Compiler.getSourceManager()));
}

} // namespace readability
} // namespace tidy
} // namespace clang
