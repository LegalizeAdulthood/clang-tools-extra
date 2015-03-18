//===--- IncludeBlockPPCallbacks.h - clang-tidy -----------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_UTILS_INCLUDEBLOCKPPCALLBACKS_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_UTILS_INCLUDEBLOCKPPCALLBACKS_H

#include "clang/Lex/PPCallbacks.h"
#include <vector>

namespace clang {

class SourceManager;

namespace tidy {

class ClangTidyCheck;

class IncludeBlockPPCallbacks : public PPCallbacks {
public:
  explicit IncludeBlockPPCallbacks(ClangTidyCheck &Check, SourceManager &SM)
      : LookForMainModule(true), Check(Check), SM(SM) {}

  void InclusionDirective(SourceLocation HashLoc, const Token &IncludeTok,
                          StringRef FileName, bool IsAngled,
                          CharSourceRange FilenameRange, const FileEntry *File,
                          StringRef SearchPath, StringRef RelativePath,
                          const Module *Imported) override;
  void EndOfMainFile() override;

protected:
  virtual void processIncludeBlocks(const std::vector<unsigned> &Blocks) = 0;

  struct IncludeDirective {
    SourceLocation Loc;    ///< '#' location in the include directive
    CharSourceRange Range; ///< SourceRange for the file name
    StringRef Filename;    ///< Filename as a string
    bool IsAngled;         ///< true if this was an include with angle brackets
    bool IsMainModule;     ///< true if this was the first include in a file
  };
  std::vector<IncludeDirective> IncludeDirectives;
  bool LookForMainModule;

  ClangTidyCheck &Check;
  SourceManager &SM;
};

} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_UTILS_INCLUDEBLOCKPPCALLBACKS_H
