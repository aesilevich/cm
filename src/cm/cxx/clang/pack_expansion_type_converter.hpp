// Copyright (c) 2024, Alexandr Esilevich
//
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file pack_expansion_type_converter.hpp
/// Contains definition of pack_expansion_type_converter class

#pragma once

#include "type_converter.hpp"


namespace cm::cxx::clang {


/// Pack expansion type converter
struct pack_expansion_type_converter {
    /// Type of clang type
    using clang_type_t = ::clang::PackExpansionType;

    /// Checks if clang type matches converter
    static constexpr bool match(const clang_type_t * type) { return true; }

    /// Gets existing or creates new code model type for clang type
    static pack_expansion_type * type(converter_impl & conv, const clang_type_t * type) {
        auto pattern_type = conv.type(type->getPattern());
        return conv.mdl().get_or_create_pack_expansion_type(pattern_type);
    }
};


static_assert(type_converter<pack_expansion_type_converter>);


}
