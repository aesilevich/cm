// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file variable.hpp
/// Contains definition of the variable class.

#pragma once

#include "named_context_entity.hpp"
#include "qual_type.hpp"
#include "single_type_use.hpp"
#include <string>


namespace cm {


/// Represents variable in code model
class variable: public named_context_entity, virtual public single_type_use {
public:
    /// Constructs variable with specified name and type. Adds use to type
    variable(context * ctx, const std::string & nm, const qual_type & t):
    named_context_entity{ctx, nm}, context_entity{ctx}, single_type_use{t} {
    }

    /// Dumps variable to output stream
    void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const override {
        print_indent_spaces(str, indent);
        str << "var " << name() << ": ";
        type().print_desc(str);
        str << ";\n";
    }
};


}
