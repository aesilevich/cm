// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file record_type_debug_info.hpp
/// Contains definition of the record_type_debug_info class.

#pragma once

#include "type_debug_info.hpp"
#include <vector>


namespace cm {

class record_type;


/// Debug info for record types
class record_type_debug_info: public type_debug_info {
public:
    /// Destroys object
    ~record_type_debug_info() override = default;

    /// Returns vector of base classes offsets
    auto & bases_offsets() { return bases_offsets_; }

    /// Returns vector of base classes offsets
    auto & bases_offsets() const { return bases_offsets_; }

    /// Returns vector of offsets of fields
    auto & vars_offsets() { return vars_offsets_; }

    /// Returns vector of offsets in bits of fields
    auto & vars_offsets() const { return vars_offsets_; }

    /// Makes default compact layout of base classes and fields
    /// for specified record type
    void make_default(const record_type * type);

private:
    std::vector<uint64_t> bases_offsets_;       ///< Offsets of base classes
    std::vector<uint64_t> vars_offsets_;        ///< Offsets of fields
};


}
