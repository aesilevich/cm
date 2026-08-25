// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file types_converter.hpp
/// Contains definition of the types_converter class.

#pragma once

#include "array_type_converter.hpp"
#include "builtin_type_converter.hpp"
#include "converter_impl.hpp"
#include "decltype_type_converter.hpp"
#include "dependent_type_converter.hpp"
#include "function_type_converter.hpp"
#include "log.hpp"
#include "lvalue_reference_type_converter.hpp"
#include "pack_expansion_type_converter.hpp"
#include "rvalue_reference_type_converter.hpp"
#include "pointer_type_converter.hpp"
#include "subst_template_type_param_type_converter.hpp"
#include "template_specialization_type_converter.hpp"
#include "type_converter.hpp"
#include "type_template_parameter_converter.hpp"
#include <tuple>


namespace cm::cxx::clang {

class converter_impl;


/// Types converter. Converts clang types to code model types using list of converters
/// for individual types.
class types_converter {
    /// Tuple of type converters
    using type_converters = std::tuple <
        builtin_type_converter,
        pointer_type_converter,
        pack_expansion_type_converter,
        lvalue_reference_type_converter,
        rvalue_reference_type_converter,
        function_type_converter,
        array_type_converter,
        type_template_parameter_converter,
        template_specialization_type_converter,
        subst_template_type_param_type_converter,
        dependent_type_converter,
        decltype_type_converter
    >;
public:
    /// Constructs types converter
    explicit types_converter(converter_impl & conv):
        conv_{conv} {}

    /// Gets existing or creates new code model type for clang type using type converters
    type_t * type(const ::clang::Type * type) {
        return type_impl<0>(type);
    }

private:
    /// Gets existing or creates new code model type for clang type using type converters
    /// starting from index I
    template <size_t I>
    type_t * type_impl(const ::clang::Type * type) {
        if constexpr (I == std::tuple_size_v<type_converters>) {
            // matching converter not found
            CM_CLANG_LOG_SCAT_TYPE_ERROR(decl, "unknown clang type", type, conv_.clang_ctx());
            // TODO: assert(false) after converters for all types are implemented
            assert(false && "unknown clang type");
            return nullptr;
        } else {
            // trying current converter
            using converter_t = std::tuple_element_t<I, type_converters>;
            if (auto conv_type = ::clang::dyn_cast<typename converter_t::clang_type_t>(type)) {
                auto & conv = std::get<I>(convs_);
                if (conv.match(conv_type)) {
                    return conv.type(conv_, conv_type);
                }
            }

            // trying next converters
            return type_impl<I + 1>(type);
        }
    }


    converter_impl & conv_;             ///< Reference to converter instance
    type_converters convs_;             ///< Tuple of type converters
};


}
