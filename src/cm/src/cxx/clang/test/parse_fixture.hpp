// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file parse_fixture.hpp
/// Contains definition of the parse_fixture class.

#pragma once

#include "cm/src/source_code_model.hpp"
#include <filesystem>


namespace cm::src::clang::test {


/// Fixture for parsing tests
struct parse_fixture {
    source_code_model mdl;

    /// Returns path to test sources
    static std::filesystem::path test_src_path() {
        auto cur_dir = std::filesystem::path{__FILE__}.parent_path();
        return cur_dir / "src";
    }
};


}
