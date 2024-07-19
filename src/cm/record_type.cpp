// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file record_type.cpp
/// Contains implementation of the record_type class.

#include "pch.hpp"
#include "cm/record_type.hpp"


namespace cm {


field * record::create_field(const std::string & name,
                                                 const qual_type & type,
                                                 member_access_spec acc,
                                                 unsigned int bit_size) {

    assert(find_var(name) == nullptr &&
            "static variable with specified name already exists");

    auto ivar = create_named_entity_impl<field>(this, name, type, bit_size);
    add_access_spec(ivar, acc);
    return ivar;
}


named_method * record::create_method(const std::string & name, member_access_spec spec) {
    // TODO: check existing overload?
    auto func = create_named_entity_impl<named_method>(this, name);
    add_access_spec(func, spec);
    return func;
}


void field::dump(std::ostream & str,
                 const dump_options & opts,
                 unsigned int indent) const {
    print_indent_spaces(str, indent);
    str << "field " << name() << ": ";
    type().print_desc(str);
    context_entity::dump_loc(str, opts);
    str << ";\n";
}


void method::dump(std::ostream & str, const dump_options & opts, unsigned int indent) const {
    print_indent_spaces(str, indent);
    str << "method ";
    dump_parameters_and_ret_type(str);
    context_entity::dump_loc(str, opts);
    str << ";\n";
}


void named_method::dump(std::ostream & str, const dump_options & opts, unsigned int indent) const {
    print_indent_spaces(str, indent);
    str << "method " << name();
    dump_parameters_and_ret_type(str);
    context_entity::dump_loc(str, opts);
    str << ";\n";
}


void record::dump_header(std::ostream & str, const dump_options & opts) const {
    str << record_kind_name(kind()) << ' ';
    print_desc(str);
    context_entity::dump_loc(str, opts);
    str << " {\n";
}


void record::dump_header_and_contents(std::ostream & str,
                                      const dump_options & opts,
                                      unsigned int indent) const {
    // dumping header
    dump_header(str, opts);

    // dumping declarations
    context::dump_entities(str, opts, indent + 1);

    // dumping footer
    print_indent_spaces(str, indent);
    str << "};\n";
}


void record_type::dump(std::ostream & str, const dump_options & opts, unsigned int indent) const {
    // printing header
    print_indent_spaces(str, indent);
    dump_header_and_contents(str, opts, indent);
}


}
