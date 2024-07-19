// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file cmclang.hpp
/// Contains declarations of functions for parsing code models from source files.

#pragma once

#include "../../code_model.hpp"
#include <filesystem>
#include <vector>


namespace cm::clang {


/// Parses code model from source file
void parse_source_file(code_model & mdl,
                       const std::filesystem::path & path,
                       const std::vector<std::string> & args);


}
