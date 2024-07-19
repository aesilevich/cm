// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file find_field.hpp
/// Contains declarations of functions for searching fields in record types

#pragma once

#include "code_model.hpp"
#include "debug_info.hpp"
#include <tuple>


namespace cm {


/// Searches for field with specified name in record type
/// taking into account base classes if check_bases flag is true.
std::tuple<field*, uint64_t> find_field(const debug_info * dbg,
                                                    record_type * rec,
                                                    const std::string & nm,
                                                    bool check_bases);


}
