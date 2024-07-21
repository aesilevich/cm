// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

#include "source_file.hpp"
#include <climits>
#include <string>

#pragma once


namespace cm {


/// Represents location in source code
class source_location {
public:
    /// Constructs source location with specified ssource file, line number and column number
    source_location(const source_file * f = nullptr,
                    unsigned int lnum = 0,
                    unsigned int cnum = 0):
        file_{f}, line_num_{lnum}, col_num_{cnum} {}

    /// Returns true if source position is valid
    bool is_valid() const {
        return file_ != nullptr;
    }

    /// Returns true if source position is valid
    operator bool() const {
        return is_valid();
    }

    /// Returns file name
    const source_file * file() const {
        return file_;
    }

    /// Returns line number
    auto line() const {
        return line_num_;
    }

    /// Returns column number
    auto column() const {
        return col_num_;
    }

    /// Prints source location to output stream
    void print(std::ostream & str, bool full_path = false) const {
        assert(is_valid() && "location is invalid");

        if (full_path) {
            str << file_->path().filename();
        } else {
            str << file_->path().string();
        }

        str << ':' << line();

        if (column() != UINT_MAX) {
            str << ':' << column();
        }
    }

private:
    const source_file * file_;  ///< Source file
    unsigned int line_num_;     ///< Line number
    unsigned int col_num_;      ///< Column number
};


/// Prints source location to output stream
inline std::ostream & operator<<(std::ostream & str, const source_location & loc) {
    loc.print(str);
    return str;
}


}
