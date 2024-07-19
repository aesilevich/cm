// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file debug_info.cpp
/// Contains implementation of the debug_info class.

#include "pch.hpp"
#include "cm/code_model.hpp"
#include "cm/debug_info.hpp"
#include "cm/record_type_debug_info.hpp"


namespace cm {


debug_info::debug_info(code_model & cm):
cm_{cm} {
    // initializing sizes of builtin types
    add_type(cm_.bt_bool(), 1);
    add_type(cm_.bt_char(), 1);
    add_type(cm_.bt_short(), 2);
    add_type(cm_.bt_int(), 4);
    add_type(cm_.bt_long(), 8);
    add_type(cm_.bt_long_long(), 8);
    add_type(cm_.bt_unsigned_char(), 1);
    add_type(cm_.bt_unsigned_short(), 2);
    add_type(cm_.bt_unsigned_int(), 4);
    add_type(cm_.bt_unsigned_long(), 8);
    add_type(cm_.bt_unsigned_long_long(), 8);
    add_type(cm_.bt_float(), 4);
    add_type(cm_.bt_double(), 8);
    add_type(cm_.bt_long_double(), 10);

    add_type(cm_.bt_wchar_t(), 4);
    add_type(cm_.bt_char16_t(), 2);
    add_type(cm_.bt_char32_t(), 4);

    add_type(cm_.bt_complex_char(), 2);
    add_type(cm_.bt_complex_short(), 4);
    add_type(cm_.bt_complex_int(), 8);
    add_type(cm_.bt_complex_long(), 16);
    add_type(cm_.bt_complex_long_long(), 16);
    add_type(cm_.bt_complex_unsigned_char(), 2);
    add_type(cm_.bt_complex_unsigned_short(), 4);
    add_type(cm_.bt_complex_unsigned_int(), 8);
    add_type(cm_.bt_complex_unsigned_long(), 16);
    add_type(cm_.bt_complex_unsigned_long_long(), 16);
    add_type(cm_.bt_complex_float(), 8);
    add_type(cm_.bt_complex_double(), 16);
    add_type(cm_.bt_complex_long_double(), 20);
}


uint64_t debug_info::find_type_size(const const_qual_type & t) const {
    // default values for some types
    if (t.type()->cast<cm::ptr_or_ref_type>()) {
        return ptr_size();
    } else if(auto mptr_type = t.type()->cast<cm::mem_ptr_type>()) {
        if (mptr_type->mem_type().cast<cm::function_type>()) {
            // pointer to member function
            return ptr_size() * 2;
        } else {
            // pointer to member variables
            return ptr_size();
        }
    } else if (auto et = t.cast<enum_type>()) {
        return type_size(et->base());
    } else if (auto tt = t.cast<typedef_type>()) {
        return type_size(tt->base());
    } else if (auto at = t.cast<array_type>()) {
        return type_size(at->base()) * at->size();
    } else if (auto vt = t.cast<vector_type>()) {
        return type_size(vt->base()) * vt->size();
    }

    auto dbg = type(t.type());
    if (dbg) {
        return dbg->size();
    }

    return 0;
}


uint64_t debug_info::type_size(const const_qual_type & t) const {
    auto sz = find_type_size(t);
    if (sz == 0) {
        auto atype = t.cast<array_type>();
        assert(atype && atype->size() == 0 && "debug info not found");
    }

    return sz;
}


const record_type_debug_info * debug_info::make_def_rec_layout(record_type * rec,
                                                               bool recursive) {
    auto rec_dbg = std::make_unique<record_type_debug_info>();

    uint64_t curr_offs = 0;

    if (rec->kind() != record_kind::union_) {
        // laying out base classes
        for (auto && base : rec->base_recs()) {
            // making layout of base class if does not exist
            auto base_dbg = type(base);
            if (!base_dbg) {
                assert(recursive && "layout for base record does not exist");
                base_dbg = make_def_rec_layout(base, true);
            }

            rec_dbg->bases_offsets().push_back(curr_offs);
            curr_offs += base_dbg->size() * 8;
        }

        // laying out fields
        for (auto && var : rec->fields()) {
            // making layout for type of field if it is record type
            auto var_rtype = var->type().type()->untypedef()->cast<record_type>();
            if (var_rtype) {
                auto var_rtype_dbg = type(var_rtype);
                if (!var_rtype_dbg) {
                    assert(recursive && "layout for field does not exist");
                    make_def_rec_layout(var_rtype, true);
                }
            }

            if (var->bit_size() == 0) {
                if (curr_offs % 8 != 0) {
                    curr_offs += (8 - curr_offs % 8);
                }
                rec_dbg->vars_offsets().push_back(curr_offs);
                curr_offs += type_size(var->type()) * 8;
            } else {
                rec_dbg->vars_offsets().push_back(curr_offs);
                curr_offs += var->bit_size();
            }
        }

    } else {
        assert(std::ranges::empty(rec->bases()) && "union should not have base classes");

        for (auto && var : rec->fields()) {
            auto var_rtype = var->type().type()->untypedef()->cast<record_type>();
            if (var_rtype) {
                auto var_rtype_dbg = type(var_rtype);
                if (!var_rtype_dbg) {
                    assert(recursive && "layout for field does not exist");
                    make_def_rec_layout(var_rtype, true);
                }
            }

            rec_dbg->vars_offsets().push_back(0);
            curr_offs = std::max(curr_offs, type_size(var->type()) * 8);
        }
    }

    // calculating size of record
    auto sz = curr_offs / 8;
    if (curr_offs % 8 != 0) {
        ++sz;
    }

    if (sz == 0) {
        sz = 1;
    }

    rec_dbg->set_size(sz);

    auto res = rec_dbg.get();
    add_type(rec, std::move(rec_dbg));
    return res;
}


entity * debug_info::entity_at_addr(uint64_t addr) const {
    // looking for object with address greater than specified address
    auto it = obj_addrs_.upper_bound(addr);

    // getting object located before found object
    if (it == obj_addrs_.begin()) {
        return static_cast<function*>(nullptr);
    }

    --it;

    assert(it->first <= addr && "invalid result of upper_bound");

    // checking that specified address fits in function size
    auto dinfo = object(it->second);
    assert(dinfo && "debug info for function not found");
    if (addr >= it->first + dinfo->size()) {
        return static_cast<function*>(nullptr);
    }

    return it->second;
}


debug_symbol * debug_info::sym_at_addr(uint64_t addr) const {
    // looking for symbol with address greater than specified address
    auto it = symbols_.upper_bound(addr);

    // getting symbol located before found symbol
    if (it == symbols_.begin()) {
        return nullptr;
    }

    --it;

    assert(it->first <= addr && "invalid result of upper_bound");

    // checking that specified address fits in symbol size
    if (addr >= it->first + it->second->size()) {
        return nullptr;
    }

    return it->second.get();
}


}
