// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file ast_converter.hpp
/// Contains definition of the ast_converter class.

#pragma once

#include "../../cmsrc.hpp"
#include "../../../cm.hpp"
#include "../../../cxx/clang/ast_converter.hpp"
#include <clang/AST/RecursiveASTVisitor.h>


namespace cm::src::clang {


/// clang AST to code model converter
class ast_converter: public ::clang::RecursiveASTVisitor<ast_converter> {
    friend class ::clang::RecursiveASTVisitor<ast_converter>;

    /// Type of base class
    using base_t = ::clang::RecursiveASTVisitor<ast_converter>;

    /// Helper class for settings current declaration contexts and restoring them
    /// at scope exit
    struct context_setter {
    public:
        /// Constructs setter with invalid state
        explicit context_setter(ast_converter & conv):
            conv_{&conv}, old_ctx_{nullptr}, old_clang_ctx_{nullptr} {}

        /// Constructs setter. Sets new current context and clang declaration context
        /// and stores old contexts in this setter instance.
        explicit context_setter(ast_converter & conv,
                                decl_context * ctx,
                                const ::clang::DeclContext * clang_ctx):
        conv_{&conv}, old_ctx_{conv.ctx_}, old_clang_ctx_{conv.clang_ctx_} {
            assert(ctx != nullptr && "invalid context");
            assert(clang_ctx != nullptr && "invalid clang decl context");

            conv.ctx_ = ctx;
            conv.clang_ctx_ = clang_ctx;
        }

        /// Restores old current context and clang declaration context if setter state is valid
        ~context_setter() {
            restore();
        }

        context_setter(const context_setter &) = delete;
        context_setter & operator=(const context_setter &) = delete;
        context_setter(context_setter && other) = delete;
        context_setter & operator=(context_setter && other) = delete;

        /// Sets current context and clang context in AST converter.
        /// Saves previous contexts. The state of setter must be invalid
        void set(decl_context * ctx, const ::clang::DeclContext * clang_ctx) {
            assert(old_ctx_ == nullptr && "the state of setter is not invalid");
            assert(old_clang_ctx_ == nullptr && "the state of setter is not invalid");

            assert(ctx != nullptr && "context is null");
            assert(clang_ctx != nullptr && "clang decl context is null");

            old_ctx_ = conv_->ctx_;
            old_clang_ctx_ = conv_->clang_ctx_;

            conv_->ctx_ = ctx;
            conv_->clang_ctx_ = clang_ctx;
        }

        /// Restores old contexts in AST converter. Sets state of setter to invalid.
        void restore() {
            if (conv_) {
                conv_->ctx_ = old_ctx_;
                conv_->clang_ctx_ = old_clang_ctx_;
                old_ctx_ = nullptr;
                old_clang_ctx_ = nullptr;
            }
        }

    private:
        ast_converter * conv_;                          ///< Pointer to AST converter
        decl_context * old_ctx_;                        ///< Previous declaration context
        const ::clang::DeclContext * old_clang_ctx_;    ///< Previous clang context
    };
public:
    /// Constructs AST converter with specified reference to source code model
    ast_converter(source_code_model & mdl):
        cm_conv_{mdl.code_mdl()}, scm_{mdl} {}

    /// Default destructor
    ~ast_converter() = default;

    /// Deleted copy constructor
    ast_converter(const ast_converter &) = delete;

    /// Deleted copy assignment operator
    ast_converter & operator=(const ast_converter &) = delete;

    /// Deleted move constructor
    ast_converter(ast_converter &&) = delete;

    /// Converts AST context to source code model
    void convert(::clang::ASTContext & ctx);

private:
    /// Traverses namespace declaration
    bool TraverseNamespaceDecl(::clang::NamespaceDecl * clang_decl);

    /// Traverses record declaration
    bool TraverseCXXRecordDecl(::clang::CXXRecordDecl * clang_decl);

    /// Traverses class template declaration
    bool TraverseClassTemplateDecl(::clang::ClassTemplateDecl * clang_decl);

    /// Traverses typedef declaration
    bool TraverseTypedefDecl(::clang::TypedefDecl * clang_decl);

    /// Traverses function template declaration
    bool TraverseFunctionTemplateDecl(::clang::FunctionTemplateDecl * clang_decl);

    /// Traverses class template specialization
    bool
    TraverseClassTemplateSpecilizationDecl(::clang::ClassTemplatePartialSpecializationDecl * decl);

    /// Traverses function declaration
    bool TraverseFunctionDecl(::clang::FunctionDecl * clang_decl);

    /// Traverses method declaration
    bool TraverseCXXMethodDecl(::clang::CXXMethodDecl * clang_decl);

    /// Traverses variable declaration
    bool TraverseVarDecl(::clang::VarDecl * clang_var_decl);

    /// Traverses field declaration
    bool TraverseFieldDecl(::clang::FieldDecl * clang_field_decl);

    /// Traverses type specifier
    bool TraverseTypeLoc(::clang::TypeLoc loc);

    bool VisitNestedNameSpecifierLoc(::clang::NestedNameSpecifierLoc loc);

    /// Converts list of lists of template parameters into chain of template declarations
    std::unique_ptr<template_decl> convert_template_lists(const ::clang::DeclaratorDecl * decl);

    /// Converts template parameters and adds them to template declaration
    void convert_template_params(template_decl * decl,
                                 template_ * templ,
                                 const ::clang::TemplateParameterList & pars);

    /// Converts record declaration
    std::unique_ptr<record_decl> convert_record_decl(ast_node * parent,
                                                     ::clang::CXXRecordDecl * clang_decl);

    /// Converts declaration name
    std::unique_ptr<identifier> convert_name(ast_node * parent,
                                             const ::clang::NamedDecl * clang_decl);

    /// Converts declaration name and scope
    void convert_named_decl(named_decl * decl, const ::clang::DeclaratorDecl * clang_decl);

    /// Converts functional declaration
    void convert_functional_decl(function * func,
                                 functional_decl * decl,
                                 ::clang::FunctionDecl * clang_decl);


    ////////////////////////////////////////////////////////////
    // Scope specifiers

    /// Converts nested name specifier location info to scope specifier
    std::unique_ptr<scope_spec> convert_scope_spec(ast_node * parent,
                                                   const ::clang::NestedNameSpecifier * nss,
                                                   const ::clang::NestedNameSpecifierLoc & nss_loc);

    /// Converts type location info to scope specifier
    std::unique_ptr<scope_spec> convert_type_scope(ast_node * parent, const ::clang::TypeLoc & loc);

    /// Converts tag scope specifier
    std::unique_ptr<scope_spec>
    convert_tag_scope_spec(ast_node * parent, const ::clang::TagTypeLoc & loc);

    /// Converts template scope specifier
    std::unique_ptr<scope_spec>
    convert_template_scope_spec(ast_node * parent,
                                const ::clang::TemplateSpecializationTypeLoc & loc);


    ////////////////////////////////////////////////////////////
    // Type specifiers

    /// Converts builtin type location to builtin type specifier
    std::unique_ptr<builtin_type_spec>
    convert_builtin_type_spec(ast_node * parent, const ::clang::BuiltinTypeLoc & loc);

    /// Converts pointer type location to pointer type specifier
    std::unique_ptr<pointer_type_spec>
    convert_pointer_type_spec(ast_node * parent, const ::clang::PointerTypeLoc & loc);

    /// Converts lvalue reference type location to lvalue reference type specifier
    std::unique_ptr<lvalue_reference_type_spec>
    convert_lvalue_reference_type_spec(ast_node * parent,
                                       const ::clang::LValueReferenceTypeLoc & loc);

    /// Converts rvalue reference type location to rvalue reference type specifier
    std::unique_ptr<rvalue_reference_type_spec>
    convert_rvalue_reference_type_spec(ast_node * parent,
                                       const ::clang::RValueReferenceTypeLoc & loc);

    /// Converts parens () type specifier
    std::unique_ptr<parens_type_spec>
    convert_parens_type_spec(ast_node * parent, const ::clang::ParenTypeLoc & loc);

    /// Converts function type specofier
    std::unique_ptr<function_type_spec>
    convert_function_type_spec(ast_node * parent, const ::clang::FunctionTypeLoc & loc);

    /// Converts template record instantiation type spec
    std::unique_ptr<template_substitution_type_spec>
    convert_template_subst_type_spec(ast_node * parent,
                                     const ::clang::TemplateSpecializationTypeLoc & loc);

    /// Converts record type spec
    std::unique_ptr<type_spec>
    convert_record_type_spec(ast_node * parent, const ::clang::TagTypeLoc & loc);

    /// Converts elaborated type specified
    std::unique_ptr<type_spec> convert_elaborated_type_spec(ast_node * parent,
                                                            const ::clang::ElaboratedTypeLoc & loc);

    /// Converts template parameter type specifier
    std::unique_ptr<template_param_type_spec>
    convert_template_param(ast_node * parent, const ::clang::TemplateTypeParmTypeLoc & loc);

    /// Converts clang type location to type specifier
    std::unique_ptr<type_spec> convert_type_spec(ast_node * parent, const ::clang::TypeLoc & loc);


    ////////////////////////////////////////////////////////////
    // Common functions

    /// Converts template specifier and template arguments
    void convert_template_and_arguments(const ::clang::TemplateSpecializationTypeLoc & loc,
                                        template_substitution_spec * parent,
                                        template_name * templ);

    /// Converts source location
    source_position convert_source_pos(const ::clang::SourceLocation & loc);

    /// Converts pair of clang source locations into source range
    source_file_range convert_source_range(const ::clang::SourceLocation & start,
                                           const ::clang::SourceLocation & end);

    /// Converts clang source range into source range
    source_file_range convert_source_range(const ::clang::SourceRange & range);

    /// Converts clang source location and end offset to source range
    source_file_range convert_source_range(const ::clang::SourceLocation & start,
                                           unsigned int offset);

    /// Converts function parameter declaration
    std::unique_ptr<function_parameter_decl>
    convert_function_parameter(functional_decl * parent,
                               function_parameter * par,
                               const ::clang::ParmVarDecl * clang_decl);


    cm::clang::ast_converter cm_conv_;  ///< Code model AST converter
    source_code_model & scm_;           ///< Reference to source code model

    /// Clang AST context
    ::clang::ASTContext * clang_ast_ctx_ = nullptr;

    /// Current declaration context
    decl_context * ctx_ = nullptr;

    /// Current opaque node for converting parts of function bodies
    opaque_ast_node * func_body_ = nullptr;

    /// Current clang context
    const ::clang::DeclContext * clang_ctx_ = nullptr;
};


}
