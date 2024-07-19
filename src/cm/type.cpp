// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file type.cpp
/// Contains implementation of the type_t class.

#include "pch.hpp"
#include "cm/type.hpp"
#include "cm/typedef_type.hpp"


namespace cm {


const type_t * type_t::untypedef() const {
    if (auto td = cast<typedef_type>()) {
        return td->base().type()->untypedef();
    }

    return this;
}


type_t * type_t::untypedef() {
    if (auto td = cast<typedef_type>()) {
        return td->base().type()->untypedef();
    }

    return this;
}


}
