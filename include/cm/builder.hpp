// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file builder.hpp
/// Contains definition of the builder class.

#pragma once

#include "code_model.hpp"
#include "debug_info.hpp"


namespace cm {


class builder;
class record_builder_base;

template <typename Parent>
class record_builder;

template <typename Parent>
class namespace_builder;

template <typename Parent, typename ... Params>
class template_builder;


/// Context builder
class entity_context_builder {
public:
    /// Constructs context builder with specified reference to main builder
    /// and pointer to context being built
    entity_context_builder(builder & b, context * ctx):
        bldr_{b}, ctx_{ctx} {}

protected:
    /// Returns reference to main builder
    builder & bldr() const { return bldr_; }

private:
    builder & bldr_;            ///< Reference to main builder
    context * ctx_;      ///< Pointer to context being built
};


/// Base class for all record builders
class record_builder_base {
    template <typename Parent, typename ... Params>
    friend class template_builder;

public:
    /// Constructs record builder for specified record and id
    record_builder_base(builder & b, record * rec, bool is_new):
        bldr_{b}, rec_{rec}, is_new_{is_new} {}

    /// Default virtual destructor
    virtual ~record_builder_base() = default;

protected:
    /// Returns reference to main builder
    auto & bldr() { return bldr_; }

    /// Starts building of record of specified kind in context.
    /// Returns record builder
    template <typename ParentBuilder>
    record_builder<ParentBuilder> record_impl(ParentBuilder * parent,
                                              const std::string & nm,
                                              record_kind knd,
                                              const std::string & id) {
        record_type * rec;
        bool is_new;
        std::tie(rec, is_new) = get_or_create_rec(nm, knd, id);
        return record_builder<ParentBuilder>{bldr_, parent, rec, is_new};
    }

    /// Starts building of template in context. Returns template builder
    template <typename ParentBuilder, typename ... Params>
    template_builder<ParentBuilder, Params...> templ_impl(ParentBuilder * parent,
                                               const std::string & nm,
                                               Params && ... params) {
        return template_builder<ParentBuilder, Params...>(bldr_, parent, nm, false, std::forward<Params>(params)...);
    }

    /// Starts building of variadic template in context. Returns template builder
    template <typename ParentBuilder, typename ... Params>
    template_builder<ParentBuilder, Params...> vtempl_impl(ParentBuilder * parent,
                                                const std::string & nm,
                                                Params && ... params) {
        return template_builder<ParentBuilder, Params...>(bldr_, parent, nm, true, std::forward<Params>(params)...);
    }

    /// Add base implementation
    void base_impl(type_t * t);

    /// typedef build implementation
    void tdef_impl(const std::string & name, const qual_type & type, const std::string & id);

    /// Instance variable build implementation
    void ivar_impl(const std::string & name,
                   const qual_type & type,
                   member_access_spec acc = member_access_spec::public_,
                   unsigned int bit_size = 0,
                   const source_location & loc = {});

    /// Instance function build implementation
    /// TODO: replace function type with parameters
    void ifunc_impl(const std::string & name,
                    function_type * type,
                    member_access_spec acc = member_access_spec::public_);

    /// Static function build implementation
    /// TODO: replace function type with parameters
    void func_impl(const std::string & name, function_type * type);

private:
    /// Returns pointer to code model context for this builder
    record * ctx() { return rec_; }

    /// Gets or crates record with specified name, kind, and ID.
    /// Returns pair containing pointer to record type and new flag
    std::tuple<record_type*, bool>
    get_or_create_rec(const std::string & nm, record_kind knd, const std::string & id);


    builder & bldr_;            ///< Reference to main builder
    record * rec_;         ///< Pointer to record being built

    /// New flag. If false then nothing is being built in this context
    bool is_new_;
};


/// Base class for all namespace builders
class namespace_builder_base {
    template <typename Parent, typename ... Params>
    friend class template_builder;

public:
    /// Constructs namespace builder with specified reference to main builder
    /// and pointer to namespace being built
    namespace_builder_base(builder & b, namespace_ * ns):
        bldr_{b}, ns_{ns} {}

    /// Default virtual destructor
    virtual ~namespace_builder_base() = default;

protected:
    /// Returns reference to main builder
    auto & bldr() { return bldr_; }

    /// Starts building namespace and returns builder for it
    template <typename ParentBuilder>
    namespace_builder<ParentBuilder> ns_impl(ParentBuilder * parent,
                                             const std::string & name) {
        auto nest_ns = ns_->get_or_create_namespace(name);
        return namespace_builder<ParentBuilder>{bldr(), parent, nest_ns};
    }

    /// Starts building of record of specified kind in namespace.
    /// Returns record builder
    template <typename ParentBuilder>
    record_builder<ParentBuilder> record_impl(ParentBuilder * parent,
                                              const std::string & nm,
                                              record_kind knd,
                                              const std::string & id) {
        record_type * rec;
        bool is_new;
        std::tie(rec, is_new) = this->get_or_create_rec(nm, knd, id);
        return record_builder<ParentBuilder>{bldr_, parent, rec, is_new};
    }

    /// Starts building of template in context. Returns template builder
    template <typename ParentBuilder, typename ... Params>
    template_builder<ParentBuilder, Params...> templ_impl(ParentBuilder * parent,
                                               const std::string & nm,
                                               Params && ... params) {
        return template_builder<ParentBuilder, Params...>(bldr_, parent, nm, false, std::forward<Params>(params)...);
    }

    /// Starts building of variadic template in context. Returns template builder
    template <typename ParentBuilder, typename ... Params>
    template_builder<ParentBuilder, Params...> vtempl_impl(ParentBuilder * parent,
                                                const std::string & nm,
                                                Params && ... params) {
        return template_builder<ParentBuilder, Params...>(bldr_, parent, nm, true, std::forward<Params>(params)...);
    }

private:
    /// Returns pointer to code model context for this builder
    namespace_ * ctx() { return ns_; }

    /// Gets existing or creates new record in namespace. Returns pair of
    /// pointer to nested record and new flag
    std::tuple<record_type*, bool>
    get_or_create_rec(const std::string & nm, record_kind knd, const std::string & id);

    builder & bldr_;            ///< Reference to main builder
    namespace_ * ns_;           ///< Pointer to namespace being built
};


/// Template builder
template <typename Parent, typename ... TemplateParams>
class template_builder {
    /// Type of tuple element for storing template parameter
    template <typename T>
    using template_parameters_tuple_element = std::conditional_t <
        std::constructible_from<std::string, T>,
        std::string,
        std::tuple<std::string, cm::type_t*>
    >;

    /// Type of tuple for storing template parameters
    using template_parameters_tuple = std::tuple <
        template_parameters_tuple_element<TemplateParams>...
    >;

public:
    /// Constructs tempalte builder with specified reference to main builder,
    /// pointer to parent builder, parameters pack flag and template parameters
    template_builder(builder & b,
                     Parent * parent,
                     const std::string & nm,
                     bool is_par_pack,
                     TemplateParams && ... tparams):
    bldr_{b}, parent_{parent}, name_{nm}, is_par_pack_{is_par_pack}, params_{std::forward<TemplateParams>(tparams)...} {
        assert(parent_ != nullptr && "parent should not be null");
    }

    /// Finishes building template and returns reference to parent builder
    Parent & end() const { return *parent_; }

    /// Starts building template record instance with range of template parameters
    template <typename Params>
    record_builder<template_builder> record_r(const Params & params) {
        record_type * rec;
        bool is_new;
        std::tie(rec, is_new) = get_or_create_rec_inst_r({}, record_kind::class_, params);
        return record_builder<template_builder>{bldr_, this, rec, is_new};
    }

    /// Starts building template record with specified kind and range of template parameters
    template <typename Params>
    record_builder<template_builder> record_r(record_kind knd, const Params & params) {
        record_type * rec;
        bool is_new;
        std::tie(rec, is_new) = get_or_create_rec_inst_r({}, knd, params);
        return record_builder<template_builder>{bldr_, this, rec, is_new};
    }


    /// Starts building template record instance with specified ID and range of parameters
    template <typename Params>
    record_builder<template_builder> record_r(const std::string & id, const Params & params) {
        record_type * rec;
        bool is_new;
        std::tie(rec, is_new) = get_or_create_rec_inst_r(id, record_kind::class_, params);
        return record_builder<template_builder>{bldr_, this, rec, is_new};
    }

    /// Starts building template record instance with specified ID and range of template parameters
    template <typename Params>
    record_builder<template_builder> record_r(const char * id, const Params & params) {
        record_type * rec;
        bool is_new;
        std::tie(rec, is_new) = get_or_create_rec_inst_r(id, record_kind::class_, params);
        return record_builder<template_builder>{bldr_, this, rec, is_new};
    }

    /// Starts building template record instance with specified kind, id and range of template parameters
    template <typename Params>
    record_builder<template_builder> record_r(record_kind knd,
                                              const std::string & id,
                                              const Params & params) {
        record_type * rec;
        bool is_new;
        std::tie(rec, is_new) = get_or_create_rec_inst_r(id, knd, params);
        return record_builder<template_builder>{bldr_, this, rec, is_new};
    }

    /// Starts building template record instance with specified record kind, id and template parameters
    template <typename Params>
    record_builder<template_builder> record_r(record_kind knd,
                                              const char * id,
                                              const Params & params) {
        auto rec = get_or_create_rec_inst_r(id, knd, params);
        return record_builder<template_builder>{bldr_, this, rec};
    }

    /// Starts building template record instance for template being built
    template <typename ... Params>
    record_builder<template_builder> record(Params && ... params) {
        record_type * rec;
        bool is_new;
        std::tie(rec, is_new) = get_or_create_rec_inst({},
                                                       record_kind::class_,
                                                       std::forward<Params>(params)...);
        return record_builder<template_builder>{bldr_, this, rec, is_new};
    }

    /// Starts building template record with specified kind
    template <typename ... Params>
    record_builder<template_builder> record(record_kind knd, Params && ... params) {
        record_type * rec;
        bool is_new;
        std::tie(rec, is_new) = get_or_create_rec_inst({},
                                                       knd,
                                                       std::forward<Params>(params)...);
        return record_builder<template_builder>{bldr_, this, rec, is_new};
    }


    /// Starts building template record instance with specified ID
    /// for template being built
    template <typename ... Params>
    record_builder<template_builder> record(const std::string & id, Params && ... params) {
        record_type * rec;
        bool is_new;
        std::tie(rec, is_new) = get_or_create_rec_inst(id,
                                                       record_kind::class_,
                                                       std::forward<Params>(params)...);
        return record_builder<template_builder>{bldr_, this, rec, is_new};
    }

    /// Starts building template record instance with specified ID
    /// for template being built
    template <typename ... Params>
    record_builder<template_builder> record(const char * id, Params && ... params) {
        record_type * rec;
        bool is_new;
        std::tie(rec, is_new) = get_or_create_rec_inst(id,
                                                       record_kind::class_,
                                                       std::forward<Params>(params)...);
        return record_builder<template_builder>{bldr_, this, rec, is_new};
    }

    /// Starts building template record instance with specified record kind
    /// and type ID
    template <typename ... Params>
    record_builder<template_builder> record(record_kind knd,
                                            const std::string & id,
                                            Params && ... params) {
        record_type * rec;
        bool is_new;
        std::tie(rec, is_new) = get_or_create_rec_inst(id,
                                                       knd,
                                                       std::forward<Params>(params)...);
        return record_builder<template_builder>{bldr_, this, rec, is_new};
    }

    /// Starts building template record instance with specified record kind
    /// and type ID
    template <typename ... Params>
    record_builder<template_builder> record(record_kind knd,
                                            const char * id,
                                            Params && ... params) {
        auto rec = get_or_create_rec_inst(id, knd, std::forward<Params>(params)...);
        return record_builder<template_builder>{bldr_, this, rec};
    }

private:
    /// Returns template record created from template parameters
    template_record * get_or_create_template_record(record_kind kind) {
        auto get_or_create_fn = [this, kind](auto && ... params) -> auto {
            return parent_->ctx()->get_or_create_template_record(name_, kind, is_par_pack_, params...);
        };

        return std::apply(get_or_create_fn, params_);
    }

    /// Gets or creates record instance with specified id, kind, and range of parameters
    template <typename Params>
    std::tuple<record_type*, bool>
    get_or_create_rec_inst_r(const std::string & id, record_kind knd, const Params & params);

    /// Gets or creates record instance with specified id, kind, and list of parameters
    template <typename ... Params>
    std::tuple<record_type*, bool>
    get_or_create_rec_inst(const std::string & id, record_kind knd, Params && ... params);

    builder & bldr_;                        ///< Reference to main builder
    Parent * parent_;                       ///< Pointer to parent builder
    std::string name_;                      ///< Template name
    bool is_par_pack_;                      ///< Parameters pack flag
    template_parameters_tuple params_;      ///< Template parameters
};


/// Record builder
template <typename Parent>
class record_builder: public record_builder_base {
public:
    /// Constructs record builder of specified kind
    record_builder(builder & b, Parent * parent, record_type * rec, bool is_new):
    record_builder_base(b, rec, is_new), parent_{parent}, rec_{rec} {
        assert(parent_ != nullptr && "Parent context should not be null");
    }

    /// Ends building record. Returns reference to parent context builder
    Parent & end() {
        assert(parent_ != nullptr && "Parent context should not be null");
        return *parent_;
    }

    /// Adds base to record
    record_builder<Parent> & base(type_t * t) {
        base_impl(t);
        return *this;
    }

    /// Creates typedef in record. Typedef must not exist
    auto tdef(const std::string & name, const qual_type & type, const std::string & id = {}) {
        tdef_impl(name, type, id);
        return *this;
    }

    /// Creates field in record
    record_builder<Parent> ivar(const std::string & name,
                                const qual_type & type,
                                member_access_spec acc = member_access_spec::public_,
                                unsigned int bit_size = 0,
                                const source_location & loc = {}) {
        ivar_impl(name, type, acc, bit_size, loc);
        return *this;
    }

    /// Creates method in record
    record_builder<Parent> ifunc(const std::string & name,
                                 function_type * type,
                                 member_access_spec acc = member_access_spec::public_) {
        ifunc_impl(name, type, acc);
        return *this;
    }

    /// Creates static function in record
    record_builder<Parent> func(const std::string & name, function_type * type) {
        func_impl(name, type);
        return *this;
    }

    /// Starts named record of specified kind in namespace. Returns
    /// reference to record builder
    record_builder<record_builder>
    record(const std::string & nm,
           record_kind knd = record_kind::class_,
           const std::string & id = {}) {
        return this->record_impl(this, nm, knd, id);
    }

    /// Starts unnamed record of specoified type in context. Returns
    /// reference to record builder
    record_builder<record_builder> record(record_kind knd = record_kind::class_,
                                          const std::string & id = {}) {
        return this->record_impl(this, {}, knd, id);
    }

    /// Starts named record with specified id
    record_builder<record_builder> record(const std::string & nm, const std::string & id) {
        return this->record_impl(this, nm, record_kind::class_, id);
    }

    /// Starts building nested template in record
    template <typename ... Params>
    template_builder<record_builder, Params...> templ(const std::string & nm, Params && ... params) {
        return this->templ_impl(this, nm, std::forward<Params>(params)...);
    }

private:
    Parent * parent_;       ///< Pointer to parent ctx builder
    record_type * rec_;     ///< Pointer to record being built
};


/// Namespace builder
template <typename Parent>
class namespace_builder: public namespace_builder_base {
    using this_type = namespace_builder<Parent>;

public:
    /// Constructs namespace builder with specified pointer to
    /// namespace being built
    namespace_builder(builder & b, Parent * parent, namespace_ * ns):
    namespace_builder_base(b, ns), parent_{parent}, ns_{ns} {
        assert(ns_ != nullptr && "null namespace pointer");
    }

    /// Ends building namespace. Returns reference to parent namespace
    Parent & end() {
        assert(parent_ && "Can't call end for global namespace");
        return *parent_;
    }

    /// Starts building namespace and returns builder for it
    namespace_builder<this_type> ns(const std::string & name) {
        return this->ns_impl(this, name);
    }

    /// Starts named record of specified kind in namespace. Returns
    /// reference to record builder
    record_builder<this_type>
    record(const std::string & nm,
           record_kind knd = record_kind::class_,
           const std::string & id = {}) {
        return this->record_impl(this, nm, knd, id);
    }

    /// Starts unnamed record of specoified type in context. Returns
    /// reference to record builder
    record_builder<this_type> record(record_kind knd = record_kind::class_,
                                     const std::string & id = {}) {
        return this->record_impl(this, {}, knd, id);
    }

    /// Starts named record with specified id
    record_builder<this_type> record(const std::string & nm, const std::string & id) {
        return this->record_impl(this, nm, record_kind::class_, id);
    }

    /// Starts building nested template with specified parameters
    template <typename ... Params>
    template_builder<this_type, Params...> templ(const std::string & nm, Params && ... params) {
        return this->templ_impl(this, nm, std::forward<Params>(params)...);
    }

    /// Starts building nested variadic template with specified name of variadic parameter
    /// and template parameters
    template <typename ... Params>
    template_builder<this_type, Params...> vtempl(const std::string & nm, Params && ... params) {
        return this->vtempl_impl(this, nm, std::forward<Params>(params)...);
    }


private:
    Parent * parent_;       ///< Parent namespace or nullptr for global ns
    namespace_ * ns_;       ///< Pointer to namespace in code model to be built
};


/// Builder result. Gives access to types referenced by IDs that was used
/// when was building CM
class builder_result {
public:
    /// Constructs empty build result
    builder_result() = default;

    /// Returns type for specified ID. Type must exist
    type_t * type(const std::string & id) {
        auto it = types_.find(id);
        assert(it != types_.end() && "can't find type with specified id");
        return it->second;
    }

    /// Returns type casted to record_type
    record_type * rtype(const std::string & id) {
        auto res = type(id)->cast<record_type>();
        assert(res != nullptr && "type is not a record");
        return res;
    }

    /// Adds type to build result
    void add_type(const std::string & id, type_t * type) {
        types_.emplace(id, type);
    }

private:
    std::map<std::string, type_t*> types_;  ///< Resolved type references
};


/// Helper class for building code model
class builder: public namespace_builder_base {
public:
    /// Constructs builder for specified code model and optional debug info.
    builder(code_model & cm, debug_info * dbg = nullptr):
        namespace_builder_base(*this, &cm),
        cm_{cm}, dbg_{dbg} {}

    /// Deleted copy constructor
    builder(const builder & b) = delete;

    /// Deleted move constructor
    builder(builder && b) = delete;

    /// Destroys builder and checks that all reference type was replaced
    /// will real types, i. e. build was called
    ~builder();


    /// Starts building namespace and returns builder for it
    namespace_builder<builder> ns(const std::string & name) {
        return this->ns_impl(this, name);
    }

    /// Starts named record of specified kind in namespace. Returns
    /// reference to record builder
    record_builder<builder> record(const std::string & nm,
                                   record_kind knd = record_kind::class_,
                                   const std::string & id = {}) {
        return this->record_impl(this, nm, knd, id);
    }

    /// Starts unnamed record of specoified type in context. Returns
    /// reference to record builder
    record_builder<builder> record(record_kind knd = record_kind::class_,
                                   const std::string & id = {}) {
        return this->record_impl(this, {}, knd, id);
    }

    /// Starts named record with specified id
    record_builder<builder> record(const std::string & nm, const std::string & id) {
        return this->record_impl(this, nm, record_kind::class_, id);
    }

    /// Starts building nested template in namespace
    template <typename ... Params>
    template_builder<builder, Params...> templ(const std::string & nm, Params && ... params) {
        return this->templ_impl(this, nm, std::forward<Params>(params)...);
    }


    /// Returns type ref with specified ID
    type_t * typeref(const std::string & id);

    /// Sets type of specified ID
    void set_type(const std::string & id, type_t * type);

    /// Adds record type to list of new records that need layout
    void add_new_rec_type(record_type * rec) {
        new_rec_types_.push_back(rec);
    }

    /// Returns reference to code model
    auto & cm() const { return cm_; }

    /// Returns reference to debug info. Debug info pointer passed in ctor
    /// must not be null
    auto & dbg() {
        assert(dbg_ != nullptr && "debug info is null");
        return *dbg_;
    }

    /// Completes build of code model and replaces all type references
    /// with real types. Returns build result
    builder_result build();

    /// Gets or creates function type with specified return type and range of parameters
    template <typename Params>
    function_type * ftype_r(const qual_type & ret_type, const Params & params) {
        return cm_.get_or_create_func_type_r(ret_type, params);
    }

    /// Gets or creates function type with specified return type and parameters
    template <typename ... Params>
    function_type * ftype(const qual_type & ret_type, Params && ... params) {
        return cm_.get_or_create_func_type(ret_type, std::forward<Params>(params)...);
    }

    /// Gets or creates array type with specified elemtn type and size
    array_type * atype(type_t * elt_type, size_t sz = 0) {
        return cm_.get_or_create_arr_type(elt_type, sz);
    }

    /// Gets or creates pointer type with specifeid base
    pointer_type * ptype(const qual_type & base) {
        return cm_.get_or_create_ptr_type(base);
    }

    /// Gets or creates reference type with specifeid base
    lvalue_reference_type * rtype(const qual_type & base) {
        return cm_.get_or_create_lvalue_ref_type(base);
    }

    /// Gets or creates member pointer type
    mem_ptr_type * mtype(record_type * obj_type, const qual_type & type) {
        return cm_.get_or_create_mem_ptr_type(obj_type, type);
    }

    /// Gets existing or creates new source file object in code model
    const source_file * source(const std::string & nm) {
        return cm_.source(nm);
    }


    // Builtin type accessors

#define CM_BUILDER_BT_ACCESSOR(name) \
    builtin_type * bt_##name() const { return cm_.bt_##name(); }

    CM_BUILDER_BT_ACCESSOR(void)
    CM_BUILDER_BT_ACCESSOR(bool)
    CM_BUILDER_BT_ACCESSOR(char)
    CM_BUILDER_BT_ACCESSOR(short)
    CM_BUILDER_BT_ACCESSOR(int)
    CM_BUILDER_BT_ACCESSOR(long)
    CM_BUILDER_BT_ACCESSOR(long_long)
    CM_BUILDER_BT_ACCESSOR(unsigned_char)
    CM_BUILDER_BT_ACCESSOR(unsigned_short)
    CM_BUILDER_BT_ACCESSOR(unsigned_int)
    CM_BUILDER_BT_ACCESSOR(unsigned_long)
    CM_BUILDER_BT_ACCESSOR(unsigned_long_long)
    CM_BUILDER_BT_ACCESSOR(float)
    CM_BUILDER_BT_ACCESSOR(double)
    CM_BUILDER_BT_ACCESSOR(long_double)

    CM_BUILDER_BT_ACCESSOR(wchar_t)
    CM_BUILDER_BT_ACCESSOR(char16_t)
    CM_BUILDER_BT_ACCESSOR(char32_t)

    CM_BUILDER_BT_ACCESSOR(complex_char)
    CM_BUILDER_BT_ACCESSOR(complex_short)
    CM_BUILDER_BT_ACCESSOR(complex_int)
    CM_BUILDER_BT_ACCESSOR(complex_long)
    CM_BUILDER_BT_ACCESSOR(complex_long_long)
    CM_BUILDER_BT_ACCESSOR(complex_unsigned_char)
    CM_BUILDER_BT_ACCESSOR(complex_unsigned_short)
    CM_BUILDER_BT_ACCESSOR(complex_unsigned_int)
    CM_BUILDER_BT_ACCESSOR(complex_unsigned_long)
    CM_BUILDER_BT_ACCESSOR(complex_unsigned_long_long)
    CM_BUILDER_BT_ACCESSOR(complex_float)
    CM_BUILDER_BT_ACCESSOR(complex_double)
    CM_BUILDER_BT_ACCESSOR(complex_long_double)

#undef CM_BUILDER_BT_ACCESSOR

private:
    /// Specifal type that is used as placeholder for type referenced by name
    class type_ref: public type_t {
    public:
        /// Prints type description to output stream
        void print_desc(std::ostream & str) const override {
            str << "typeref";
        }
    };

    /// Returns true if specified type is ref type from this builder
    bool is_ref(type_t * type) const;

    /// Returns true if CM composite type is derived from one of the ref types
    /// in this builder
    bool is_ref_derived(type_t * type) const;

    /// Returns true if CM qual composite type is derived from one of the ref
    /// types in this builder
    bool is_ref_derived(const qual_type & qt) const {
        return is_ref_derived(qt.type());
    }


    code_model & cm_;           ///< Reference to code model
    debug_info * dbg_;          ///< Pointer to debug info

    /// Map between type IDs and ref types
    std::map<std::string, std::unique_ptr<type_ref>> ref_types_;

    /// Map between IDs and current type (real or ref type)
    std::map<std::string, type_t*> real_ref_types_;

    /// Vector of new record types that needs layout
    std::vector<record_type*> new_rec_types_;
};


template <typename Parent, typename ... TemplateParams>
template <typename Params>
std::tuple<record_type*, bool>
template_builder<Parent, TemplateParams...>::get_or_create_rec_inst_r(const std::string & id,
                                                                      record_kind knd,
                                                                      const Params & params) {

    auto templ = get_or_create_template_record(knd);
    auto rec = templ->find_instantiation(params);
    bool is_new = false;

    if (!rec) {
        rec = templ->create_instantiation(params);
        bldr_.add_new_rec_type(rec);
        is_new = true;
    }

    if (!id.empty()) {
        bldr_.set_type(id, rec);
    }

    return {rec, is_new};
}


template <typename Parent, typename ... TemplateParams>
template <typename ... Params>
std::tuple<record_type*, bool>
template_builder<Parent, TemplateParams...>::get_or_create_rec_inst(const std::string & id,
                                                                    record_kind knd,
                                                                    Params && ... params) {

    auto templ = get_or_create_template_record(knd);
    auto rec = templ->find_instantiation(std::forward<Params>(params)...);
    bool is_new = false;

    if (!rec) {
        rec = templ->create_instantiation(std::forward<Params>(params)...);
        bldr_.add_new_rec_type(rec);
        is_new = true;
    }

    if (!id.empty()) {
        bldr_.set_type(id, rec);
    }

    return {rec, is_new};
}


}
