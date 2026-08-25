// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file vector_type.hpp
/// Contains definition of the vector_type class.

#pragma once

#include "builtin_type.hpp"
#include "type.hpp"
#include <cassert>
#include <cstdint>


namespace cm {


/// Vector type ID
struct vector_type_id {
    const type_t * type;
    uint64_t size;

    bool operator==(const vector_type_id & id2) const {
        return type == id2.type && size == id2.size;
    }
};


/// Represents vector (SSE/AVX) type in code model
class vector_type: public type_t {
public:
    /// Constructor, makes vector type with specified element
    /// type and size
    vector_type(type_t * b, uint64_t sz):
    base_{b}, size_{sz} {
        assert(base_ && "vector element type should not be null");
        assert(b->is<builtin_type>() && "element of vector type must be builtin type");
        assert(sz != SIZE_MAX && "vector type must have size");
        base_->add_use(this);
    }

    /// Destructor, removes use of element type
    ~vector_type() override {
        base_->remove_use(this);
    }

    /// Returns type of vector element
    type_t * base() {
        return base_;
    }

    /// Returns type of vector element
    const type_t * base() const {
        return base_;
    }

    /// Returns vector size
    auto size() const {
        return size_;
    }

    /// Creates vector type description
    vector_type_id type_id() const {
        return {base_, size_};
    }

    /// Prints type description to output stream
    void print_desc(std::ostream & str) const override {
        base()->print_desc(str);
        str << "<" << size() << ">";
    }

private:
    type_t * base_;         ///< Type of vector element
    uint64_t size_;         ///< Vector size
};


/// Hash function for the vector_type_id
struct vector_type_id_hash {
    using argument_type = const vector_type_id &;
    using result_type = size_t;
    result_type operator()(argument_type arg) const {
        auto res = static_cast<size_t>(reinterpret_cast<intptr_t>(arg.type));
        res |= arg.size;
        return res;
    }
};


}


namespace std {
    /// std::hash specialization for vector_type_id
    template <> struct hash<cm::vector_type_id>:
        public cm::vector_type_id_hash {};
}
