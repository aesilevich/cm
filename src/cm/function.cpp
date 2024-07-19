// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file function.cpp
/// Contains implementation of function related classes.

#include "pch.hpp"
#include "cm/function.hpp"
#include "cm/context.hpp"


namespace cm {


function_parameter::function_parameter(function * f, const qual_type & t):
single_type_use{t}, func_{f} {
}


function * function_parameter::func() {
    return func_;
}


const function * function_parameter::func() const {
    return func_;
}


named_function_parameter::named_function_parameter(function * f, const std::string & nm, const qual_type & t):
function_parameter{f, t}, single_type_use{t}, name_{nm} {
}


void function::add_param(const qual_type & t) {
    params_.push_back(std::make_unique<function_parameter>(this, t));
}


void function::add_param(const std::string & name, const qual_type & t) {
    params_.push_back(std::make_unique<named_function_parameter>(this, name, t));
}


void function::remove_param(function_parameter * par) {
    // removing parameter from list of function parameters
    auto it = std::ranges::find_if(params_, [par](auto && up) -> bool {
        return up.get() == par;
    });
    assert(it != params_.end() && "can't find function parameter");
    params_.erase(it);
}


void function::dump(std::ostream & str, const dump_options & opts, unsigned int indent) const {
    dump(str, indent, std::string_view{});
}


void function::dump_parameters(std::ostream & str) const {
    str << "(";

    bool first = true;
    for (auto && par : params()) {
        if (first) {
            first = false;
        } else {
            str << ", ";
        }

        par->dump(str, {true, false}, 0);
    }

    str << ")";
}


void function::dump_parameters_and_ret_type(std::ostream & str) const {
    dump_parameters(str);
    if (ret_type()) {
        str << " -> ";
        ret_type().print_desc(str);
    }
}


void function::dump(std::ostream & str, unsigned int indent, const std::string_view & nm) const {
    print_indent_spaces(str, indent);
    str << "func " << nm;
    dump_parameters_and_ret_type(str);
    str << ";\n";
}


void named_function::dump(std::ostream & str,
                          const dump_options & opts,
                          unsigned int indent) const {
    function::dump(str, indent, name());
}


}
