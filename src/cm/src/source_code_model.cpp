// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file source_code_model.cpp
/// Contains implementation of the source_code_model class

#include "pch.hpp"
#include "cm/src/source_code_model.hpp"
#include <sstream>


namespace cm::src {


void source_code_model::print(std::ostream & str) {    
}


source_file * source_code_model::find_source(const std::filesystem::path & p, bool find_name) {
    if (auto file = code_mdl_.find_source(p, find_name)) {
        if (auto it = sources_.find(file); it != sources_.end()) {
            return it->second.get();
        }
    }

    return nullptr;
}


const source_file * source_code_model::find_source(const std::filesystem::path & p,
                                                   bool find_name) const {
    if (auto file = code_mdl_.find_source(p, find_name)) {
        if (auto it = sources_.find(file); it != sources_.end()) {
            return it->second.get();
        }
    }

    return nullptr;
}


source_file * source_code_model::get_or_create_source(const std::filesystem::path & p) {
    auto cm_src = code_mdl_.source(p);
    auto & src = sources_[cm_src];
    if (src) {
        return src.get();
    }

    src = std::make_unique<source_file>(cm_src);
    return src.get();
}


class ast_node_finder: public const_ast_visitor {
public:
    /// Constructs finder with specified search source position
    explicit ast_node_finder(const source_position & pos):
        pos_{pos} {}

    /// Traverses through AST node. Returns true if traversing should continue after call.
    bool traverse(const ast_node * node) override {
        if (node->source_range().is_valid()) {
            // if node has source range information then check that specified source position
            // is located withing node range
            if (node->source_range().range().start() > pos_ ||
                node->source_range().range().end() < pos_) {
                // skipping this node and continue searching
                return true;
            }

            // setting current node as the result of searching
            result_ = node;

            // traversing children nodes to find node with better position match
            node->traverse_children(*this);

            // stopping search
            return false;
        } else {
            // if node does not have source range when trying search in children nodes
            return node->traverse_children(*this);
        }
    }

    /// Returns search result
    const ast_node * result() const { return result_; }

private:
    source_position pos_;                   ///< Search position
    const ast_node * result_ = nullptr;     ///< Result of search
};


/// Searches for AST node located at specified source position
const ast_node * source_code_model::find_node_at_pos(const source_file_position & pos) const {
    // looking for source file AST node
    auto src_it = sources_.find(pos.file());
    if (src_it == sources_.end()) {
        std::ostringstream msg;
        msg << "can't find source file '" << pos.file()->path() << "' in source code model";
        throw std::runtime_error{msg.str()};
    }

    ast_node_finder finder{pos.pos()};
    if (finder.traverse(src_it->second.get())) {
        // AST node not found
        assert(finder.result() == nullptr && "invalid finder result");
        return nullptr;
    }

    assert(finder.result() != nullptr && "invalid finder result");
    return finder.result();
}


const ast_node * source_code_model::find_child_node_at_pos(const ast_node * node,
                                                           const source_position & pos) {
    
    if (node->source_range().is_valid()) {
        // if node has source range information then check that specified source position
        // is located withing node rangte
        if (node->source_range().range().start() > pos ||
            node->source_range().range().end() < pos) {
            return nullptr;
        }
    }

    // checking node children
    //node->traverse_children(const_ast_visitor &v)

    return nullptr;
}


}
