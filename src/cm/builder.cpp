// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file builder.cpp
/// Contains implementation of the builder class.

#include "pch.hpp"
#include <cm/builder.hpp>
#include <ranges>


namespace  cm {


void record_builder_base::base_impl(type_t * t) {
    if (!is_new_) {
        return;
    }

    rec_->add_base(t);
}

void record_builder_base::tdef_impl(const std::string & name,
                                    const qual_type & type,
                                    const std::string & id) {
    if (!is_new_) {
        // we are building anonymous record inside another record that already exists,
        // doing nothing
        return;
    }

    type_t * td = nullptr;
    if (is_new_) {
        td = rec_->create_typedef(name, type, cm::access_level::public_);
    } else {
        td = dynamic_cast<type_t*>(rec_->find_named_entity(name));
        assert(td && "can't find typedef in existing record");
    }

    if (!id.empty()) {
        bldr_.set_type(id, td);
    }
}


void record_builder_base::ivar_impl(const std::string & name,
                                    const qual_type & type,
                                    access_level acc,
                                    unsigned int bit_size,
                                    const source_location & loc) {
    if (!is_new_) {
        return;
    }

    auto ivar = rec_->create_field(name, type, acc, bit_size);
    ivar->set_loc(loc);
}


void record_builder_base::ifunc_impl(const std::string & name,
                                     function_type * type,
                                     access_level acc) {
    if (!is_new_) {
        return;
    }

    auto func = rec_->create_method(name, acc);
    func->set_ret_type(type->ret_type());
    for (auto && par : type->params()) {
        func->add_param(par);
    }
}


void record_builder_base::func_impl(const std::string & name,
                                    function_type * type) {
    if (!is_new_) {
        return;
    }

    auto func = rec_->create_function(name);
    func->set_ret_type(type->ret_type());
    for (auto && par : type->params()) {
        func->add_param(par);
    }
}


std::tuple<record_type *, bool>
record_builder_base::get_or_create_rec(const std::string & nm,
                                       record_kind knd,
                                       const std::string & id) {
    if (rec_ == nullptr) {
        // we are building nested anonymous record for existing record.
        // Nothing to do here
        return {nullptr, false};
    }

    if (is_new_) {
        // we should build new nested records
        record_type * nest_rec = nullptr;

        if (!nm.empty()) {
            // nested record with same name should not exist at this point
            nest_rec = rec_->create_named_record(nm, knd, cm::access_level::public_);
            bldr().add_new_rec_type(nest_rec);
        } else {
            nest_rec = rec_->create_record(knd, cm::access_level::public_);
            bldr().add_new_rec_type(nest_rec);
        }

        if (!id.empty()) {
            bldr().set_type(id, nest_rec);
        }

        return {nest_rec, true};
    } else {
        // don't build anything. Just adding record types to ID map

        if (nm.empty()) {
            // we can't lookup nested anonymous records. Returning nullptr in this case
            return {nullptr, false};
        }

        auto rec = rec_->find_named_record(nm);
        assert(rec && "can't find nested record with specified ID for existing record");

        if (!id.empty()) {
            bldr().set_type(id, rec);
        }

        return {rec, false};
    }
}


std::tuple<record_type*, bool>
namespace_builder_base::get_or_create_rec(const std::string & nm,
                                          record_kind knd,
                                          const std::string & id) {
    record_type * rec = nullptr;
    bool is_new = false;

    if (!nm.empty()) {
        rec = ns_->find_named_record(nm);
        if (!rec) {
            rec = ns_->create_named_record(nm, knd);
            bldr_.add_new_rec_type(rec);
            is_new = true;
        }
    } else {
        rec = ns_->create_record(knd);
        bldr_.add_new_rec_type(rec);
        is_new = true;
    }

    if (!id.empty()) {
        bldr_.set_type(id, rec);
    }

    return {rec, is_new};
}


builder::~builder() {
    assert(ref_types_.empty() && "reference types still exist");
    assert(new_rec_types_.empty() && "debug info for new records was not built");
}


type_t * builder::typeref(const std::string & id) {
    auto & ref_type = ref_types_[id];
    if (!ref_type) {
        ref_type = std::make_unique<type_ref>();
    }

    return ref_type.get();
}


void builder::set_type(const std::string & id, type_t * t) {
    real_ref_types_[id] = t;
}


builder_result builder::build() {
    // replacing all reference types with real types
    for (auto & ref : ref_types_) {
        auto it = real_ref_types_.find(ref.first);

        assert(it != real_ref_types_.end() && "can't find type for ref name");
        assert(it->second != ref.second.get() && "type was not defined");

        cm_.replace_type(ref.second.get(), it->second);
    }

    // removing all unused composite types derived from ref types
    bool removed = true;
    while (removed) {
        removed = false;

        const auto & ctypes = cm_.composite_types();
        for (auto it = std::begin(ctypes), end = std::end(ctypes); it != end;) {
            type_t * type = *it;
            ++it;

            if (type->uses().empty() && is_ref_derived(type)) {
                cm_.remove_type(type);
                removed = true;
            }
        }
    }

    // checking that all ref types don't have uses
    for (auto & ref : ref_types_) {
        assert(ref.second.get()->uses().empty() && "ref type still has use");
        (void)ref;
    }

    // removing all ref types
    ref_types_.clear();

    // making layout for all new records
    if (dbg_) {
        for (auto && rec : new_rec_types_) {
            dbg_->make_def_rec_layout(rec);
        }
    }

    new_rec_types_.clear();

    // building result
    builder_result res;
    for (auto && pair : real_ref_types_) {
        res.add_type(pair.first, pair.second);
    }

    return res;
}


bool builder::is_ref(type_t * type) const {
    auto rt = type->cast<type_ref>();
    if (!rt) {
        return false;
    }

    auto it = std::ranges::find_if(ref_types_, [rt](auto && pair) {
        return pair.second.get() == rt;
    });
    return it != std::ranges::end(ref_types_);
}


bool builder::is_ref_derived(type_t * type) const {
    if (is_ref(type)) {
        return true;
    }

    if (auto ptype = type->cast<ptr_or_ref_type>()) {
        return is_ref_derived(ptype->base());
    } else if (auto ftype = type->cast<function_type>()) {
        if (is_ref_derived(ftype->ret_type())) {
            return true;
        }

        auto check_fn = [this](auto && par) { return this->is_ref_derived(par); };
        return std::ranges::find_if(ftype->params(), check_fn) != std::ranges::end(ftype->params());

    } else if (auto atype = type->cast<array_type>()) {
        return is_ref_derived(atype->base());
    } else if (auto mptype = type->cast<mem_ptr_type>()) {
        return is_ref_derived(mptype->obj_type()) ||
               is_ref_derived(mptype->mem_type());
    }

    return false;
}


}
