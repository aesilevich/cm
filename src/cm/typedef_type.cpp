// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file typedef_type.cpp
/// Contains implementation of the typedef_type class.

#include "pch.hpp"
#include "cm/typedef_type.hpp"
#include <sstream>


namespace cm {


void typedef_type::dump(std::ostream & str, const dump_options & opts, unsigned int indent) const {
    print_indent_spaces(str, indent);
    str << "typedef " << name() << " = ";
    base().print_desc(str);
    context_entity::dump_loc(str, opts);
    str << ";\n";
}


}
