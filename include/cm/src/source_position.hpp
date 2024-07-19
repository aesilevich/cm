// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file source_position.hpp
/// Contains definition of the source_position class.

#pragma once

#include <climits>
#include <compare>


namespace cm::src {


/// Represents source position
class source_position {
public:
    /// Constructs invalid source position
    source_position():
        line_{UINT_MAX}, column_{UINT_MAX} {}

    /// Constructs source position with specified line and column numbers
    source_position(unsigned int l, unsigned int c):
        line_{l}, column_{c} {}

    /// Returns true if source position is valid
    bool is_valid() const { return line() != UINT_MAX; }

    /// Returns true if source position is valid
    explicit operator bool() const { return is_valid(); }

    /// Returns line number
    unsigned int line() const { return line_; }

    /// Sets line number
    void set_line(unsigned int l) { line_ = l; }

    /// Returns column number
    unsigned int column() const { return column_; }

    /// Sets column number
    void set_column(unsigned int c) { column_ = c; }

    /// Comparison operator
    auto operator<=>(const source_position &) const = default;

private:
    unsigned int line_;         ///< Line number
    unsigned int column_;       ///< Column number
};


}
