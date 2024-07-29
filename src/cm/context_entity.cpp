// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file context_entity.cpp
/// Contains implementation of the context_entity class.

#include "pch.hpp"
#include "cm/context_entity.hpp"
#include "cm/context.hpp"


namespace cm {


context_entity::context_entity(context * ctx):
context_entity{ctx, access_level::public_} {
    if (ctx != nullptr) {
        set_access_lev(ctx->default_access_level());
    }
}


void context_entity::dump_loc(std::ostream & str, const dump_options & opts) const {
    if (!opts.locations) {
        return;
    }

    if (!loc().is_valid()) {
        return;
    }

    str << " [" << loc().file()->path().filename()
        << ':' << loc().line()
        << ':' << loc().column()
        << ']';
}


}
