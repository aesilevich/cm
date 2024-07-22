// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file source_file_range.hpp
/// Contains definition of the source_file_range class.

#pragma once

#include "source_range.hpp"
#include "source_file_position.hpp"
#include "../cm.hpp"


namespace cm::src {


/// Pair of source file and range in it
class source_file_range {
public:
    /// Constructs source file range with specified pointer to source file
    /// and range in it
    source_file_range(const cm::source_file * file = nullptr, const source_range & r = {}):
        file_{file}, range_{r} {}

    /// Returns true if source file range is valid
    bool is_valid() const { return file() != nullptr; }

    /// Returns true if source file range is valid
    explicit operator bool() const { return is_valid(); }

    /// Returns pointer to source file
    const cm::source_file * file() const { return file_; }

    /// Returns const reference to range in source file
    const source_range & range() const { return range_; }

    /// Returns reference to range in source file
    source_range & range() { return range_; }

    /// Sets range in source file
    void set_range(const source_range & r) { range_ = r; }

    /// Prints range to output stream
    void print(std::ostream & str, bool full_src_path = true) const {
        str << "[";
        if (full_src_path) {
            str << file()->path();
        } else {
            str << file()->path().filename();
        }

        str << ": " << range() << ']';
    }

private:
    const cm::source_file * file_;  ///< Pointer to source file object
    source_range range_;            ///< Range in source file
};


/// Prints range to output stream
inline std::ostream & operator<<(std::ostream & str, const source_file_range & range) {
    range.print(str);
    return str;
}


}
