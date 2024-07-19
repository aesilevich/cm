// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file find_field.cpp
/// Contains implementations of functions for searching fields in record types.

#include "pch.hpp"
#include "cm/find_field.hpp"
#include "cm/record_type_debug_info.hpp"
#include <ranges>


namespace cm {


/// Find field implementation. Finds field with specified name in record type
/// taking into account base classes if check_bases flag is true.
std::tuple<field*, uint64_t> find_field(const debug_info * dbg,
                                                    record_type * rec,
                                                    const std::string & nm,
                                                    bool check_bases) {

    const record_type_debug_info * dinfo = nullptr;
    if (dbg) {
        dinfo = dbg->type_as<record_type_debug_info>(rec);
        if (!dinfo) {
            return {};
        }
    }

    // looking for directs fields of record type
    size_t idx = 0;
    for (auto && var : rec->fields()) {
        if (var->name() == nm) {
            // found field with required name
            if (dinfo) {
                assert(idx < dinfo->vars_offsets().size() && "invalid field index");
                return std::make_tuple(var, dinfo->vars_offsets()[idx]);
            } else {
                return std::make_tuple(var, 0);
            }
        } else if (var->name().empty()) {
            // found anonymous member, looking for field in it

            auto fld_type = var->type();
            auto rec_type = fld_type.cast<record_type>();
            assert(rec_type && "anonymous field is not a record");

            field * var = nullptr;
            uint64_t offs = 0;
            std::tie(var, offs) = find_field(dbg, rec_type.type(), nm, true);

            if (var) {
                if (dinfo) {
                    assert(idx < dinfo->vars_offsets().size() && "invalid field index");
                    return {var, dinfo->vars_offsets()[idx] + offs};
                } else {
                    return {var, 0};
                }
            }
        }

        ++idx;
    }


    // looking for field in base classes
    if (check_bases) {
        size_t idx = 0;
        for (auto && base : rec->bases()) {
            auto base_rec = base->untypedef()->cast<record_type>();
            assert(base_rec && "base is not a record");

            field * var = nullptr;
            uint64_t offs = 0;
            std::tie(var, offs) = find_field(dbg, base_rec, nm, true);

            if (!var) {
                ++idx;
                continue;
            }

            if (dinfo) {
                assert(idx < dinfo->bases_offsets().size() && "invalid index");
                return {var, dinfo->bases_offsets()[idx] + offs};
            } else {
                return {var, 0};
            }

            ++idx;
        }
    }

    return {nullptr, 0};
}


}
