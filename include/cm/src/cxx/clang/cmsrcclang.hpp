// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file cmsrcclang.hpp
/// Contains declarations of functions for parsing source code models from source files.

#pragma once

#include "../../source_code_model.hpp"
#include <filesystem>
#include <vector>


namespace cm::src::clang {


/// Parses code model from source file
void parse_source_file(source_code_model & mdl,
                       const std::filesystem::path & path,
                       const std::vector<std::string> & args);


}
