// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file cmclang.cpp
/// Contains implementation of functions for parsing code models from source files.

#include "cm/cxx/clang/cmclang.hpp"
#include "cm/cxx/clang/ast_converter.hpp"
#include <clang-c/Index.h>
//#include <clang/tools/libclang/CXTranslationUnit.h>
#include <clang/Frontend/ASTUnit.h>
#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>


namespace cm::clang {


/// Helper class for accessing TheASTUnit member from CXTranslationUnit instance
struct CXTranslationUnitImpl {
  void * CIdx;
  ::clang::ASTUnit * TheASTUnit;
};


void parse_source_file(code_model & mdl,
                       const std::filesystem::path & path,
                       const std::vector<std::string> & args) {
    // creating clang index
    auto clang_idx = ::clang_createIndex(0, 1);

    // converting command line arguments
    std::vector<const char*> c_args;
    for (auto && arg : args) {
        c_args.push_back(arg.c_str());
    }
    

    // parsing source file
    auto tu = ::clang_parseTranslationUnit(clang_idx,
                                           path.string().c_str(),
                                           c_args.data(),
                                           c_args.size(),
                                           nullptr,     // unsaved files
                                           0,           // number of unsaved files
                                           0);          // options

    // checking for parse errors
    if (!tu) {
        std::ostringstream msg;
        msg << "can't parse source file '" << path.string() << "'";
        throw std::runtime_error(msg.str());
    }

    // creating AST converter and converting AST to code model
    {
        // getting AST contetx from translation unit
        // TODO: try avoid this hack
        auto & ctx = reinterpret_cast<CXTranslationUnitImpl*>(tu)->TheASTUnit->getASTContext();

        ast_converter ast_conv{mdl};
        ast_conv.convert(ctx);
    }

    // removing translation unit
    ::clang_disposeTranslationUnit(tu);

    // removing clang index
    ::clang_disposeIndex(clang_idx);
}


}
