// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file source_range.hpp
/// Contains definition of the source_range class.

#pragma once

#include "source_position.hpp"
#include <cassert>
#include <ostream>


namespace cm::src {


/// Represents range in a source file
class source_range {
public:
    /// Constructs source range with specified start and end positions
    source_range(const source_position & s = {}, const source_position & e = {}):
    start_{s}, end_{e} {
        assert(start_ <= end_ && "invalid source range start/end");
    }

    /// Returns start position of source range
    const source_position & start() const { return start_; }

    /// Sets start position of source range
    void set_start(const source_position & pos) {
        assert(pos <= end_ && "invalid source range start");
        start_ = pos;
    }

    /// Returns end position of source range
    const source_position & end() const { return end_; }

    /// Sets end position of source range
    void set_end(const source_position & pos) {
        assert(start_ <= pos && "invalid soure range end");
        end_ = pos;
    }

    /// Prints range to output stream
    void print(std::ostream & str) const {
        str << "("
                << start().line() << ", "
                << start().column() << ")";

        str << " - ";

        str << "("
                << end().line() << ", "
                << end().column() << ")";
    }

private:
    source_position start_;     ///< Start position
    source_position end_;       ///< End position
};


/// Prints source range to output stream
inline std::ostream & operator<<(std::ostream & str, const source_range & r) {
    r.print(str);
    return str;
}


}
