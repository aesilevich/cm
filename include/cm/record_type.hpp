// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file record_type.hpp
/// Contains definition of the record_type class.

#pragma once

#include "context.hpp"
#include "function.hpp"
#include "record_kind.hpp"
#include "template_function.hpp"
#include <ranges>
#include <map>
#include <vector>


namespace cm {


class record;
class record_type;
class named_record_type;
class template_method;


/// Base class for all members
class member {
public:
    /// Default constructor
    explicit member() = default;

    /// Virtual destructor
    virtual ~member() = default;
};


/// Instance variable
class field: public variable, public member {
public:
    /// Constructor, makes isntance variable with specified name,
    /// type and access level
    inline field(record * rec,
                             const std::string & nm,
                             const qual_type & tp,
                             unsigned int bsize = 0);

    /// Returns size of bitfield
    auto bit_size() const { return bit_size_; }

    /// Dumps variable to output stream
    void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const override;

private:
    unsigned int bit_size_;         ///< Number of bits in bitfield
};


/// Static record variable
class static_record_variable: public variable, public member {
public:
    /// Constructs static record variable with specified name, type, and access specified
    static_record_variable(record * rec,
                           const std::string & nm,
                           const qual_type & tp);
};


/// Record method
class method: virtual public function, public member {
public:
    /// Constructs method
    explicit inline method(record * rec);

    /// Dumps method to output stream
    void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const override;
};


/// Named (user defined non template) method
class named_method: public method, public named_function {
public:
    /// Constructs method
    explicit inline named_method(record * rec, const std::string & nm);

    /// Dumps method to output stream
    void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const override;
};


/// Static record member function
class static_record_function: virtual public function, public member {
public:
    /// Constructors static record member function with specified
    /// name, type and access level
    inline static_record_function(record * rec,
                                  const std::string & nm,
                                  function_type * tp);
};


class instance_template_function_instantiation;


/// Represents record (class, struct, union) in code model.
/// Record is not necessary a type, it may be a template record.
class record: public entity_context_t<static_record_variable, function> {
public:
    /// Constructs record with specified context and record kind
    explicit record(context * dctx, record_kind knd):
        context_entity(dctx),
        entity_context_t<static_record_variable, function>(dctx),
        kind_{knd} {}

    /// Returns kind of record
    record_kind kind() const { return kind_; }

    /// Sets record kind
    void set_kind(record_kind knd) {
        kind_ = knd;
    }

    /// Returns default access level for this context
    access_level default_access_level() const override {
        if (kind() == record_kind::class_) {
            return access_level::private_;
        } else {
            return access_level::public_;
        }
    }

    /// Creates unnamed entity in record context
    template <typename Entity, typename ... Args>
    Entity * create_entity(Args && ... args) {
        return create_entity_impl<Entity>(this, std::forward<Args>(args)...);
    }

    /// Create named entity in record context
    template <typename Entity, typename ... Args>
    Entity * create_named_entity(const std::string & name, Args && ... args) {
        return create_named_entity_impl<Entity>(this, name, std::forward<Args>(args)...);
    }


    //////////////////////////////////////////////////////////////////////
    // Base classes

    /// Returns range of base records
    auto bases() const {
        return bases_ | std::ranges::views::all;
    }

    /// Returns range of base types casted to record_type
    auto base_recs() const {
        auto fn = [](auto && t) {
            auto rec = t->untypedef()->template cast<record_type>();
            assert(rec && "base type is not a record");
            return rec;
        };

        return bases() | std::ranges::views::transform(fn);
    }

    /// Adds base class into record
    void add_base(type_t * base, access_level acc = access_level::private_) {
        // checking that record is not in vector of base classes
        assert(std::ranges::find(bases_, base) == std::ranges::end(bases_) &&
               "record is already an base class");

        // TODO: implement access levels for base types

        base->add_use(this);
        bases_.push_back(base);
    }

    /// Removes all base records
    void remove_all_bases() {
        for (auto && b : bases_) {
            b->remove_use(this);
        }

        bases_.clear();
    }

    /// Replaces base type
    void replace_base(type_t * src, type_t * dst) {
        for (auto & base : bases_) {
            if (base == src) {
                base->remove_use(this);
                base = dst;
                base->add_use(this);
            }
        }
    }

    //////////////////////////////////////////////////////////////////////
    // Fields

    /// Returns range of const fields
    auto fields() const {
        return entities<field>();
    }

    /// Returns range of fields
    auto fields() {
        return entities<field>();
    }

    /// Creates field with specified name and type
    field * create_field(const std::string & name,
                         const qual_type & type,
                         access_level acc = access_level::public_,
                         unsigned int bit_size = 0);


    //////////////////////////////////////////////////////////////////////
    // Static variables

    /// Returns range of const static variables
    auto vars() const {
        return entities<static_record_variable>();
    }

    /// Returns range of static variables
    auto vars() {
        return entities<static_record_variable>();
    }

    /// Creates static public variable with specified name and type in record
    static_record_variable * create_var(const std::string & name, const qual_type & type) override {
        return create_named_entity_impl<static_record_variable>(this, name, type);
    }


    //////////////////////////////////////////////////////////////////////
    // Methods

    /// Creates method and adds it to the end of the list of declarations
    named_method * create_method(const std::string & name,
                                 access_level spec = access_level::public_);

    /// Creates template method and adds it to the list of record declarations
    inline template_method *
    create_template_method(const std::string & name);


    //////////////////////////////////////////////////////////////////////
    // Utilities

    /// Dumps record header to output stream
    void dump_header(std::ostream & str, const dump_options & opts) const;

    /// Dumps record header and contents to output stream
    void dump_header_and_contents(std::ostream & str,
                                  const dump_options & opts,
                                  unsigned int indent) const;


private:
    /// Record kind
    record_kind kind_;

    /// Vector of base types (non necessary records, may be typedef or template instantiation)
    std::vector<type_t*> bases_;
};


/// Represents record type in code model
class record_type: public record, virtual public context_type {
public:
    /// Constructs record type with specified parent context and record kind
    explicit record_type(context * ctx, record_kind knd):
        record{ctx, knd},
        context_type{ctx},
        context{ctx},
        context_entity{ctx} {}

    /// Prints record description to output stream
    void print_desc(std::ostream & str) const override {
        str << "(unnamed record)";
    }

    /// Dumps record type to output stream
    void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const override;
};


/// Named record type
class named_record_type: virtual public named_type, public record_type {
public:
    /// Constructs record with specified context and name
    named_record_type(context * ctx, const std::string & nm, record_kind knd):
        named_type{ctx, nm},
        record_type{ctx, knd},
        context_type{ctx},
        context{ctx},
        context_entity{ctx} {}

    /// Prints record description to output stream
    void print_desc(std::ostream & str) const override {
        named_type::print_desc(str);
    }
};



////////////////////////////////////////////////////////////////////////////////
// Implementation


inline method::method(record * rec):
function{rec} {
}


inline named_method::named_method(record * rec, const std::string & nm):
method{rec},
named_function{rec, nm},
function{rec},
named_context_entity{rec, nm},
context_entity{rec},
context{rec} {
}


inline static_record_function::static_record_function(record * rec,
                                                      const std::string & nm,
                                                      function_type * tp):
function{rec} {
}


inline field::field(record * rec,
                                            const std::string & nm,
                                            const qual_type & tp,
                                            unsigned int bsize):
variable(rec, nm, tp), context_entity(rec), bit_size_{bsize}, single_type_use{tp} {

    // number of bits can be specified only for integral types

    if (bit_size_ == 0) {
        return;
    }

    auto bt_type = tp.cast<builtin_type>();
    assert(bt_type && "bitfield can only be of builtin type");

    switch (bt_type->kind()) {
    case builtin_type::kind_t::bool_:
    case builtin_type::kind_t::char_:
    case builtin_type::kind_t::short_:
    case builtin_type::kind_t::int_:
    case builtin_type::kind_t::long_:
    case builtin_type::kind_t::long_long_:
    case builtin_type::kind_t::unsigned_char_:
    case builtin_type::kind_t::unsigned_short_:
    case builtin_type::kind_t::unsigned_int_:
    case builtin_type::kind_t::unsigned_long_:
    case builtin_type::kind_t::unsigned_long_long_:
    case builtin_type::kind_t::wchar_t_:
    case builtin_type::kind_t::char16_t_:
    case builtin_type::kind_t::char32_t_:
        break;
    default:
        assert(false && "bitfield can only be of integral builtin type");
    }
}


inline static_record_variable::static_record_variable(record * rec,
                                                      const std::string & nm,
                                                      const qual_type & tp):
variable{rec, nm, tp}, context_entity{rec}, single_type_use{tp} {}



inline auto context::records() const { return entities<record_type>(); }

inline auto context::records() { return entities<record_type>(); }

inline auto context::named_records() const { return entities<named_record_type>(); }

inline auto context::named_records() { return entities<named_record_type>(); }



}
