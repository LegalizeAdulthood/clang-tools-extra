//===--- IncludeBlockPPCallbacks.cpp - clang-tidy -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "IncludeBlockPPCallbacks.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/Preprocessor.h"

namespace clang {
namespace tidy {

void IncludeBlockPPCallbacks::InclusionDirective(
    SourceLocation HashLoc, const Token &IncludeTok, StringRef FileName,
    bool IsAngled, CharSourceRange FilenameRange, const FileEntry *File,
    StringRef SearchPath, StringRef RelativePath, const Module *Imported) {
  // We recognize the first include as a special main module header and want
  // to leave it in the top position.
  IncludeDirective ID = {HashLoc, FilenameRange, FileName, IsAngled, false};
  if (LookForMainModule && !IsAngled) {
    ID.IsMainModule = true;
    LookForMainModule = false;
  }
  IncludeDirectives.push_back(std::move(ID));
}

void IncludeBlockPPCallbacks::EndOfMainFile() {
  LookForMainModule = true;
  if (IncludeDirectives.empty())
    return;

  // Form blocks of includes. We don't want to sort across blocks. This also
  // implicitly makes us never reorder over #defines or #if directives.
  // FIXME: We should be more careful about sorting below comments as we don't
  // know if the comment refers to the next include or the whole block that
  // follows.
  std::vector<unsigned> Blocks(1, 0);
  for (unsigned I = 1, E = IncludeDirectives.size(); I != E; ++I)
    if (SM.getExpansionLineNumber(IncludeDirectives[I].Loc) !=
        SM.getExpansionLineNumber(IncludeDirectives[I - 1].Loc) + 1)
      Blocks.push_back(I);
  Blocks.push_back(IncludeDirectives.size()); // Sentinel value.

  processIncludeBlocks(Blocks);

  IncludeDirectives.clear();
}

} // namespace tidy
} // namespace clang
