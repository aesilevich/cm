// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file debug_symbol.hpp
/// Contains definition of the debug_symbol class.

#pragma once

#include <cstdint>
#include <string>


namespace cm {


/// Represents symbol located in executable
class debug_symbol {
public:
    /// Constructs debug symbol with specified name, address, and size
    debug_symbol(const std::string & nm, uint64_t a, uint64_t sz):
        name_{nm}, addr_{a}, size_{sz} {}

    /// Returns symbol name
    auto & name() const { return name_; }

    /// Sets symbol name
    void set_name(const std::string & nm) { name_ = nm; }

    /// Returns symbol address
    auto addr() const { return addr_; }

    /// Sets symbol address
    void set_addr(uint64_t a) { addr_ = a; }

    /// Returns symbol size
    auto size() const { return size_; }

    /// Sets symbol size
    void set_size(uint64_t sz) { size_ = sz; }

private:
    std::string name_;      ///< Symbol name
    uint64_t addr_;         ///< Symbol address
    uint64_t size_;         ///< Symbol size
};


}
