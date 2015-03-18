set(LLVM_LINK_COMPONENTS support)

add_clang_library(clangTidyUtils
  HeaderGuard.cpp
  IncludeBlockPPCallbacks.cpp

  LINK_LIBS
  clangAST
  clangASTMatchers
  clangBasic
  clangLex
  clangTidy
  )
