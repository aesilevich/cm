// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file qual_type.hpp
/// Contains definition of the qual_type class.

#pragma once

#include "type.hpp"
#include <functional>
#include <sstream>
#include <type_traits>


namespace cm {


/// Base class for all qual types
class qual_type_base {
public:
    /// Consturcts qualified type base class with specified const and volatile predicates
    qual_type_base(bool is_c = false, bool is_v = false):
        is_const_{is_c}, is_volatile_{is_v} {}

    /// Default destructor
    virtual ~qual_type_base() = default;

    /// Returns pointer to const type
    virtual const type_t * ctype() const = 0;

    /// Returns true if type has const qualifier
    bool is_const() const { return is_const_; }

    /// Returns true if type has volatile qualifier
    bool is_volatile() const { return is_volatile_; }

    /// Sets const flag for qual type
    void set_const(bool val) { is_const_ = val; }

    /// Sets volatile flag for qual type
    void set_volatile(bool val) { is_volatile_ = val; }

    /// Writes qualifiers to output stream
    void print_qual(std::ostream & str, bool first_space, bool last_space) const;

    /// Prints type description to output stream
    void print_desc(std::ostream & str) const;

private:
    bool is_const_;             ///< Has const qualifier?
    bool is_volatile_;          ///< Has volatile qualifier?
};


/// Constant qualifier type
template <typename T = type_t>
class qual_type_t: public qual_type_base {
    static_assert(std::is_base_of<type_t, T>::value, "T should be derived from type_t");

public:
    /// Constructor, makes const qual type with specified type and qualifiers
    qual_type_t(T * t = nullptr, bool is_c = false, bool is_v = false):
    qual_type_base{is_c, is_v}, type_{t} {
    }

    /// Constructs qual type from another convertible qual type
    template <typename T2>
    qual_type_t(const qual_type_t<T2> & qt):
    qual_type_base{qt.is_const(), qt.is_volatile()}, type_{qt.type()} {
    }

    /// Returns type
    auto type() const { return type_; }

    /// Sets type in CV type
    void set_type(T * t) { type_ = t; }

    /// Returns qual type with replaced pointer to type
    qual_type_t<T> replaced_type(T * t) const {
        return qual_type_t<T>{t, is_const(), is_volatile()};
    }

    /// Returns qual type with replaced pointer to type if
    /// the original pointer is equal to src
    qual_type_t<T> replaced_type(T * src, T * dst) const {
        if (type() == src) {
            return replaced_type(dst);
        } else {
            return *this;
        }
    }

    /// Returns pointer to const type
    const type_t * ctype() const override { return type(); }

    /// Returns type name for named type
    std::string name() const {
        return type()->name();
    }

    /// Returns true if qual type is null
    bool is_null() const { return type_ == nullptr; }

    /// Returns true if type is not null
    explicit operator bool() const { return !is_null(); }

    /// Returns true of type is null
    bool operator!() const { return is_null(); }

    /// Casts qual type to another qual type. Returns invalid qual type
    /// if cast is not possible
    template <typename T2>
    auto cast() const {
        auto casted_type = type_->template cast<std::remove_const_t<T2>>();
        using ret_type = std::conditional_t<std::is_const<T>::value, const T2, T2>;
        return qual_type_t<ret_type>{casted_type, is_const(), is_volatile()};
    }

    /// Returns pointer to type that the type() member function returns
    T * operator->() const { return type(); }

private:
    /// Writes CV qualifiers to output stream
    void write_cv(std::ostream & str) const {
        if (is_const()) {
            str << " const";
        }

        if (is_volatile()) {
            str << " volatile";
        }
    }

protected:
    T * type_;                  ///< Pointer to type
};


template <typename Type> using const_qual_type_t = qual_type_t<const Type>;

using const_qual_type = qual_type_t<const type_t>;
using qual_type = qual_type_t<>;


/// Implementation of qualified use of type
template <typename Type = type_t>
class qual_type_use_impl: virtual public entity_use {
public:
    /// Constructs use of type. Adds this instance to the list of entity uses.
    explicit qual_type_use_impl(const qual_type_t<Type> & t):
    type_{t} {
        do_add_use();
    }

    qual_type_use_impl(const qual_type_use_impl &) = delete;
    qual_type_use_impl(qual_type_use_impl &&) = delete;
    qual_type_use_impl & operator=(const qual_type_use_impl &) = delete;
    qual_type_use_impl & operator=(qual_type_use_impl &&) = delete;

    /// Destroys use of type. Removes this instance from the list of entity uses.
    ~qual_type_use_impl() override {
        do_remove_use();
    }

    /// Returns used type with qualifiers
    qual_type_t<Type> & type() { return type_; }

    /// Returns const used type with qualifiers
    const_qual_type_t<Type> type() const { return type_; }

    /// Sets used type with qualifiers. Removes this use from the list of uses of
    /// current type and adds this use to the list of uses of the new type
    void set_type(const qual_type_t<Type> & t) {
        do_remove_use();
        type_ = t;
        do_add_use();
    }

private:
    /// Adds this use to the list of uses of current entity if it's not null
    void do_add_use() {
        if (type_) {
            type_->add_use(this);
        }
    }

    /// Removes this use from the list of uses of current used entity if it's not null
    void do_remove_use() {
        if (type_) {
            type_->remove_use(this);
        }
    }

    qual_type_t<Type> type_;        ///< Used type
};


/// Returns true if qualified types are equal
template <typename T>
inline bool operator==(const qual_type_t<T> & ct1, const qual_type_t<T> & ct2) {
    return ct1.type() == ct2.type() &&
           ct1.is_const() == ct2.is_const() &&
           ct1.is_volatile() == ct2.is_volatile();
}


/// Returns true if qualified types are not equal
template <typename T>
inline bool operator!=(const qual_type_t<T> & ct1, const qual_type_t<T> & ct2) {
    return !(ct1 == ct2);
}


/// Less operator for const qual types (for std::map / std::set)
template <typename T>
inline bool operator<(const qual_type_t<T> & ct1, const qual_type_t<T> & ct2) {
    if (ct1.is_const() == ct2.is_const()) {
        if (ct1.is_volatile() == ct2.is_volatile()) {
            return ct1.type() < ct2.type();
        } else {
            return !ct1.is_volatile();
        }
    } else {
        return !ct1.is_const();
    }
}


/// Calculates hash of qual type
template <typename Type>
struct qual_type_hash {
    using argument_type = qual_type_t<Type>;
    using result_type = size_t;
    result_type operator()(argument_type arg) const {
        auto iptr = reinterpret_cast<intptr_t>(arg.type());
        auto res = static_cast<size_t>(iptr);

        if (arg.is_const()) {
            res |= 1;
        }

        if (arg.is_volatile()) {
            res |= 2;
        }

        return res;
    }
};


}


namespace std {
    /// std::hash specialization for qual type
    template <typename Type> struct hash<cm::qual_type_t<Type>>:
        public cm::qual_type_hash<Type> {};
}
