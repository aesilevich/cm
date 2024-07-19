// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file source_file.hpp
/// Contains definition of the source_file class.

#pragma once

#include <filesystem>


namespace cm {


/// Represents source file in code model
class source_file {
public:
    /// Constructs source file with specified path
    explicit source_file(std::filesystem::path p):
        path_{std::move(p)} {}

    /// Returns source file path
    const std::filesystem::path & path() const { return path_; }

private:
    std::filesystem::path path_;        ///< Source file path
};


}
