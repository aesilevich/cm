// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file source_code_model.hpp
/// Contains definition of the source_code_model class.

#pragma once

#include "function_decl.hpp"
#include "namespace_decl.hpp"
#include "record_decl.hpp"
#include "source_file.hpp"
#include "template_record_decl.hpp"
#include "template_record_instantiation_type_spec.hpp"
#include "type_spec.hpp"
#include "../code_model.hpp"


namespace cm::src {


/// Source code model
class source_code_model {
public:
    /// Constructs emtpy source code model
    source_code_model() = default;

    /// Destroys source code model
    ~source_code_model() = default;

    /// Returns reference to underlying code model
    code_model & code_mdl() { return code_mdl_; }

    /// Returns const reference to underlying code model
    const code_model & code_mdl() const { return code_mdl_; }

    /// Prints source code model to output stream
    void print(std::ostream & str);


    //////////////////////////////////////////////////////////////////////
    // Source files

    /// Searches for existing source file with specified path. Returns nullptr if not found.
    source_file * find_source(const std::filesystem::path & p, bool find_name = false);

    /// Searches for existing source file with specified path. Returns nullptr if not found.
    const source_file * find_source(const std::filesystem::path & p, bool find_name = false) const;

    /// Gets existing or creates new source with specified path
    source_file * get_or_create_source(const std::filesystem::path & p);

    /// Returns range of source files
    auto sources() {
        auto fn = [](auto && pair) { return pair.second.get(); };
        return sources_ | std::ranges::views::transform(fn);
    }

    /// Returns range of const source files
    auto sources() const {
        auto fn = [](auto && pair) { return const_cast<const source_file*>(pair.second.get()); };
        return sources_ | std::ranges::views::transform(fn);
    }


    //////////////////////////////////////////////////////////////////////
    // Source position search

    /// Searches for AST node located at specified source position
    const ast_node * find_node_at_pos(const source_file_position & pos) const;

private:
    /// Searches for child AST node located at specified source position
    const ast_node * find_child_node_at_pos(const ast_node * node, const source_position & pos);

    code_model code_mdl_;           ///< Code model

    /// Map of sources in code model
    std::map<const cm::source_file*, std::unique_ptr<source_file>> sources_;
};


}
