// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file typedef_type.hpp
/// Contains definition of the typedef_type class.

#pragma once

#include "named_type.hpp"
#include "qual_type.hpp"


namespace cm {


/// Represents typedef type in code model
class typedef_type: public named_type {
public:
    /// Constructs typedef type with specified context, name and base qual type
    typedef_type(context * ctx, const std::string & nm, const qual_type & b):
        named_type(ctx, nm), context_type(ctx), context_entity(ctx), base_{b} {
        base_.type()->add_use(this);
    }

    /// Destructor, removes use of base type
    ~typedef_type() {
        base_.type()->remove_use(this);
    }

    /// Returns base type
    const qual_type & base() { return base_; }

    /// Returns const base type
    const_qual_type base() const { return base_; }

    /// Sets base type
    void set_base(const qual_type & b) {
        if (base_.type() == b.type()) {
            base_ = b;
            return;
        }

        base_->remove_use(this);
        base_ = b;
        base_->add_use(this);
    }

    /// Dumps typedef type to output stream
    void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const override;

private:
    qual_type base_;            ///< Base qual type
};


}
