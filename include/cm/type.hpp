// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file type.hpp
/// Contains definition of the type_t class.

#pragma once

#include "entity.hpp"
#include <ranges>
#include <cassert>
#include <memory>


namespace cm {

class code_model;
class pointer_type;
class lvalue_reference_type;
class type_t;


/// Represents type in code model
class type_t: virtual public entity {
public:
    /// Gets existing or creates new pointer to this type
    inline pointer_type * ptr(code_model & cm);

    /// Gets existing or creates new reference to this type
    inline lvalue_reference_type * ref(code_model & cm);

    /// Returns type removing all typedefs
    const type_t * untypedef() const;

    /// Returns type removing all typedefs
    type_t * untypedef();
};


}
