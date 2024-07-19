// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file record_kind.hp
/// Contains definition of the record_kind enum

#pragma once


namespace cm {


/// Record kind type
enum class record_kind {
    class_,
    struct_,
    union_
};


/// Returns record kind name
inline std::string record_kind_name(record_kind knd) {
    switch (knd) {
    case record_kind::class_:
        return "class";
    case record_kind::struct_:
        return "struct";
    case record_kind::union_:
        return "union";
    default:
        assert(false && "unknown record kind");
        return {"unknown_kind"};
    }
}


}
