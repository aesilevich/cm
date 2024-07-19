// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file namespace.hpp
/// Contains definition of the namespace_ class

#pragma once

#include "context.hpp"
#include "function.hpp"
#include "named_entity.hpp"
#include "named_type.hpp"
#include "qual_type.hpp"
#include "range_utils.hpp"
#include "record_type.hpp"
#include "variable.hpp"
#include <ranges>
#include <memory>
#include <vector>
#include <unordered_map>


namespace cm {


/// Represents namespace
class namespace_: public entity_context_t<>, public named_entity {
public:
    /// Constructs namespace with specified parent namespace and name
    namespace_(namespace_ * parent, const std::string & nm):
    entity_context_t<>{parent},
    context{parent},
    context_entity{parent},
    name_{nm} {
    }

    /// Creates nested namespace with specified name. Namespace must not exist.
    namespace_ * create_namespace(const std::string & name);

    /// Gets or creates nested namespace with specified name.
    /// Returns pointer to fetched/created namespace
    namespace_ * get_or_create_namespace(const std::string & name);

    /// Creates new anonymous nested namespace
    namespace_ * create_anon_namespace();

    /// Searches for nested namespace with specified name. Returns pointer
    /// to namespace or nullptr if namespace was not found
    const namespace_ * find_namespace(const std::string & name) const;

    /// Searches for nested namespace with specified name. Returns pointer
    /// to namespace or nullptr if namespace was not found
    namespace_ * find_namespace(const std::string & name) {
        auto cthis = const_cast<const namespace_*>(this);
        return const_cast<namespace_*>(cthis->find_namespace(name));
    }

    /// Returns name of namespace
    const std::string & name() const override { return name_; }

    /// Returns range of const nested namespaces
    const auto & namespaces() const { return c_namespaces_r_; }

    /// Returns range of nested namespaces
    const auto & namespaces() { return namespaces_r_; }

    /// Removes nested namespace
    void remove_namespace(namespace_ * ns) {
        auto cnt = namespaces_.erase(ns->name());
        assert(cnt && "Can't find namespace in map");
    }

    /// Prints namespace description to output stream
    void print_desc(std::ostream & str) const override {
        str << "namespace " << name();
    }

    /// Dumps namespace entities to output stream
    void dump_entities(std::ostream & str, const dump_options & opts, unsigned int indent) const;

    /// Dumps namespace to output stream
    void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const override;

private:
    std::string name_;          ///< Name of namespace

    /// Map of nested namespaces
    std::unordered_map<std::string, std::shared_ptr<namespace_>> namespaces_;

    /// Range of pointers to namespaces
    decltype(util::make_second_ptr_range(namespaces_)) namespaces_r_ = util::make_second_ptr_range(namespaces_);

    /// Range of const pointers to namespaces
    decltype(util::make_const_second_ptr_range(namespaces_)) c_namespaces_r_ = util::make_const_second_ptr_range(namespaces_);

    /// Number of nested anonymous namespaces
    unsigned int num_anon_ns_ = 0;
};


}
