// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file parse_fixture.hpp
/// Contains definition of the parse_fixture class.

#pragma once

#include "cm/code_model.hpp"
#include <filesystem>
#include <fstream>


namespace cm::clang::test {


/// Fixture for parsing tests
struct parse_fixture {
    code_model mdl;

    /// Returns path to test sources
    static std::filesystem::path test_src_path() {
        auto cur_dir = std::filesystem::path{__FILE__}.parent_path();
        return cur_dir / "src";
    }

    // /// Reads file content to string
    // static std::string read_file(const std::string & nm) {
    //     std::ifstream str{nm};

    //     BOOST_REQUIRE(str.is_open());

    //     std::string res;
    //     while(true) {
    //         char ch;
    //         str.read(&ch, 1);
    //         if (!str) {
    //             break;
    //         }

    //         res.push_back(ch);
    //     }

    //     return res;
    // }
};


}
