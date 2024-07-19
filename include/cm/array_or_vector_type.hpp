// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file array_or_vector_type.hpp
/// Contains definition of the array_or_vector_type class.

#pragma once

#include "type.hpp"
#include "qual_type.hpp"
#include <cassert>


namespace cm {


/// Array or vector type ID
struct array_or_vector_type_id {
    const type_t * type;
    uint64_t size;

    bool operator==(const array_or_vector_type_id & id2) const {
        return type == id2.type && size == id2.size;
    }
};


/// Base class for array and vector types in code model
class array_or_vector_type: public type_t {
public:
    /// Constructor, makes array or vector type type with specified element
    /// type and size
    array_or_vector_type(type_t * b, uint64_t sz):
    base_{b}, size_{sz} {
        assert(base_ && "Array element type should not be null");
        base_->add_use(this);
    }

    /// Destructor, removes use of element type
    ~array_or_vector_type() override {
        base_->remove_use(this);
    }

    /// Returns type of array or vector element
    auto base() {
        return base_;
    }

    /// Returns type of array or vector element
    const type_t * base() const {
        return base_;
    }

    /// Returns array or vector size
    auto size() const {
        return size_;
    }

    /// Creates array or vector type description
    array_or_vector_type_id type_id() const {
        return {base_, size_};
    }

private:
    type_t * base_;         ///< Type of array element
    uint64_t size_;         ///< Array size
};


/// Hash function for the array_or_vector_type_id
struct array_or_vector_type_id_hash {
    using argument_type = const array_or_vector_type_id &;
    using result_type = size_t;
    result_type operator()(argument_type arg) const {
        auto res = static_cast<size_t>(reinterpret_cast<intptr_t>(arg.type));
        res |= arg.size;
        return res;
    }
};


}


namespace std {
    /// std::hash specialization for array_type_id
    template <> struct hash<cm::array_or_vector_type_id>:
        public cm::array_or_vector_type_id_hash {};
}

