// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file namespace.cpp
/// Contains implementation of the namespace_ class.

#include "pch.hpp"
#include "cm/namespace.hpp"
#include <sstream>


namespace cm {


namespace_ *namespace_::create_namespace(const std::string & name) {
    auto & ns_ptr = namespaces_[name];
    assert(!ns_ptr && "namespace with specified name already exists");
    ns_ptr = std::make_shared<namespace_>(this, name);
    return ns_ptr.get();
}


namespace_ * namespace_::get_or_create_namespace(const std::string & name) {

    assert(!name.empty() && "namespace name should not be empty");

    auto & nsps_ptr = namespaces_[name];
    if (nsps_ptr)
        return nsps_ptr.get();

    nsps_ptr = std::make_shared<namespace_>(this, name);
    return nsps_ptr.get();
}


namespace_ * namespace_::create_anon_namespace() {
    // creating key for anonymous namespace
    ++num_anon_ns_;
    std::ostringstream str;
    str << "<##anon_namespace_" << num_anon_ns_ << ">";

    // creating anonymous namespace and adding it into map
    auto ns = std::make_shared<namespace_>(this, "");
    auto res = namespaces_.insert(std::make_pair(str.str(), ns));
    assert(res.second && "anon namespace with same key already exists");
    return ns.get();
}


const namespace_ * namespace_::find_namespace(const std::string & name) const {
    auto it = namespaces_.find(name);
    if (it == namespaces_.end()) {
        return nullptr;
    }

    return it->second.get();
}


void namespace_::dump_entities(std::ostream & str,
                               const dump_options & opts,
                               unsigned int indent) const {
    // dumping namespaces
    for (auto && ns : namespaces()) {
        ns->dump(str, opts, indent);
    }

    // dumping other context entities
    context::dump_entities(str, opts, indent);
}


void namespace_::dump(std::ostream & str, const dump_options & opts, unsigned int indent) const {
    print_indent_spaces(str, indent);
    print_desc(str);

    if (!name().empty()) {
        str << ' ';
    }

    str << "{\n";
    dump_entities(str, opts, indent + 1);

    print_indent_spaces(str, indent);
    str << "}\n";
}


}
