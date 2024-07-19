// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file named_type.hpp
/// Contains definition of the named_type class.

#pragma once

#include "context_type.hpp"
#include "named_context_entity.hpp"
#include "type.hpp"
#include <string>


namespace cm {


class context;


/// Represents a type that has user defined name
class named_type: virtual public context_type,
                  public named_context_entity {
public:
    /// Constructor, makes named type with specified decl context and name
    named_type(context * ctx, const std::string & nm):
    context_type{ctx}, context_entity{ctx}, named_context_entity{ctx, nm} {
    }
};


}
