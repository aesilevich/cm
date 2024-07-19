// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file named_context_entity.hpp
/// Contains definition of the named_context_entity class.

#pragma once

#include "context_entity.hpp"
#include "named_entity.hpp"


namespace cm {


/// Context entity with name
class named_context_entity: virtual public context_entity, virtual public named_entity {
    friend class context;

public:
    /// Constructs named declaration with specified parent decl context and name
    explicit named_context_entity(context * ctx, const std::string & nm):
        context_entity{ctx}, name_{nm} {}

    /// Returns Entity name
    const std::string & name() const override { return name_; }

private:
    /// Sets entity name
    template <typename String>
    requires (std::assignable_from<std::string &, String>)
    void set_name_impl(String && str) {
        name_ = std::forward<String>(str);
    }

    std::string name_;          ///< Entity name
};


}
