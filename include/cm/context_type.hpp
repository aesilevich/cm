// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file context_type.hpp
/// Contains definition of the context_type class.

#pragma once

#include "context_entity.hpp"
#include "type.hpp"


namespace cm {


/// Represents type that is located in some context
class context_type: virtual public type_t, virtual public context_entity {
public:
    /// Constructs decl type with specified parent context
    context_type(context * ctx): context_entity(ctx) {}
};


}
