// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file array_type_converter.hpp
/// Contains definition of array_type_converter class

#pragma once

#include "type_converter.hpp"


namespace cm::cxx::clang {


/// Array type converter. Converts all kinds of clang array types.
struct array_type_converter {
    /// Type of clang type
    using clang_type_t = ::clang::ArrayType;

    /// Checks if clang type matches converter
    static constexpr bool match(const clang_type_t * type) { return true; }

    /// Gets existing or creates new code model type for clang type
    static array_type * type(converter_impl & conv, const clang_type_t * type) {
        auto elt_type = conv.type(type->getElementType()).type();

        if (auto const_type = ::clang::dyn_cast<::clang::ConstantArrayType>(type)) {
            return conv.mdl().get_or_create_fixed_arr_type(
                elt_type, const_type->getSize().getLimitedValue());

        } else if (::clang::isa<::clang::IncompleteArrayType>(type)) {
            return conv.mdl().get_or_create_incomplete_arr_type(elt_type);

        } else if (auto var_type = ::clang::dyn_cast<::clang::VariableArrayType>(type)) {
            return conv.mdl().get_or_create_dynamic_arr_type(
                elt_type, size_value(conv, var_type->getSizeExpr()));

        } else if (auto dep_type = ::clang::dyn_cast<::clang::DependentSizedArrayType>(type)) {
            return conv.mdl().get_or_create_dependent_arr_type(
                elt_type, size_value(conv, dep_type->getSizeExpr()));
        }

        assert(false && "unknown clang array type");
        return nullptr;
    }

private:
    /// Converts array size expression to the code model value
    static value size_value(converter_impl & conv, const ::clang::Expr * size_expr) {
        assert(size_expr != nullptr && "array size expression should not be null");

        std::string str;
        {
            ::llvm::raw_string_ostream sout{str};
            size_expr->printPretty(sout, nullptr, conv.clang_ctx().getPrintingPolicy());
        }

        return value{str};
    }
};


static_assert(type_converter<array_type_converter>);


}
