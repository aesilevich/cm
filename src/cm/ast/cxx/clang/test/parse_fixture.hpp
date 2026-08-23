// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file parse_fixture.hpp
/// Contains definition of the parse_fixture class.

#pragma once

#include "cm/ast/ast_model.hpp"
#include <filesystem>


namespace cm::ast::cxx::clang::test {


/// Fixture for parsing tests
struct parse_fixture {
    ast_model mdl;

    /// Returns path to test sources
    static std::filesystem::path test_src_path() {
        auto cur_dir = std::filesystem::path{__FILE__}.parent_path();
        return cur_dir / "src";
    }
};


}
