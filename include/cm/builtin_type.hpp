// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file builtin_type.hpp
/// Contains definition of the builtin_type class.

#pragma once

#include "type.hpp"
#include <memory>
#include <ostream>
#include <string>


namespace cm {


/// Builtin type
class builtin_type: public type_t {
public:
    /// Kind of builtin type
    enum class kind_t {
        void_,
        bool_,
        char_,
        short_,
        int_,
        long_,
        long_long_,
        int128_,
        signed_char_,
        unsigned_char_,
        unsigned_short_,
        unsigned_int_,
        unsigned_long_,
        unsigned_long_long_,
        uint128_,
        float_,
        double_,
        long_double_,

        wchar_t_,
        char8_t_,
        char16_t_,
        char32_t_,

        nullptr_t_,

        complex_char_,
        complex_short_,
        complex_int_,
        complex_long_,
        complex_long_long_,
        complex_unsigned_char_,
        complex_unsigned_short_,
        complex_unsigned_int_,
        complex_unsigned_long_,
        complex_unsigned_long_long_,
        complex_float_,
        complex_double_,
        complex_long_double_,

        // ARM SVE

        arm_sve_int8x1_,
        arm_sve_int8x2_,
        arm_sve_int8x3_,
        arm_sve_int8x4_,
        arm_sve_int16x1_,
        arm_sve_int16x2_,
        arm_sve_int16x3_,
        arm_sve_int16x4_,
        arm_sve_int32x1_,
        arm_sve_int32x2_,
        arm_sve_int32x3_,
        arm_sve_int32x4_,
        arm_sve_int64x1_,
        arm_sve_int64x2_,
        arm_sve_int64x3_,
        arm_sve_int64x4_,

        arm_sve_uint8x1_,
        arm_sve_uint8x2_,
        arm_sve_uint8x3_,
        arm_sve_uint8x4_,
        arm_sve_uint16x1_,
        arm_sve_uint16x2_,
        arm_sve_uint16x3_,
        arm_sve_uint16x4_,
        arm_sve_uint32x1_,
        arm_sve_uint32x2_,
        arm_sve_uint32x3_,
        arm_sve_uint32x4_,
        arm_sve_uint64x1_,
        arm_sve_uint64x2_,
        arm_sve_uint64x3_,
        arm_sve_uint64x4_,

        arm_sve_float16x1_,
        arm_sve_float16x2_,
        arm_sve_float16x3_,
        arm_sve_float16x4_,
        arm_sve_float32x1_,
        arm_sve_float32x2_,
        arm_sve_float32x3_,
        arm_sve_float32x4_,
        arm_sve_float64x1_,
        arm_sve_float64x2_,
        arm_sve_float64x3_,
        arm_sve_float64x4_,

        arm_sve_bfloat16x1_,
        arm_sve_bfloat16x2_,
        arm_sve_bfloat16x3_,
        arm_sve_bfloat16x4_,

        arm_sve_boolx1_,
        arm_sve_boolx2_,
        arm_sve_boolx4_,

        arm_sve_count_,

        num_types_
    };


    /// Constructor, makes builtin type with specified kind and name
    builtin_type(kind_t k, const std::string & nm):
        kind_{k}, name_{nm} {}

    /// Move constructor
    builtin_type(builtin_type && bt) = default;

    /// Copy constructor, makes copy of builtin type
    builtin_type(const builtin_type & bt):
        kind_{bt.kind_}, name_{bt.name_} {
    }

    /// Returns kind of builtin type
    kind_t kind() const {
        return kind_;
    }

    /// Returns name of builtin type
    const auto & name() const {
        return name_;
    }

    /// Prints type description to output stream
    void print_desc(std::ostream & str) const override {
        str << name_;
    }

private:
    kind_t kind_;        ///< Kind of builtin type
    std::string name_;   ///< Name of builtin type
};


}
