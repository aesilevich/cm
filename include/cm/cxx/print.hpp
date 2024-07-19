// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file print.hpp
/// Contains declaration of code model printing functions for C++ language.

#pragma once

#include "../entity.hpp"
#include "../qual_type.hpp"
#include "../template_instantiation.hpp"


namespace cm::cxx {


/// Prints qualified type to output stream
void print_type(std::ostream & str, const const_qual_type & qt);

/// Prints entity to output stream
void print_entity(std::ostream & str, const entity * ent, bool full_name = true);

/// Prints template instantiation name with parameters
void print_template_instantiation_name(std::ostream & str, const template_instantiation * inst);

/// Prints function name without parameters
void print_function_name(std::ostream & str, const function * func);


}
