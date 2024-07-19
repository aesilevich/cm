// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file print.cpp
/// Contains implementation of code model printing for C++ language.

#include "cm/cxx/print.hpp"
#include "cm/array_type.hpp"
#include "cm/builtin_type.hpp"
#include "cm/mem_ptr_type.hpp"
#include "cm/named_type.hpp"
#include "cm/ptr_or_ref_type.hpp"
#include "cm/vector_type.hpp"


namespace cm::cxx {


static void print_qual_type_decl_result(std::ostream & str, const const_qual_type & qt);
static void print_qual_type_decl_prefix(std::ostream & str, const const_qual_type & qt, bool end_sp);
static void print_qual_type_decl_suffix(std::ostream & str, const const_qual_type & qt, bool parent_op_suffix);
static void print_context_entity_name(std::ostream & str, const context_entity * ent, bool full_name);


/// Prints template instantiation name with parameters to output stream
void print_template_instantiation_name(std::ostream & str, const template_instantiation * inst) {
    str << inst->templ()->name();

    str << '<';
    bool first = true;

    // Is last parameter ends with>?
    bool last_rangle = false;

    for (auto && arg : inst->args()) {
        if (!first) {
            str << ", ";
        } else {
            first = false;
        }

        if (auto type_arg = dynamic_cast<const type_template_argument*>(arg)) {
            print_type(str, type_arg->type());

            if (dynamic_cast<const template_instantiation*>(type_arg->type().type())) {
                last_rangle = true;
            } else {
                last_rangle = false;
            }
        } else if (auto val_arg = dynamic_cast<const value_template_argument*>(arg)) {
            str << val_arg->val().str();
        } else {
            assert(false && "unknown template argument type");
        }
    }

    if (last_rangle) {
        str << ' ';
    }

    str << '>';
}


/// Prints fully qualified name of context entity
static void print_context_entity_name(std::ostream & str, const context_entity * ent, bool full_name) {
    // printing context name
    if (full_name) {
        if (!ent->ctx()->is_root()) {
            print_context_entity_name(str, ent->ctx(), true);
            str << "::";
        }
    }

    // printing named context entity
    if (auto n_ent = dynamic_cast<const named_entity*>(ent)) {
        str << n_ent->name();
        return;
    }

    // printing template instantiation entity
    if (auto t_inst = dynamic_cast<const template_instantiation*>(ent)) {
        print_template_instantiation_name(str, t_inst);
        return;
    }

    // printing anonymous record entity
    if (auto rec = dynamic_cast<const record_type*>(ent)) {
        str << "(anonymous record)";
        return;
    }

    assert(false && "don't know how to print context entity");
}


/// Prints type declaration result
static void print_type_decl_result(std::ostream & str, const type_t * t) {
    if (auto a_t = dynamic_cast<const array_type*>(t)) {
        print_type_decl_result(str, a_t->base());
    } else if (auto v_t = dynamic_cast<const vector_type*>(t)) {
        print_type_decl_result(str, v_t->base());
        str << " __attribute__((vector_size(" << v_t->size() << ")))";
    } else if (auto b_t = dynamic_cast<const builtin_type*>(t)) {
        str << b_t->name();
    } else if (auto f_t = dynamic_cast<const function_type*>(t)) {
        print_qual_type_decl_result(str, f_t->ret_type());
    } else if (auto m_t = dynamic_cast<const mem_ptr_type*>(t)) {
        print_qual_type_decl_result(str, m_t->mem_type());
    } else if (auto p_t = dynamic_cast<const ptr_or_ref_type*>(t)) {
        print_qual_type_decl_result(str, p_t->base());
    } else if (auto t_t_p = dynamic_cast<const type_template_parameter*>(t)) {
        str << t_t_p->name();
    } else if (const context_type * c_t = dynamic_cast<const context_type*>(t)) {
        print_context_entity_name(str, c_t, true);
    } else {
        assert(false && "uknown type for printing decl result");
    }
}


/// Prints type declaration prefix
static void print_type_decl_prefix(std::ostream & str, const type_t * t, bool end_sp) {
    if (dynamic_cast<const builtin_type*>(t)) {
        // doing nothing for builtin type name execpt of writing space if asked
        if (end_sp) {
            str << ' ';
        }
    } else if (auto a_t = dynamic_cast<const array_or_vector_type*>(t)) {
        print_type_decl_prefix(str, a_t->base(), end_sp);
    } else if (auto f_t = dynamic_cast<const function_type*>(t)) {
        print_qual_type_decl_prefix(str, f_t->ret_type(), end_sp);
    } else if (auto m_t = dynamic_cast<const mem_ptr_type*>(t)) {
        print_qual_type_decl_prefix(str, m_t->mem_type(), false);

        // writing ( parens if child is suffix op
        if (m_t->mem_type().cast<array_type>() || m_t->mem_type().cast<function_type>()) {
            str << " (";
        } else {
            str << ' ';
        }

        print_type(str, m_t->obj_type());
        str << "::*";
    } else if (auto p_t = dynamic_cast<const ptr_or_ref_type*>(t)) {
        print_qual_type_decl_prefix(str, p_t->base(), false);

        // writing ( parens if child is suffix op
        if (p_t->base().cast<array_type>() || p_t->base().cast<function_type>()) {
            str << " (";
        }
        
        str << (p_t->is_ref() ? '&' : '*');
    } else if(dynamic_cast<const record_type*>(t)) {
        // nothing to do here
    } else if (dynamic_cast<const typedef_type*>(t)) {
        // nothing to do here
    } else if (dynamic_cast<const enum_type*>(t)) {
        // nothing to do here
    } else {
        assert(false && "don't know how to print type decl prefix");
    }
}


/// Prints type declartion suffix
static void print_type_decl_suffix(std::ostream & str, const type_t * t, bool p_s) {
    if (dynamic_cast<const builtin_type*>(t)) {
        // nothing to do here
    } else if (auto a_t = dynamic_cast<const array_type*>(t)) {
        // printing space before array size if parent op did not write suffix
        if (!p_s)
            str << ' ';


        // printing array size

        str << '[';

        if (a_t->size() != SIZE_MAX) {
            str << a_t->size();
        }

        str << ']';


        // printing element suffix
        print_type_decl_suffix(str, a_t->base(), true);
    } else if (auto v_t = dynamic_cast<const vector_type*>(t)) {
        // printing element suffix
        print_type_decl_suffix(str, v_t->base(), true);
    } else if (auto f_t = dynamic_cast<const function_type*>(t)) {
        str << '(';
        bool first = true;
        for (const auto & p : f_t->params()) {
            if (!first) {
                str << ", ";
            } else {
                first = false;
            }

            print_type(str, p);
        }
        str << ')';

        print_qual_type_decl_suffix(str, f_t->ret_type(), true);
    } else if (auto p_t = dynamic_cast<const ptr_or_ref_type*>(t)) {
        // writing ) parens if child is suffix op
        bool suf = false;
        if (p_t->base().cast<array_type>() || p_t->base().cast<function_type>()) {
            str << ')';
            suf = true;
        }

        print_qual_type_decl_suffix(str, p_t->base(), suf);
    } else if (auto m_t = dynamic_cast<const mem_ptr_type*>(t)) {
        // writing ) parens if child is suffix op
        bool suf = m_t->mem_type().cast<array_type>() || m_t->mem_type().cast<function_type>();
        if (suf) {
            str << ')';
        }

        print_qual_type_decl_suffix(str, m_t->mem_type(), suf);
    } else if (dynamic_cast<const record_type*>(t)) {
        // nothing to do here
    } else if (dynamic_cast<const typedef_type*>(t)) {
        // nothing to do here
    } else if (dynamic_cast<const enum_type*>(t)) {
        // nothing to do here
    } else {
        assert(false && "don't know how to print type suffix");
    }
}


/// Prints qualified type qualifiers
static void print_qualifiers(std::ostream & str,
                             const const_qual_type & qt,
                             bool first_space,
                             bool last_space) {
    bool first = true;

    if (qt.is_const()) {
        if (first_space) {
            str << ' ';
        }

        str << "const";
        first = false;
    }

    if (qt.is_volatile()) {
        if (!first || first_space) {
            str << ' ';
        }

        str << "volatile";
        first = false;
    }

    if (!first && last_space) {
        str << ' ';
    }
}


/// Prints qualified type declaration result
static void print_qual_type_decl_result(std::ostream & str, const const_qual_type & qt) {
    // write qualifiers before result for array of user defined types
    if (const array_type * at = qt.type()->cast<array_type>()) {
        if (auto nt = at->base()->cast<named_type>()) {
            print_qualifiers(str, qt, false, true);
            print_type_decl_result(str, qt.type());
            return;
        }
    }

    // writing qualifiers only for user defined or builtin types

    if (auto bt = qt.type()->cast<builtin_type>()) {
        // qualifiers should be after builtin type name
        print_type_decl_result(str, bt);
        print_qualifiers(str, qt, true, false);
    } else if (qt.type()->cast<named_type>()) {
        // qualifiers should be before user defined type names
        print_qualifiers(str, qt, false, true);
        print_type_decl_result(str, qt.type());
    } else {
        // printing only result
        print_type_decl_result(str, qt.type());
    }
}


/// Prints qualified type declaration prefix
static void print_qual_type_decl_prefix(std::ostream & str, const const_qual_type & qt, bool end_sp) {
    print_type_decl_prefix(str, qt.type(), end_sp);

    // write qualifiers after prefix for prefix ops types
    // TODO: pointer to member type support
    if (qt.type()->cast<ptr_or_ref_type>()) {
        print_qualifiers(str, qt, true, false);
    }


    // write qualifiers in prefix for array types
    // except of arrays of user defined types (we print cv for such types in result)

    auto at = qt.type()->cast<array_type>();
    if (!at) {
        return;
    }

    if (at->base()->cast<named_type>()) {
        // user defined type
        return;
    }

    print_qualifiers(str, qt, true, false);
}


/// Prints qualified type declaration suffix
static void print_qual_type_decl_suffix(std::ostream & str, const const_qual_type & qt, bool parent_op_suffix) {
    print_type_decl_suffix(str, qt.type(), parent_op_suffix);

    // writing qualifiers for functions
    if (qt.type()->cast<function_type>()) {
        print_qualifiers(str, qt, true, false);
    }
}


void print_type(std::ostream & str, const const_qual_type & qt) {
    print_qual_type_decl_result(str, qt);
    print_qual_type_decl_prefix(str, qt, false);
    print_qual_type_decl_suffix(str, qt, false);
}


/// Prints function to output stream
void print_function(std::ostream & str, const function * func, bool full_name) {
    // printing function result type and preifx
    print_qual_type_decl_result(str, func->ret_type());
    print_qual_type_decl_prefix(str, func->ret_type(), true);

    // writing function name
    print_context_entity_name(str, func, full_name);

    // writing function parameters
    str << '(';
    bool first = true;
    for (const auto & p : func->params()) {
        if (!first) {
            str << ", ";
        } else {
            first = false;
        }

        print_entity(str, p, false);
    }
    str << ')';
}


/// Prints variable to output stream
void print_variable(std::ostream & str, const variable * var, bool full_name) {
    // printing variable type result and preifx
    print_qual_type_decl_result(str, var->type());
    print_qual_type_decl_prefix(str, var->type(), true);

    // printing variable name
    print_context_entity_name(str, var, full_name);

    // printing variable type suffix
    print_qual_type_decl_suffix(str, var->type(), false);
}


/// Prints named function parameter to output stream
static void print_named_function_parameter(std::ostream & str,
                                           const named_function_parameter * par) {
    // printing variable type result and preifx
    print_qual_type_decl_result(str, par->type());
    print_qual_type_decl_prefix(str, par->type(), true);

    // printing parameter name
    str << par->name();

    // printing variable type suffix
    print_qual_type_decl_suffix(str, par->type(), false);
}



void print_entity(std::ostream & str, const entity * ent, bool full_name) {
    if (auto func = dynamic_cast<const function*>(ent)) {
        print_function(str, func, full_name);
    } else if (auto var = dynamic_cast<const variable*>(ent)) {
        print_variable(str, var, full_name);
    } else if (auto templ = dynamic_cast<const templated_entity*>(ent)) {
        print_context_entity_name(str, templ, full_name);
    } else if (auto n_par = dynamic_cast<const named_function_parameter*>(ent)) {
        print_named_function_parameter(str, n_par);
    } else if (auto par = dynamic_cast<const function_parameter*>(ent)) {
        // unnamed function parameter
        print_type(str, par->type());
    } else {
        assert(false && "don't know how to print entity");
    }
}


void print_function_name(std::ostream & str, const function * func) {
    print_context_entity_name(str, func, true);
}


}
