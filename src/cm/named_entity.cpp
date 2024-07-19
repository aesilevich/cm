// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file named_entity.cpp
/// Contains implementation of the named_entity class.

#include "pch.hpp"
#include "cm/named_entity.hpp"
#include <array>


namespace cm {


bool named_entity::is_builtin() const {
    static constexpr std::array builtin_names{
        // clang builtins
        "__int128_t",
        "__uint128_t",
        "__NSConstantString_tag",
        "__NSConstantString",
        "__builtin_ms_va_list",
        "__va_list_tag",
        "__builtin_va_list",

        // ARM vector builtins
        "__SVInt8_t",
        "__SVInt16_t",
        "__SVInt32_t",
        "__SVInt64_t",
        "__SVUint8_t",
        "__SVUint16_t",
        "__SVUint32_t",
        "__SVUint64_t",
        "__SVFloat16_t",
        "__SVFloat32_t",
        "__SVFloat64_t",
        "__SVBFloat16_t",
        "__SVBfloat16_t",
        "__clang_svint8x2_t",
        "__clang_svint16x2_t",
        "__clang_svint32x2_t",
        "__clang_svint64x2_t",
        "__clang_svuint8x2_t",
        "__clang_svuint16x2_t",
        "__clang_svuint32x2_t",
        "__clang_svuint64x2_t",
        "__clang_svfloat16x2_t",
        "__clang_svfloat32x2_t",
        "__clang_svfloat64x2_t",
        "__clang_svbfloat16x2_t",
        "__clang_svint8x3_t",
        "__clang_svint16x3_t",
        "__clang_svint32x3_t",
        "__clang_svint64x3_t",
        "__clang_svuint8x3_t",
        "__clang_svuint16x3_t",
        "__clang_svuint32x3_t",
        "__clang_svuint64x3_t",
        "__clang_svfloat16x3_t",
        "__clang_svfloat32x3_t",
        "__clang_svfloat64x3_t",
        "__clang_svbfloat16x3_t",
        "__clang_svint8x4_t",
        "__clang_svint16x4_t",
        "__clang_svint32x4_t",
        "__clang_svint64x4_t",
        "__clang_svuint8x4_t",
        "__clang_svuint16x4_t",
        "__clang_svuint32x4_t",
        "__clang_svuint64x4_t",
        "__clang_svfloat16x4_t",
        "__clang_svfloat32x4_t",
        "__clang_svfloat64x4_t",
        "__clang_svbfloat16x4_t",
        "__SVBool_t",
        "__clang_svboolx2_t",
        "__clang_svboolx4_t",
        "__SVCount_t",
    };

    for (auto && nm : builtin_names) {
        if (name() == nm) {
            return true;
        }
    }

    return false;
}


}
