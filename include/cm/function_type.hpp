// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file function_type.hpp
/// Contains definition of the function_type class.

#pragma once

#include "qual_type.hpp"
#include <ranges>
#include <numeric>
#include <vector>


namespace cm {


/// Data structure that completely identifies function type
struct function_type_id {
    qual_type ret_type;
    std::vector<qual_type> params;

    /// Constructs function type description with specified return type
    /// and parameters range
    template <typename Params>
    function_type_id(const qual_type & rtype, const Params & pars):
    ret_type{rtype}, params(std::begin(pars), std::end(pars)) {
    }

    /// Returns true if function descriptions are equal
    bool operator==(const function_type_id & other) const {
        return ret_type == other.ret_type && params == other.params;
    }
};


/// Represents function type
class function_type: public type_t {
public:
    /// Type of vector of qual types
    typedef std::vector<qual_type> qual_type_vector;

    /// Constructor, makes function type with specified return type
    function_type(const qual_type & rt):
    ret_type_{rt} {
        ret_type_->add_use(this);
    }

    /// Constructor, makes function type with specified return type
    /// and range of parameter types
    template <typename Params>
    function_type(const qual_type & rt, const Params & pars):
    function_type(rt) {
        params_.reserve(std::ranges::size(pars));
        for (auto && par : pars) {
            add_param(par);
        }
    }

    /// Destructor, removes uses of return type and all parameter types
    ~function_type() {
        ret_type_.type()->remove_use(this);

        for (auto && par : params()) {
            par.type()->remove_use(this);
        }
    }

    /// Returns return type
    const qual_type & ret_type() {
        return ret_type_;
    }

    /// Returns return type
    const_qual_type ret_type() const {
        return ret_type_;
    }

    /// Returns range of parameters
    const qual_type_vector & params() const {
        return params_;
    }

    /// Creates function type description
    function_type_id type_id() const {
        return function_type_id{ret_type_, params_};
    }

    /// Prints type description to output stream
    void print_desc(std::ostream & str) const override {
        ret_type()->print_desc(str);
        str << " (";

        bool first = true;
        for (auto && par : params()) {
            if (first) {
                first = false;
            } else {
                str << ", ";
            }

            par.print_desc(str);
        }

        str << ")";
    }

private:
    /// Adds parameter before parameter at specified position
    void add_param(const qual_type_vector::const_iterator & pos, const qual_type & ptype) {
        params_.insert(pos, ptype);
        ptype.type()->add_use(this);
    }

    /// Adds parameter at the end of parameters list
    void add_param(const qual_type & type) {
        add_param(params().end(), type);
    }

    /// Removes parameter at specified iterator
    void remove_param(const qual_type_vector::const_iterator & pos) {
        pos->type()->remove_use(this);
        params_.erase(pos);
    }


    qual_type ret_type_;                ///< Return type
    qual_type_vector params_;           ///< Function parameters
};


/// Hash function for the function_type_id value
struct function_type_id_hash {
    using argument_type = const function_type_id &;
    using result_type = size_t;

    result_type operator()(argument_type arg) const {
        auto type_hash = qual_type_hash<type_t>();
        auto res = qual_type_hash<type_t>()(arg.ret_type);
        const auto & par_hashes = arg.params | std::ranges::views::transform(type_hash);

        return std::accumulate(std::ranges::begin(par_hashes), std::ranges::end(par_hashes), res);
    }
};


}


namespace std {
    /// std::hash specialization for function_type_id
    template <> struct hash<cm::function_type_id>:
        public cm::function_type_id_hash {};
}
