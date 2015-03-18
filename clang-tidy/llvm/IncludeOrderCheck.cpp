//===--- IncludeOrderCheck.cpp - clang-tidy -------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "IncludeOrderCheck.h"
#include "../utils/IncludeBlockPPCallbacks.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Preprocessor.h"

namespace clang {
namespace tidy {
namespace llvm {

namespace {
class IncludeOrderPPCallbacks : public IncludeBlockPPCallbacks {
public:
  explicit IncludeOrderPPCallbacks(ClangTidyCheck &Check, SourceManager &SM)
      : IncludeBlockPPCallbacks(Check, SM) {}

protected:
  void processIncludeBlocks(const std::vector<unsigned> &Blocks) override;
};
} // namespace

void IncludeOrderCheck::registerPPCallbacks(CompilerInstance &Compiler) {
  Compiler.getPreprocessor().addPPCallbacks(
      ::llvm::make_unique<IncludeOrderPPCallbacks>(
          *this, Compiler.getSourceManager()));
}

static int getPriority(StringRef Filename, bool IsAngled, bool IsMainModule) {
  // We leave the main module header at the top.
  if (IsMainModule)
    return 0;

  // LLVM and clang headers are in the penultimate position.
  if (Filename.startswith("llvm/") || Filename.startswith("llvm-c/") ||
      Filename.startswith("clang/") || Filename.startswith("clang-c/"))
    return 2;

  // System headers are sorted to the end.
  if (IsAngled || Filename.startswith("gtest/"))
    return 3;

  // Other headers are inserted between the main module header and LLVM headers.
  return 1;
}

// We don't want to sort across blocks. This also implicitly makes us never
// reorder over #defines or #if directives.
// FIXME: We should be more careful about sorting below comments as we don't
// know if the comment refers to the next include or the whole block that
// follows.
void IncludeOrderPPCallbacks::processIncludeBlocks(const std::vector<unsigned> &Blocks) {
  // Get a vector of indices.
  std::vector<unsigned> IncludeIndices;
  for (unsigned I = 0, E = IncludeDirectives.size(); I != E; ++I)
    IncludeIndices.push_back(I);

  // Sort the includes. We first sort by priority, then lexicographically.
  for (unsigned BI = 0, BE = Blocks.size() - 1; BI != BE; ++BI)
    std::sort(IncludeIndices.begin() + Blocks[BI],
              IncludeIndices.begin() + Blocks[BI + 1],
              [this](unsigned LHSI, unsigned RHSI) {
      IncludeDirective &LHS = IncludeDirectives[LHSI];
      IncludeDirective &RHS = IncludeDirectives[RHSI];

      int PriorityLHS =
          getPriority(LHS.Filename, LHS.IsAngled, LHS.IsMainModule);
      int PriorityRHS =
          getPriority(RHS.Filename, RHS.IsAngled, RHS.IsMainModule);

      return std::tie(PriorityLHS, LHS.Filename) <
             std::tie(PriorityRHS, RHS.Filename);
    });

  // Emit a warning for each block and fixits for all changes within that block.
  for (unsigned BI = 0, BE = Blocks.size() - 1; BI != BE; ++BI) {
    // Find the first include that's not in the right position.
    unsigned I, E;
    for (I = Blocks[BI], E = Blocks[BI + 1]; I != E; ++I)
      if (IncludeIndices[I] != I)
        break;

    if (I == E)
      continue;

    // Emit a warning.
    auto D = Check.diag(IncludeDirectives[I].Loc,
                        "#includes are not sorted properly");

    // Emit fix-its for all following includes in this block.
    for (; I != E; ++I) {
      if (IncludeIndices[I] == I)
        continue;
      const IncludeDirective &CopyFrom = IncludeDirectives[IncludeIndices[I]];

      SourceLocation FromLoc = CopyFrom.Range.getBegin();
      const char *FromData = SM.getCharacterData(FromLoc);
      unsigned FromLen = std::strcspn(FromData, "\n");

      StringRef FixedName(FromData, FromLen);

      SourceLocation ToLoc = IncludeDirectives[I].Range.getBegin();
      const char *ToData = SM.getCharacterData(ToLoc);
      unsigned ToLen = std::strcspn(ToData, "\n");
      auto ToRange =
          CharSourceRange::getCharRange(ToLoc, ToLoc.getLocWithOffset(ToLen));

      D << FixItHint::CreateReplacement(ToRange, FixedName);
    }
  }

  IncludeDirectives.clear();
}

} // namespace llvm
} // namespace tidy
} // namespace clang
