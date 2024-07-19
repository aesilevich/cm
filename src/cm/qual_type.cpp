// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file qual_type.cpp
/// Contains implementation of the qual_type class.

#include "pch.hpp"
#include "cm/qual_type.hpp"
#include "cm/array_type.hpp"
#include "cm/builtin_type.hpp"
#include "cm/function_type.hpp"
#include "cm/named_type.hpp"
#include "cm/ptr_or_ref_type.hpp"
#include <sstream>


namespace cm {


void qual_type_base::print_qual(std::ostream &str, bool first_space, bool last_space) const {
    bool first = true;

    if (is_const()) {
        if (first_space) {
            str << ' ';
        }

        str << "const";
        first = false;
    }

    if (is_volatile()) {
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


void qual_type_base::print_desc(std::ostream & str) const {
    bool first = true;

    // printing type description
    ctype()->print_desc(str);

    // printing qualifiers

    if (is_const()) {
        str << " const";
    }

    if (is_volatile()) {
        str << " volatile";
    }
}


}
