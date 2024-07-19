// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file value.hpp
/// Contains definition of the value class.

#pragma once

#include <sstream>
#include <string>


namespace cm {


/// Represents compile time value
class value {
public:
    /// Constructs value with specified string representation
    explicit value(const std::string & s):
        str_{s} {}

    /// Constructs value with string representation of int
    explicit value(int v) {
        std::ostringstream str;
        str << v;
        str_ = str.str();
    }

    /// Constructs value with string representation of size_t
    explicit value(size_t v) {
        std::ostringstream str;
        str << v;
        str_ = str.str();
    }

    /// Constructs value with string representation of long long
    explicit value(long long v) {
        std::ostringstream str;
        str << v;
        str_ = str.str();
    }

    /// Default destructor
    virtual ~value() = default;

    /// Returns string representation of value
    auto & str() const { return str_; }

    /// Calculates hash of value
    size_t hash() const { return std::hash<std::string>()(str_); }

    /// Compares two values
    bool operator==(const value & other) const { return str() == other.str(); }

private:
    std::string str_;           ///< String representation of value
};


}
