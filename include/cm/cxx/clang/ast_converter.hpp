// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file ast_converter.hpp
/// Contains definition of the ast_converter class.

#pragma once

#include "../../cm.hpp"
#include "../../context_entity.hpp"
#include "../../record_type.hpp"
#include "../../template.hpp"
#include <clang/AST/Decl.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/DeclTemplate.h>
#include <clang/AST/TypeLoc.h>
#include <clang/AST/ASTContext.h>


namespace cm::clang {


/// clang AST to code model converter
class ast_converter {
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
                                context * ctx,
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
        void set(context * ctx, const ::clang::DeclContext * clang_ctx) {
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
        context * old_ctx_;                             ///< Previous context
        const ::clang::DeclContext * old_clang_ctx_;    ///< Previous clang context
    };

public:
    /// Constructs AST converter with specified reference to global code model
    /// and clang AST context
    ast_converter(code_model & mdl): mdl_{mdl} {}

    /// Default destructor
    ~ast_converter() = default;

    /// Deleted copy constructor
    ast_converter(const ast_converter &) = delete;

    /// Deleted copy assignment operator
    ast_converter & operator=(const ast_converter &) = delete;

    /// Default move constructor
    ast_converter(ast_converter &&) = default;

    /// Converts AST context to code model
    void convert(const ::clang::ASTContext & ctx);


    //////////////////////////////////////////////////////////////////////
    // Types conversion

    /// Converts clang qual type to code model type. Creates composite types if needed.
    qual_type convert_type(const ::clang::QualType & clang_type);

    /// Converts bultin clang type to code model type.
    type_t * convert_builtin_type(const ::clang::BuiltinType * clang_bt_type);

    /// Converts pointer clang type to code model type.
    pointer_type * convert_pointer_type(const ::clang::PointerType * clang_ptr_type);

    /// Converts lvalue reference clang type to code model type.
    lvalue_reference_type *
    convert_lvalue_reference_type(const ::clang::LValueReferenceType * clang_ref_type);

    /// Converts rvalue reference clang type to code model type.
    rvalue_reference_type *
    convert_rvalue_reference_type(const ::clang::RValueReferenceType * clang_ref_type);

    /// Converts array clang type to code model type.
    array_type * convert_array_type(const ::clang::ConstantArrayType * clang_arr_type);

    /// Converts type template parameter type to code model type
    type_template_parameter *
    convert_type_template_param_type(const ::clang::TemplateTypeParmType * clang_tpar_type);

    /// Converts template specialization type to code model type
    type_t *
    convert_template_spec_type(const ::clang::TemplateSpecializationType * clang_templ_spec);

    /// Converts clang function type to code model type. Creates new composite types if needed.
    function_type * convert_function_type(const ::clang::FunctionType * clang_func_type);

    /// Converts clang record type to code model type. Creates new empty record if needed.
    record_type * convert_record_type(const ::clang::RecordType * clang_rec_type);

    /// Converts dependent type
    dependent_type * convert_dependent_type(const ::clang::DependentNameType * clang_type);

    /// Converts decltype type
    decltype_type * convert_decltype_type(const ::clang::DecltypeType * clang_type);


    //////////////////////////////////////////////////////////////////////
    // Declarations

    /// Converts declaration
    void convert_decl(const ::clang::Decl * clang_decl);

    /// Converts namespace
    namespace_ * convert_ns(const ::clang::NamespaceDecl * clang_ns);

    /// Converts record
    record_type * convert_record(const ::clang::RecordDecl * clang_record_decl);

    /// Converts typedef
    typedef_type * convert_typedef(const ::clang::TypedefNameDecl * clang_typedef_decl);

    /// Converts function declaration
    function * convert_function(const ::clang::FunctionDecl * clang_func_decl);

    /// Converts variable declaration
    variable * convert_variable(const ::clang::VarDecl * clang_var_decl);

    /// Converts field declaration
    field * convert_field(cm::record * rec, const ::clang::FieldDecl * clang_field_decl);

    /// Converts class template declaration
    template_record * convert_template_class(const ::clang::ClassTemplateDecl * clang_templ_decl);

    /// Converts class template partial specialization
    template_record_partial_specialization * convert_template_partial_specialization(
            const ::clang::ClassTemplatePartialSpecializationDecl * clang_decl);

    /// Converts class template specialization declaration
    record * convert_template_class_spec(
            cm::template_record * templ,
            const ::clang::ClassTemplateSpecializationDecl * clang_spec_decl);

    /// Converts function template declaration
    template_function *
    convert_template_function(const ::clang::FunctionTemplateDecl * clang_templ_decl);

    /// Converts funciton template instantiation declaration
    template_function_instantiation *
    convert_template_function_inst(cm::template_function * templ,
                                   const ::clang::FunctionDecl * clang_func);

    /// Converts linkage specification declaration
    void convert_linkage_spec(const ::clang::LinkageSpecDecl * decl);


    //////////////////////////////////////////////////////////////////////
    // Entities

    /// Finds code model entity associated with clang declaration.
    /// First gets canonical declaration of clang declaration.
    /// Returns nullptr if associated context_entity not found
    context_entity * get_cm_entity(const ::clang::Decl * clang_decl);

    /// Finds code model enitty associated with clang declaration and converts it to
    /// specified type. Found context_entity must be convertible to specified type.
    /// Returns nullptr if context_entity is not found.
    template <typename Entity>
    Entity * get_cm_entity_as(const ::clang::Decl * clang_decl) {
        auto ent = get_cm_entity(clang_decl);
        if (!ent) {
            return nullptr;
        }

        auto casted_ent = dynamic_cast<Entity*>(ent);
        assert(casted_ent && "existing context_entity type mismatch");
        return casted_ent;
    }

    /// Adds code model context_entity associated with clang declaration.
    void add_cm_entity(const ::clang::Decl * clang_decl, context_entity * cm_ent);

private:
    /// Converts source location
    source_location convert_loc(const ::clang::SourceLocation & loc) const;

    /// Converts all record contents and adds it to code model record
    void fill_record_contents(cm::record * rec, const ::clang::RecordDecl * clang_record_decl);

    /// Converts and adds template parameters from clang AST to code model template
    void convert_template_params(templated_entity * templ,
                                 const ::clang::TemplateParameterList * clang_params);

    /// Updates template parameters locations and names to match parameters from the list
    void update_template_params(templated_entity * templ,
                                const ::clang::TemplateParameterList * clang_params);

    /// Converts function return type and paramters
    void convert_function_ret_type_and_params(
        function * func, const ::clang::FunctionDecl * clang_func);

    /// Updates function parameter names and locations to match marapeters from clang declaration
    void update_function_params(function * func, const ::clang::FunctionDecl * clang_func);

    /// Converts template arguments
    template_argument_desc_vector
    convert_template_arguments(const ::clang::ArrayRef<::clang::TemplateArgument> & args);

    /// Creates new empty record in context for clang record declaration
    record_type * create_new_record(const ::clang::RecordDecl * clang_rec_decl);

    /// Recursively searches for type template parameter dclaration
    /// in the chain of parent tempalte declarations
    const ::clang::TemplateTypeParmDecl *
    find_type_template_parameter_decl(const ::clang::Decl * curr_decl,
                                      const ::clang::TemplateTypeParmType * type);

    /// Executes one of passed functors depending on type of current decl context
    template <typename NsBlock, typename RecBlock>
    auto exec_ns_or_rec(NsBlock && ns_block, RecBlock && rec_block) {
        if (auto * rec_dctx = dynamic_cast<record*>(ctx_)) {
            return rec_block(rec_dctx);
        } else {
            auto * ns_dctx = dynamic_cast<namespace_*>(ctx_);
            assert(ns_dctx && "context must be record or namespace");
            return ns_block(ns_dctx);
        }
    }


    code_model & mdl_;      ///< Reference to code model

    context * ctx_ = nullptr;                           ///< Current code model context
    const ::clang::DeclContext * clang_ctx_ = nullptr;  ///< Current clang decl context

    /// Clang AST context
    const ::clang::ASTContext * clang_ast_ctx_ = nullptr;

    /// Map from clang canonical declarations to code model entities
    std::map<const ::clang::Decl *, context_entity *> decls_;
};


}
