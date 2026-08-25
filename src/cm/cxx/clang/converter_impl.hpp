// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file converter_impl.hpp
/// Contains definition of the converter_impl class.

#pragma once

#include "decl_map.hpp"
#include "function_converter.hpp"
#include "linkage_spec_converter.hpp"
#include "namespace_converter.hpp"
#include "record_converter.hpp"
#include "template_function_converter.hpp"
#include "template_record_converter.hpp"
#include "template_record_partial_specialization_converter.hpp"
#include "template_record_specialization_converter.hpp"
#include "template_variable_converter.hpp"
#include "template_variable_partial_specialization_converter.hpp"
#include "template_variable_specialization_converter.hpp"
#include "typedef_converter.hpp"
#include "variable_converter.hpp"
#include "cm/cm.hpp"
#include <clang/AST/ASTContext.h>


namespace cm::cxx::clang {

class types_converter;


/// Context for converting clang AST to code model. Stores maps between AST and code
/// model entities and current conveting state.
class converter_impl {
    using decl_converters = std::tuple <
        namespace_converter,
        template_record_converter,
        template_record_specialization_converter,
        record_converter,
        template_record_partial_specialization_converter,
        template_variable_converter,
        template_variable_partial_specialization_converter,
        template_variable_specialization_converter,
        typedef_converter,
        function_converter,
        variable_converter,
        template_function_converter,
        linkage_spec_converter
    >;

public:
    /// Helper class for settings current declaration contexts and restoring them
    /// at scope exit
    struct decl_context_setter {
    public:
        /// Constructs setter with invalid state
        explicit decl_context_setter(converter_impl & conv_ctx):
            conv_ctx_{&conv_ctx}, old_ctx_{nullptr}, old_clang_decl_{nullptr} {}

        /// Constructs setter. Sets new current context and clang declaration
        /// and stores old ones in this setter instance.
        explicit decl_context_setter(converter_impl & conv_ctx,
                                     context * ctx,
                                     const ::clang::Decl * clang_decl):
        conv_ctx_{&conv_ctx},
        old_ctx_{conv_ctx.decl_ctx()},
        old_clang_decl_{conv_ctx.clang_decl()} {
            assert(ctx != nullptr && "invalid context");
            assert(clang_decl != nullptr && "invalid clang declaration");

            conv_ctx_->set_decl_ctx(ctx);
            conv_ctx_->set_clang_decl(clang_decl);
        }

        /// Restores old current context and clang declaration context if setter state is valid
        ~decl_context_setter() {
            restore();
        }

        decl_context_setter(const decl_context_setter &) = delete;
        decl_context_setter & operator=(const decl_context_setter &) = delete;
        decl_context_setter(decl_context_setter && other) = delete;
        decl_context_setter & operator=(decl_context_setter && other) = delete;

        /// Sets current context and clang declaration in AST converter.
        /// Saves previous ones. The state of setter must be invalid
        void set(context * ctx, const ::clang::Decl * clang_decl) {
            assert(old_ctx_ == nullptr && "the state of setter is not invalid");
            assert(old_clang_decl_ == nullptr && "the state of setter is not invalid");

            assert(ctx != nullptr && "context is null");
            assert(clang_decl != nullptr && "clang declaration is null");

            old_ctx_ = conv_ctx_->decl_ctx();;
            old_clang_decl_ = conv_ctx_->clang_decl();

            conv_ctx_->set_decl_ctx(ctx);
            conv_ctx_->set_clang_decl(clang_decl);
        }

        /// Restores old contexts in AST converter. Sets state of setter to invalid.
        void restore() {
            if (conv_ctx_) {
                conv_ctx_->set_decl_ctx(old_ctx_);
                conv_ctx_->set_clang_decl(old_clang_decl_);
                old_ctx_ = nullptr;
                old_clang_decl_ = nullptr;
            }
        }

    private:
        converter_impl * conv_ctx_;              ///< Pointer to converter context
        context * old_ctx_;                     ///< Previous context
        const ::clang::Decl * old_clang_decl_;  ///< Previous clang declaration
    };


    /// Constructs converter
    explicit converter_impl(code_model & mdl, const ::clang::ASTContext & clang_ctx);

    /// Destroys converter
    ~converter_impl();

    converter_impl(const converter_impl &) = delete;
    converter_impl(converter_impl &&) = delete;
    converter_impl & operator=(const converter_impl &) = delete;
    converter_impl & operator=(converter_impl &&) = delete;

    /// Returns reference to code model
    code_model & mdl() const { return mdl_; }

    /// Returns reference to clang AST context
    const ::clang::ASTContext & clang_ctx() const { return clang_ctx_; }

    /// Converts entire clang translation unit into code model
    void convert();


    ////////////////////////////////////////////////////////////
    // Current conversion context

    /// Returns pointer to current declaration context in code model
    context * decl_ctx() const { return decl_ctx_; }

    /// Sets pointer to current declaration context in code model
    void set_decl_ctx(context * c) { decl_ctx_ = c; }

    /// Returns pointer to the clang declaration being converted
    const ::clang::Decl * clang_decl() const { return clang_decl_; }

    /// Sets pointer to the clang declaration being converted
    void set_clang_decl(const ::clang::Decl * d) { clang_decl_ = d; }


    ////////////////////////////////////////////////////////////
    // Declarations

    /// Returns reference to declarations map
    auto & decls() { return decls_; }

    /// Returns const reference to declarations map
    const auto & decls() const { return decls_; }

    /// Finds code model entity associated with clang declaration.
    /// First gets canonical declaration of clang declaration.
    /// Returns nullptr if associated context_entity not found
    context_entity * get_decl_entity(const ::clang::Decl * clang_decl) const {
        return decls_.get(clang_decl);
    }

    /// Finds code model enitty associated with clang declaration and converts it to
    /// specified type. Found context_entity must be convertible to specified type.
    /// Returns nullptr if context_entity is not found.
    template <typename Entity>
    Entity * get_decl_entity_as(const ::clang::Decl * clang_decl) const {
        return decls_.get_as<Entity>(clang_decl);
    }

    /// Adds code model context entity associated with clang declaration.
    void add_decl_entity(const ::clang::Decl * clang_decl, context_entity * cm_ent) {
        decls_.add(clang_decl, cm_ent);
    }

    /// Gets existing or creates new code model entity for clang declaration.
    /// Created declaration is empty until convert function for clang declaration is called.
    context_entity * decl_entity(const ::clang::Decl * decl);

    /// Gets existing or creates new code model entity and casts it to specified type.
    /// Existing entity must be convertable to specified type.
    template <std::derived_from<context_entity> Entity>
    Entity * decl_entity_as(const ::clang::Decl * decl) {
        auto ent = decl_entity(decl);
        assert(ent != nullptr && "decl_entity returned null");
        auto casted_ent = dynamic_cast<Entity*>(ent);
        assert(casted_ent != nullptr && "invalid entity type");
        return casted_ent;
    }

    /// Gets existing or creates new code model context for parent declaration context
    /// of specified clang declaration
    context * parent_ctx(const ::clang::Decl * decl);

    /// Converts declaration to code model entity
    void convert_decl(const ::clang::Decl * decl);


    ////////////////////////////////////////////////////////////
    // Types

    /// Gets existing or creates new code model type for clang type.
    type_t * type(const ::clang::Type * clang_type);

    /// Gets existing or creates new code model qual type for clang qual type.
    qual_type type(const ::clang::QualType & clang_type);

    /// Gets existing or creates new code model type for clang type and converts it
    /// to specified type. Type must be convertible to specified type.
    template <std::derived_from<type_t> Type>
    Type * type_as(const ::clang::Type * clang_type) {
        auto t = dynamic_cast<Type*>(type(clang_type));
        assert(t && "code model type can't be converted to specified type");
        return t;
    }

    /// Gets existing or creates new code model type for clang type and converts it
    /// to specified type. Type must be convertible to specified type.
    template <std::derived_from<type_t> Type>
    qual_type_t<Type> type_as(const ::clang::QualType & clang_type) {
        auto t = type(clang_type).cast<Type>();
        assert(!t.is_null() && "code model type can't be converted to specified type");
        return t;
    }

    /// Gets existing code model type for clang type. Type must exist in code model.
    type_t * get_type(const ::clang::Type * clang_type) const;

    /// Gets existing code model type for clang type. Type must exist in code model.
    qual_type get_type(const ::clang::QualType & clang_type) const;

    /// Gets existing code model type for clang type and converts it to specified type.
    /// Type must exist in code model and be convertible to specified type.
    template <std::derived_from<type_t> Type>
    Type * get_type_as(const ::clang::Type * clang_type) const {
        auto type = dynamic_cast<Type*>(get_type(clang_type));
        assert(type && "code model type can't be converted to specified type");
        return type;
    }

    /// Gets existing code model type for clang type and converts it to specified type.
    /// Type must exist in code model and be convertible to specified type.
    template <std::derived_from<type_t> Type>
    qual_type_t<Type> get_type_as(const ::clang::QualType & clang_type) const {
        auto type = get_type(clang_type).cast<Type>();
        assert(!type.is_null() && "code model type can't be converted to specified type");
        return type;
    }

private:
    /// Creates code model entity for specified clang declaration
    context_entity * create_decl_entity(const ::clang::Decl * decl);

    /// Creates entity using converters tuple starting from specified index
    template <size_t I>
    context_entity * create_decl_entity_impl(const ::clang::Decl * decl);

    /// Converts entity using converters tuple starting from specified index
    template <size_t I>
    void convert_decl_impl(const ::clang::Decl * decl);

    /// Returns clang declaration corresponding to clang type. Returns nullptr if type
    /// does not have declaration.
    static const ::clang::TypeDecl * get_type_decl(const ::clang::Type * type);


    code_model & mdl_;                                      ///< Reference to code model
    const ::clang::ASTContext & clang_ctx_;                 ///< Reference to Clang AST context
    context * decl_ctx_ = nullptr;                          ///< Current code model context
    const ::clang::Decl * clang_decl_ = nullptr;            ///< Current clang declaration
    decl_map decls_;                                        ///< Declarations map
    std::map<const ::clang::Type *, type_t *> types_;       ///< Types map
    decl_converters decl_convs_;                            ///< Tuple of decl converters
    std::unique_ptr<types_converter> types_conv_;           ///< Types converter
};


}
