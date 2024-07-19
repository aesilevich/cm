// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file ast_converter.cpp
/// Contains implementation of the ast_converter class.

#include "pch.hpp"
#include "cm/src/cxx/clang/ast_converter.hpp"
#include "cm/namespace.hpp"
#include "cm/record_kind.hpp"
#include "cm/record_type.hpp"
#include "cm/template_instantiation.hpp"
#include <clang/Basic/SourceManager.h>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclBase.h>
#include <clang/AST/Type.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/DeclTemplate.h>
#include <clang/Lex/Lexer.h>
#include <ranges>


namespace cm::src::clang {


void ast_converter::convert(::clang::ASTContext & ctx) {
    clang_ast_ctx_ = &ctx;

    // auto tu_decl = ctx.getTranslationUnitDecl();
    // tu_decl->dump();

    // setting source code model as current context
    auto & src_mngr = ctx.getSourceManager();
    source_file * src = nullptr;
    if (auto main_entry = src_mngr.getFileEntryRefForID(src_mngr.getMainFileID())) {
        src = scm_.get_or_create_source(main_entry->getName().str());
    } else {
        src = scm_.get_or_create_source("<unknown>");
    }

    src->create_decl_ctx();
    ctx_ = src->decl_ctx();

    // setting translation unit as current clang context
    clang_ctx_ = ctx.getTranslationUnitDecl();

    // converting AST to code model
    cm_conv_.convert(ctx);

    // setting source code model as current declaration context
    //ctx_ = &scm_.f;

    // processing source code
    this->TraverseAST(ctx);

    // // traversing over all top level declarations in translation unit
    // auto tu_decl = ctx_.getTranslationUnitDecl();

    // //std::wcerr << "BEGIN DUMP\n";
    // for (auto && decl : tu_decl->decls()) {
    //     decl->dump();
    // }
    // //std::wcerr << "END DUMP\n";

    // for (auto && decl : tu_decl->decls()) {
    //     // converting namespaces separately from other declarations
    //     if (auto ns = ::clang::dyn_cast<::clang::NamespaceDecl>(decl)) {
    //         convert_ns(&scm_, ns);
    //     } else {
    //         convert_decl(&scm_, decl);
    //     }
    // }
}


bool ast_converter::TraverseNamespaceDecl(::clang::NamespaceDecl * clang_decl) {
    // getting namespace code model entity
    auto ns = cm_conv_.get_cm_entity_as<namespace_>(clang_decl);
    assert(ns && "can't find namespace code model entity");

    // creating namespace declaration
    auto decl = ctx_->create_decl<namespace_decl>();
    decl->set_entity(ns);
    decl->create_decl_ctx();

    // converting source location
    decl->set_source_range(convert_source_range(clang_decl->getSourceRange()));

    // converting namespace name
    decl->set_name(convert_name(decl, clang_decl));

    // setting namespace as current declaration context
    context_setter csetter{*this, decl->decl_ctx(), clang_decl};

    return base_t::TraverseNamespaceDecl(clang_decl);
}


bool ast_converter::TraverseCXXRecordDecl(::clang::CXXRecordDecl * clang_decl) {
    // converting record declaration
    auto rec_decl = convert_record_decl(ctx_, clang_decl);

    // adding record to current decl context
    ctx_->add_decl(std::move(rec_decl));

    return true;
}


bool ast_converter::TraverseClassTemplateDecl(::clang::ClassTemplateDecl * clang_decl) {
    // getting code model template record from code model converter
    auto templ = cm_conv_.get_cm_entity_as<template_record>(clang_decl->getTemplatedDecl());
    assert(templ && "can't find code model record for clang declaration");

    // creating new template declaration
    auto templ_decl = ctx_->create_decl<template_decl>();
    templ_decl->set_source_range(convert_source_range(clang_decl->getSourceRange()));

    // converting template parameters
    convert_template_params(templ_decl, templ, *clang_decl->getTemplateParameters());

    // converting record declaration
    auto rec_decl = convert_record_decl(templ_decl, clang_decl->getTemplatedDecl());

    // setting record as templated declaration in template
    templ_decl->set_templated_decl(std::move(rec_decl));

    return true;
}


bool ast_converter::TraverseTypedefDecl(::clang::TypedefDecl * clang_decl) {
    auto td_type = cm_conv_.get_cm_entity_as<typedef_type>(clang_decl);
    assert(td_type != nullptr && "can't find code model type for typedef declaration");

    auto td_decl = ctx_->create_decl<typedef_decl>();
    td_decl->set_entity(td_type);
    td_decl->set_source_range(convert_source_range(clang_decl->getSourceRange()));
    td_decl->set_name(td_type->name(), convert_source_range(clang_decl->getLocation(),
                                                            td_type->name().size()));

    // adjusting end of range of typedef declaration
    if (td_decl->source_range().range().end() < td_decl->name()->source_range().range().end()) {
        td_decl->set_source_range_end(td_decl->name()->source_range().range().end());
    }

    auto src_info = clang_decl->getTypeSourceInfo();
    assert(src_info != nullptr && "invalid source info for clang typedef declaration");
    td_decl->set_type(convert_type_spec(td_decl, src_info->getTypeLoc()));

    return true;
}


bool ast_converter::TraverseFunctionTemplateDecl(::clang::FunctionTemplateDecl * clang_decl) {
    auto clang_func_decl =
        ::clang::dyn_cast<::clang::FunctionDecl>(clang_decl->getTemplatedDecl());

    // getting code model entity for clang declaration
    auto func = cm_conv_.get_cm_entity_as<template_function>(clang_func_decl);
    assert(func && "can't find code model entity for method delcaration");

    // creating template declaration
    auto templ_decl = ctx_->create_decl<template_decl>();
    templ_decl->set_source_range(convert_source_range(clang_decl->getSourceRange()));
    convert_template_params(templ_decl, func, *clang_decl->getTemplateParameters());

    // creating function declaration
    auto func_decl = std::make_unique<function_decl>(templ_decl);
    func_decl->set_entity(func);

    // converting functional declaration
    convert_functional_decl(func, func_decl.get(), clang_func_decl);

    templ_decl->set_templated_decl(std::move(func_decl));

    return true;
}


bool ast_converter::TraverseClassTemplateSpecilizationDecl(
        ::clang::ClassTemplatePartialSpecializationDecl * decl) {

    // skipping implicit instantiations
    if (decl->isImplicit()) {
        return true;
    }

    assert(false && "template specialization parsing is not yet implemented");
}


bool ast_converter::TraverseFunctionDecl(::clang::FunctionDecl * clang_decl) {
    // getting code model entity for clang declaration
    auto func = cm_conv_.get_cm_entity_as<function>(clang_decl);
    assert(func && "can't find code model entity for method delcaration");

    // creating method declaration
    auto decl = ctx_->create_decl<function_decl>();
    decl->set_entity(func);

    // converting functional declaration
    convert_functional_decl(func, decl, clang_decl);

    return true;
}


bool ast_converter::TraverseCXXMethodDecl(::clang::CXXMethodDecl * clang_decl) {
    std::unique_ptr<template_decl> templ_decl;
    if (auto lists_cnt = clang_decl->getNumTemplateParameterLists(); lists_cnt > 0) {
        // this is template method declaration or template record method delcaration
        templ_decl = convert_template_lists(clang_decl);
    }

    std::unique_ptr<declaration> decl;

    auto parent = templ_decl ?
                  static_cast<ast_node*>(templ_decl.get()) :
                  static_cast<ast_node*>(ctx_);

    if (clang_decl->isStatic()) {
        // getting code model entity for clang declaration
        auto m = cm_conv_.get_cm_entity_as<function>(clang_decl);
        assert(m && "can't find code model entity for method delcaration");

        // creating static method declaration
        auto mdecl = std::make_unique<static_method_decl>(parent);
        mdecl->set_entity(m);

        // converting functional declaration
        convert_functional_decl(m, mdecl.get(), clang_decl);

        decl = std::move(mdecl);
    } else {
        // getting code model entity for clang declaration
        auto m = cm_conv_.get_cm_entity_as<method>(clang_decl);
        assert(m && "can't find code model entity for method delcaration");

        // creating method declaration
        auto mdecl = std::make_unique<method_decl>(parent);
        mdecl->set_entity(m);

        // converting functional declaration
        convert_functional_decl(m, mdecl.get(), clang_decl);
        decl = std::move(mdecl);
    }

    assert(decl && "invalid method declaration");

    if (templ_decl) {
        // setting templated declaration in the last template in template chain
        auto last_templ_decl = templ_decl.get();
        while (last_templ_decl->templated_decl() != nullptr) {
            last_templ_decl = dynamic_cast<template_decl*>(last_templ_decl->templated_decl());
            assert(last_templ_decl != nullptr && "declaration is not a template declaration");
        }

        last_templ_decl->set_templated_decl(std::move(decl));
        decl = std::move(templ_decl);
    }

    // adding declaration into current context
    ctx_->add_decl(std::move(decl));

    return true;
}


bool ast_converter::TraverseVarDecl(::clang::VarDecl * clang_var_decl) {
    // skipping local variables
    if (clang_var_decl->hasLocalStorage() || clang_var_decl->isStaticLocal()) {
        return base_t::TraverseVarDecl(clang_var_decl);
    }

    auto var = cm_conv_.get_cm_entity_as<variable>(clang_var_decl);
    assert(var && "can't find code model variable");

    // creating new variable declaration
    auto var_decl = ctx_->create_decl<global_variable_decl>();
    var_decl->set_entity(var);
    var_decl->set_source_range(convert_source_range(clang_var_decl->getSourceRange()));

    // converting variable type
    auto type_spec = convert_type_spec(var_decl, clang_var_decl->getTypeSourceInfo()->getTypeLoc());
    var_decl->set_type(std::move(type_spec));

    // converting variable name
    auto nm = clang_var_decl->getName();
    var_decl->set_name(nm.str(), convert_source_range(clang_var_decl->getLocation(), nm.size()));

    return true;
}


bool ast_converter::TraverseFieldDecl(::clang::FieldDecl * clang_field_decl) {
    auto fld = cm_conv_.get_cm_entity_as<field>(clang_field_decl);
    assert(fld && "can't find code model field");

    // creating new variable declaration
    auto fld_decl = ctx_->create_decl<field_decl>();
    fld_decl->set_entity(fld);
    fld_decl->set_source_range(convert_source_range(clang_field_decl->getSourceRange()));

    // converting variable type
    auto type_spec = convert_type_spec(fld_decl,
                                       clang_field_decl->getTypeSourceInfo()->getTypeLoc());
    fld_decl->set_type(std::move(type_spec));

    // converting variable name
    auto nm = clang_field_decl->getName();
    fld_decl->set_name(nm.str(), convert_source_range(clang_field_decl->getLocation(), nm.size()));

    return true;
}


void debug_break() {
}

bool ast_converter::TraverseTypeLoc(::clang::TypeLoc loc) {
    assert(func_body_ != nullptr && "can't convert type loc without function body");
    func_body_->add_child(convert_type_spec(ctx_, loc));
    return true;
}


bool ast_converter::VisitNestedNameSpecifierLoc(::clang::NestedNameSpecifierLoc loc) {
    std::cout << "NNS LOC:" << loc.getSourceRange().printToString(clang_ast_ctx_->getSourceManager())
              << std::endl;
    return true;
}


std::unique_ptr<template_decl>
ast_converter::convert_template_lists(const ::clang::DeclaratorDecl * clang_decl) {
    auto num_lists = clang_decl->getNumTemplateParameterLists();
    assert(num_lists > 0 && "declaration doses not have template parameters");

    // getting code model entity for clang declaration
    auto ent = cm_conv_.get_cm_entity_as<context_entity>(clang_decl);
    assert(ent != nullptr && "can't find code model entity for declaration");

    std::unique_ptr<template_decl> decl;

    // building chain of template declarations from back to front
    unsigned int list_idx = num_lists - 1;
    while (true) {
        // looking for the first template entity in chain parent contexts
        template_ * templ = nullptr;
        while (ent != nullptr) {
            templ = dynamic_cast<template_*>(ent);
            if (templ != nullptr) {
                break;
            }

            ent = ent->ctx();
        }

        assert(templ != nullptr && "can't find template entity in chain of parent contexts");

        // creating template declaration
        auto parent = decl ? static_cast<ast_node*>(decl.get()) : static_cast<ast_node*>(ctx_);
        auto new_decl = std::make_unique<template_decl>(parent);
        new_decl->set_templated_decl(std::move(decl));
        decl = std::move(new_decl);

        // converting template parameters
        auto list = clang_decl->getTemplateParameterList(list_idx);
        assert(list != nullptr && "invalid template parameter list");
        convert_template_params(decl.get(), templ, *list);

        // checking for the end of list
        if (list_idx == 0) {
            break;
        }

        --list_idx;
    }

    return decl;
}


void ast_converter::convert_template_params(template_decl * decl,
                                            template_ * templ,
                                            const ::clang::TemplateParameterList & clang_pars) {
    // auto templ = decl->entity();
    // assert(templ != nullptr && "template declaration does not have assoicated cm entity");

    auto templ_params = templ->template_params();
    auto par_it = std::ranges::begin(templ_params);

    for (auto && clang_par : clang_pars) {
        assert(par_it != std::ranges::end(templ_params) && "template parameters inconsistency");
        auto par = *par_it;
        template_parameter_decl * par_decl = nullptr;

        if (auto clang_type_par = ::clang::dyn_cast<::clang::TemplateTypeParmDecl>(clang_par)) {
            auto t_par = dynamic_cast<type_template_parameter*>(par);
            //auto par = cm_conv_.get_cm_entity_as<type_template_parameter>(clang_par);
            assert(t_par != nullptr && "template parameters inconsistency");
            par_decl = decl->add_type_param(t_par);

            // we have to manually add association between clang parameter declaration and
            // code model entity because this declaration not always processed by CM converter
            if (!cm_conv_.get_cm_entity(clang_type_par)) {
                cm_conv_.add_cm_entity(clang_type_par, t_par);
            }

        } else if (auto val_par = ::clang::dyn_cast<::clang::NonTypeTemplateParmDecl>(clang_par)) {
            //auto par = cm_conv_.get_cm_entity_as<value_template_parameter>(clang_par);
            //assert(par != nullptr && "can't get code model entity for value template parameter");

            auto v_par = dynamic_cast<value_template_parameter*>(par);
            assert(v_par != nullptr && "template parameters inconsistency");

            auto val_par_decl = decl->add_value_param(v_par);
            val_par_decl->set_type(convert_type_spec(par_decl,
                                                     val_par->getTypeSourceInfo()->getTypeLoc()));
            par_decl = val_par_decl;
        } else if (auto tpar = ::clang::dyn_cast<::clang::TemplateTemplateParmDecl>(clang_par)) {
            // TODO: implement template template parameters
            assert(false && "NYI");
        
        // C++20 support in latest clang
        // } else if (auto clang_obj_par = ::clang::dyn_cast<::clang::TemplateParamObjectDecl>(par)) {
        //     // TODO: check if we need something special for template object parameters other
        //     // than name and type
        //     auto par_type = convert_type(clang_val_par->getType());
        //     rec->add_value_template_param(par_name, par_type.type());

        } else {
            assert(false && "Unknown template parameter type");
        }

        // converting parameter name
        if (auto name = clang_par->getName(); !name.empty()) {
            par_decl->set_name(name, convert_source_range(clang_par->getLocation(), name.size()));
        }

        // converting parameter source range
        par_decl->set_source_range(convert_source_range(clang_par->getSourceRange()));

        // adjusting template parameter end
        auto name_end = par_decl->name()->source_range().range().end();
        if (name_end > par_decl->source_range().range().end()) {
            par_decl->set_source_range_end(name_end);
        }

        ++par_it;
    }
}


std::unique_ptr<record_decl>
ast_converter::convert_record_decl(ast_node * parent, ::clang::CXXRecordDecl * clang_decl) {
    // getting code model record from code model converter
    auto cm_rec = cm_conv_.get_cm_entity_as<record>(clang_decl);
    assert(cm_rec && "can't find code model record for clang declaration");

    // creating new record declaration
    auto rec_decl = std::make_unique<record_decl>(parent);
    rec_decl->set_entity(cm_rec);
    rec_decl->set_source_range(convert_source_range(clang_decl->getSourceRange()));

    // setting record name
    if (auto nm = clang_decl->getName(); !nm.empty()) {
        auto src_range = convert_source_range(clang_decl->getLocation(),
                                              static_cast<unsigned int>(nm.size()));
        rec_decl->set_name(nm.str(), src_range);
    }

    // converting record scope
    if (auto nns = clang_decl->getQualifier()) {
        auto nns_loc = clang_decl->getQualifierLoc();
        assert(nns_loc && "can't get nested named specifier location");
        rec_decl->set_scope(convert_scope_spec(rec_decl.get(), nns, nns_loc));
    }

    // traversing record contents
    if (clang_decl->isCompleteDefinition()) {
        // creating declaration context for record declaration
        rec_decl->create_decl_ctx();

        // setting record declaration as current context
        context_setter csetter{*this, rec_decl->decl_ctx(), clang_decl};

        // traversing record members
        base_t::TraverseCXXRecordDecl(clang_decl);
    }

    return rec_decl;
}


std::unique_ptr<identifier> ast_converter::convert_name(ast_node * parent,
                                                        const ::clang::NamedDecl * clang_decl) {
    auto name = clang_decl->getName();
    if (name.empty()) {
        return {};
    }

    auto ident = std::make_unique<identifier>(parent);
    ident->set_string(name);
    ident->set_source_range(convert_source_range(clang_decl->getLocation(), name.size()));

    return ident;
}


void ast_converter::convert_named_decl(named_decl * decl,
                                       const ::clang::DeclaratorDecl * clang_decl) {
    // converting declaration name
    decl->set_name(convert_name(decl, clang_decl));

    // converting declaration scope
    if (auto qual = clang_decl->getQualifier()) {
        auto scope = convert_scope_spec(decl, qual, clang_decl->getQualifierLoc());
        decl->set_scope(std::move(scope));
    }
}


void ast_converter::convert_functional_decl(function * func,
                                            functional_decl * decl,
                                            ::clang::FunctionDecl * clang_decl) {
    // converting name and scope
    convert_named_decl(decl, clang_decl);

    // converting source range
    decl->set_source_range(convert_source_range(clang_decl->getSourceRange()));

    auto floc = clang_decl->getFunctionTypeLoc();

    // converting return type
    decl->set_ret_type(convert_type_spec(decl, floc.getReturnLoc()));

    // converting parameters
    auto params = func->params();
    auto par_it = std::ranges::begin(params);
    for (auto && clang_par : clang_decl->parameters()) {
        assert(par_it != std::ranges::end(params) && "invalid parameter iterator");
        decl->add_param(convert_function_parameter(decl, *par_it, clang_par));

        ++par_it;
    }

    assert(par_it == std::ranges::end(params) && "parameter count inconsistency");

    if (!clang_decl->doesThisDeclarationHaveABody()) {
        return;
    }

    // converting function body
    decl->create_body();
    func_body_ = decl->body();
    TraverseStmt(clang_decl->getBody());
    func_body_ = nullptr;
}


std::unique_ptr<scope_spec>
ast_converter::convert_scope_spec(ast_node * parent,
                                  const ::clang::NestedNameSpecifier * nns,
                                  const ::clang::NestedNameSpecifierLoc & nns_loc) {

    if (auto nns2 = nns_loc.getNestedNameSpecifier()) {
        std::cout << std::endl;
    }

    // TODO:
    // nns_loc.getPrefix() returns location info for scope of the nns_loc scope!!!

    if (auto ident = nns->getAsIdentifier()) {
        std::cout << "IDENTIFIER: " << ident->getName().str() << std::endl;
        assert(false && "don't know how to convert identitifers");
        return {};
    } else if (auto ns = nns->getAsNamespace()) {
        auto cm_ns = cm_conv_.get_cm_entity_as<namespace_>(ns);
        auto res = std::make_unique<namespace_scope_spec>(parent);
        res->set_entity(cm_ns);
        res->set_source_range(convert_source_range(nns_loc.getSourceRange()));
        res->set_name(ns->getName().str(), convert_source_range(nns_loc.getLocalSourceRange()));
        return res;
    } else if (auto ns_alias = nns->getAsNamespaceAlias()) {
        // TODO: add namespace aliases support in code model
        assert(false && "NYI");
        return {};
    } else if (auto type = nns->getAsType()) {
        return convert_type_scope(parent, nns_loc.getTypeLoc());
    } else {
        assert(false && "unknown nested name specified kind");
        return {};
    }
}


std::unique_ptr<scope_spec>
ast_converter::convert_type_scope(ast_node * parent, const ::clang::TypeLoc & loc) {

    if (auto bt_loc = loc.getAs<::clang::BuiltinTypeLoc>()) {
        assert(false && "invalid scope type spec");
        return {};
    } else if (auto td_loc = loc.getAs<::clang::TypedefTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto icls_loc = loc.getAs<::clang::InjectedClassNameTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto using_loc = loc.getAs<::clang::UnresolvedUsingTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto tag_loc = loc.getAs<::clang::TagTypeLoc>()) {
        return convert_tag_scope_spec(parent, tag_loc);
    } else if (auto tpar_loc = loc.getAs<::clang::TemplateTypeParmTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto macro_loc = loc.getAs<::clang::MacroQualifiedTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto p_loc = loc.getAs<::clang::ParenTypeLoc>()) {
        assert(false && "invalid scope type spec");
        return {};
    } else if (auto adj_loc = loc.getAs<::clang::AdjustedTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto dec_loc = loc.getAs<::clang::DecayedTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto ptr_loc = loc.getAs<::clang::PointerTypeLoc>()) {
        assert(false && "invalid scope type spec");
        return {};
    } else if (auto ref_loc = loc.getAs<::clang::LValueReferenceTypeLoc>()) {
        assert(false && "invalid scope type spec");
        return {};
    } else if (auto ref_loc = loc.getAs<::clang::RValueReferenceTypeLoc>()) {
        assert(false && "invalid scope type spec");
        return {};
    } else if (auto func_loc = loc.getAs<::clang::FunctionTypeLoc>()) {
        assert(false && "invalid scope type spec");
        return {};
    } else if (auto mem_ptr_loc = loc.getAs<::clang::MemberPointerTypeLoc>()) {
        assert(false && "invalid scope type spec");
        return {};
    } else if (auto func_loc = loc.getAs<::clang::FunctionNoProtoTypeLoc>()) {
        assert(false && "invalid scope type spec");
        return {};
    } else if (auto arr_loc = loc.getAs<::clang::ArrayTypeLoc>()) {
        assert(false && "invalid scope type spec");
        return {};
    } else if (auto vec_loc = loc.getAs<::clang::VectorTypeLoc>()) {
        assert(false && "invalid scope type spec");
        return {};
    } else if (auto m_loc = loc.getAs<::clang::MatrixTypeLoc>()) {
        assert(false && "invalid scope type spec");
        return {};
    } else if (auto compl_loc = loc.getAs<::clang::ComplexTypeLoc>()) {
        assert(false && "invalid scope type spec");
        return {};
    } else if (auto tspec_loc = loc.getAs<::clang::TemplateSpecializationTypeLoc>()) {
        return convert_template_scope_spec(parent, tspec_loc);
    } else if (auto dep_space_loc = loc.getAs<::clang::DependentAddressSpaceTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto tof_expr_loc = loc.getAs<::clang::TypeOfExprTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto tof_loc = loc.getAs<::clang::TypeOfTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto dtype_loc = loc.getAs<::clang::DecltypeTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto ut_loc = loc.getAs<::clang::UnaryTransformTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto dt_loc = loc.getAs<::clang::DeducedTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto a_loc = loc.getAs<::clang::AutoTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto t_loc = loc.getAs<::clang::DeducedTemplateSpecializationTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto e_loc = loc.getAs<::clang::ElaboratedTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto dn_loc = loc.getAs<::clang::DependentNameTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto dspec_loc = loc.getAs<::clang::DependentTemplateSpecializationTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto pack_loc = loc.getAs<::clang::PackExpansionTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto at_loc = loc.getAs<::clang::AtomicTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto p_loc = loc.getAs<::clang::PipeTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (loc.getAs<::clang::ObjCTypeParamTypeLoc>() ||
               loc.getAs<::clang::ObjCObjectTypeLoc>() ||
               loc.getAs<::clang::ObjCInterfaceTypeLoc>() ||
               loc.getAs<::clang::BlockPointerTypeLoc>() ||
               loc.getAs<::clang::ObjCObjectPointerTypeLoc>()) {
        assert (false && "Objective-C support is not implemented");
        return {};
    } else {
        assert(false && "unknown type location kind");
        return {};
    }

    // // converting clang type to code model type
    // auto type = cm_conv_.convert_type(loc.getType());



    // if (auto td_type = type->cast<typedef_type>()) {
    //     // creating typedef scope specifier
    //     auto spec = std::make_unique<typedef_scope_spec>(parent);
    //     spec->set_entity(td_type);

    //     // // converting parent scope
    //     // auto td_loc = loc.castAs<::clang::TypedefTypeLoc>();

    //     // td_loc.
    //     return spec;

    // } else if (auto rec_type = type->cast<named_record_type>()) {
    //     std::cout << "LOC KIND FOR RECORD: " << loc.getTypeLocClass() << std::endl;
    //     auto rec_loc = loc.castAs<::clang::RecordTypeLoc>();
    //     assert(!rec_loc.isNull() && "invalid type location for record");

    //     auto scope = std::make_unique<record_scope_spec>(parent);
    //     scope->set_entity(rec_type);
    //     scope->set_name(rec_type->name(),
    //                     convert_source_range(rec_loc.getNameLoc(), rec_type->name().size()));

    //     return scope;

    //     // auto qual_loc = rec_loc.findExplicitQualifierLoc();
    //     // if (qual_loc) {
    //     //     std::cout << "HAS QUAL" << std::endl;
    //     // } else {
    //     //     std::cout << "DOES NOT HAVE QUAL" << std::endl;
    //     // }

    //     // auto next_loc = rec_loc.getNextTypeLoc();
    //     // if (next_loc) {
    //     //     std::cout << "HAS NEXT" << std::endl;
    //     // } else {
    //     //     std::cout << "DOES NOT HAVE NEXT" << std::endl;
    //     // }
    // } else if (auto rec_type = type->cast<template_record_substitution>()) {
    // } else if (auto t_type = type->cast<template_record_type>()) {
    //     // TODO: add parent qualifier 
    //     auto res = std::make_unique<template_record_scope_spec>(parent);
    //     res->set_entity(t_type);
    //     return res;
    // } else if (auto dep_type = type->cast<template_record_dependent_instantiation_type>()) {
    // } else {
    //     assert(false && "don't know how to convert type scope qualifier");
    //     return {};
    // }

    // if (auto clang_bt_type = ::clang::dyn_cast<::clang::BuiltinType>(type)) {
    //     type = convert_builtin_type(clang_bt_type);
    // } else if (auto clang_ptr_type = ::clang::dyn_cast<::clang::PointerType>(type)) {
    //     type = convert_pointer_type(clang_ptr_type);
    // } else if (auto clang_ref_type = ::clang::dyn_cast<::clang::LValueReferenceType>(type)) {
    //     type = convert_lvalue_reference_type(clang_ref_type);
    // } else if (auto clang_ref_type = ::clang::dyn_cast<::clang::RValueReferenceType>(type)) {
    //     type = convert_rvalue_reference_type(clang_ref_type);
    // } else if (auto clang_func_type = ::clang::dyn_cast<::clang::FunctionType>(type)) {
    //     type = convert_function_type(clang_func_type);
    // } else if (auto clang_rec_type = ::clang::dyn_cast<::clang::RecordType>(type)) {
    //     type = convert_record_type(clang_rec_type);
    // } else if (auto clang_arr_type = ::clang::dyn_cast<::clang::ConstantArrayType>(type)) {
    //     type = convert_array_type(clang_arr_type);
    // } else if (auto clang_elab_type = ::clang::dyn_cast<::clang::ElaboratedType>(type)) {
    //     type = convert_type(clang_elab_type->getNamedType()).type();
    // } else if (auto clang_td_type = ::clang::dyn_cast<::clang::TypedefType>(type)) {
    //     // typedef type must already exist in code model, getting it
    //     auto typedef_decl = clang_td_type->getDecl();
    //     type = get_cm_entity_as<typedef_type>(typedef_decl);
    //     assert(type && "typedef type must already exist in code model");
    // } else if (auto clang_tpar_type = ::clang::dyn_cast<::clang::TemplateTypeParmType>(type)) {
    //     type = convert_type_template_param_type(clang_tpar_type);
    // } else if (auto clang_templ_spec = ::clang::dyn_cast<::clang::TemplateSpecializationType>(type)) {
    //     type = convert_template_spec_type(clang_templ_spec);
    // } else if (auto clang_subst_tpar = ::clang::dyn_cast<::clang::SubstTemplateTypeParmType>(type)) {
    //     // TODO: do we need store this info in code model?
    //     type = convert_type(clang_subst_tpar->getReplacementType()).type();
    // } else if (auto dep_type = ::clang::dyn_cast<::clang::DependentNameType>(type)) {
    //     type = convert_dependent_type(dep_type);
    // } else if (auto dt_type = ::clang::dyn_cast<::clang::DecltypeType>(type)) {
    //     type = convert_decltype_type(dt_type);
    // } else {
    //     std::cerr << "DON'T NOT KNOW HOW TO CONVERT TYPE:\n";
    //     clang_type->dump();
    //     // TODO: diagnostics
    //     assert(false && "Don't not know how to convert type");
    // }


    return {};
}


std::unique_ptr<scope_spec>
ast_converter::convert_tag_scope_spec(ast_node * parent, const ::clang::TagTypeLoc & loc) {
    // getting code model entity associated with record declaration
    auto clang_decl = loc.getDecl();
    auto rec = cm_conv_.get_cm_entity_as<named_record_type>(clang_decl);
    assert(rec != nullptr && "can't find entity for tag decl scope");

    // creating record scope specifier
    auto spec = std::make_unique<record_scope_spec>(parent);
    spec->set_entity(rec);
    auto range = convert_source_range(loc.getSourceRange().getBegin(), rec->name().size());
    spec->set_source_range(range);
    spec->set_name(rec->name(), range);

    return spec;
}


std::unique_ptr<scope_spec>
ast_converter::convert_template_scope_spec(ast_node * parent,
                                           const ::clang::TemplateSpecializationTypeLoc & loc) {

    // getting code model entity associated with template specialization
    auto clang_decl = loc.getType()->getAsCXXRecordDecl();
    auto ent = cm_conv_.get_cm_entity_as<context_entity>(clang_decl);
    assert(ent != nullptr && "can't get entity for template record");

    // TODO: implement support of injected class records

    // if (auto trec_type = dynamic_cast<template_record_type*>(ent)) {
    //     assert(false && "XXX");
    //     // this template specialization refers to a tempalte record type inside template
    //     // (injected record type)

    //     // creating template scope specifier
    //     auto spec = std::make_unique<template_record_scope_spec>(parent);
    //     spec->set_entity(trec_type);
    //     spec->set_source_range(convert_source_range(loc.getSourceRange()));

    //     // converting template name and arguments
    //     convert_template_and_arguments(loc, spec.get(), trec_type->templ());

    //     return spec;
    // }

    if (auto inst = dynamic_cast<template_record_instantiation_type*>(ent)) {
        // creating template instantiation scope specifier
        auto spec = std::make_unique<template_record_instantiation_scope_spec>(parent);
        spec->set_entity(inst);
        spec->set_source_range(convert_source_range(loc.getSourceRange()));

        // converting template name and arguments
        convert_template_and_arguments(loc, spec.get(), inst->templ());

        return spec;

    } else if (auto trec = dynamic_cast<template_record*>(ent)) {
        // this template scope refers to template itself

        // creating template scope specifier
        auto spec = std::make_unique<template_record_scope_spec>(parent);
        spec->set_entity(trec->this_type());
        spec->set_source_range(convert_source_range(loc.getSourceRange()));

        // converting template name and arguments
        convert_template_and_arguments(loc, spec.get(), trec);

        return spec;
    } else {
        assert(false && "unknown template specialization kind");
    }
}


std::unique_ptr<builtin_type_spec>
ast_converter::convert_builtin_type_spec(ast_node * parent, const ::clang::BuiltinTypeLoc & loc) {
    auto clang_bt_type = ::clang::dyn_cast<::clang::BuiltinType>(loc.getTypePtr());
    assert(clang_bt_type && "invalid clang type for builtin type loc");

    auto cm_type = cm_conv_.convert_builtin_type(clang_bt_type);
    auto cm_bt_type = dynamic_cast<builtin_type*>(cm_type);
    assert(cm_bt_type && "invalid builtin cm type");
    auto res = std::make_unique<builtin_type_spec>(parent);
    res->set_entity(cm_bt_type);
    res->set_source_range(convert_source_range(loc.getSourceRange()));
    return res;
}


std::unique_ptr<pointer_type_spec>
ast_converter::convert_pointer_type_spec(ast_node * parent, const ::clang::PointerTypeLoc & loc) {
    auto clang_type = ::clang::dyn_cast<::clang::PointerType>(loc.getTypePtr());
    assert(clang_type && "invalid clang type for pointer type loc");

    auto type = cm_conv_.convert_pointer_type(clang_type);
    auto res = std::make_unique<pointer_type_spec>(parent);
    res->set_entity(type);
    res->set_source_range(convert_source_range(loc.getSourceRange()));
    res->set_base(convert_type_spec(res.get(), loc.getNextTypeLoc()));

    return res;
}


std::unique_ptr<lvalue_reference_type_spec>
ast_converter::convert_lvalue_reference_type_spec(ast_node * parent,
                                                  const ::clang::LValueReferenceTypeLoc & loc) {
    auto clang_type = ::clang::dyn_cast<::clang::LValueReferenceType>(loc.getTypePtr());
    assert(clang_type && "invalid clang type for reference type loc");

    auto type = cm_conv_.convert_lvalue_reference_type(clang_type);
    auto res = std::make_unique<lvalue_reference_type_spec>(parent);
    res->set_entity(type);
    res->set_source_range(convert_source_range(loc.getSourceRange()));
    res->set_base(convert_type_spec(res.get(), loc.getNextTypeLoc()));

    return res;
}


std::unique_ptr<rvalue_reference_type_spec>
ast_converter::convert_rvalue_reference_type_spec(ast_node * parent,
                                                  const ::clang::RValueReferenceTypeLoc & loc) {
    auto clang_type = ::clang::dyn_cast<::clang::RValueReferenceType>(loc.getTypePtr());
    assert(clang_type && "invalid clang type for reference type loc");

    auto type = cm_conv_.convert_rvalue_reference_type(clang_type);
    auto res = std::make_unique<rvalue_reference_type_spec>(parent);
    res->set_entity(type);
    res->set_source_range(convert_source_range(loc.getSourceRange()));
    res->set_base(convert_type_spec(res.get(), loc.getNextTypeLoc()));

    return res;
}


std::unique_ptr<parens_type_spec>
ast_converter::convert_parens_type_spec(ast_node * parent, const ::clang::ParenTypeLoc & loc) {
    auto res = std::make_unique<parens_type_spec>(parent);
    res->set_source_range(convert_source_range(loc.getSourceRange()));
    res->set_base(convert_type_spec(res.get(), loc.getNextTypeLoc()));

    return res;
}


std::unique_ptr<function_type_spec>
ast_converter::convert_function_type_spec(ast_node * parent, const ::clang::FunctionTypeLoc & loc) {
    auto clang_type = ::clang::dyn_cast<::clang::FunctionType>(loc.getTypePtr());
    assert(clang_type && "invalid clang type for function type loc");

    // creating function type specifier
    auto type = cm_conv_.convert_function_type(clang_type);
    auto type_spec = std::make_unique<function_type_spec>(parent, type);
    type_spec->set_source_range(convert_source_range(loc.getSourceRange()));

    // converting return type
    type_spec->set_ret_type(convert_type_spec(type_spec.get(), loc.getReturnLoc()));

    // converting parameters
    for (auto & clang_par : loc.getParams()) {
        // creating new parameter
        auto par_type = cm_conv_.convert_type(clang_par->getType());
        auto par_spec = std::make_unique<function_type_spec_parameter>(type_spec.get());
        par_spec->set_entity(par_type.type());

        // converting parameter name
        auto nm = clang_par->getName();
        par_spec->set_name(nm.str(), convert_source_range(clang_par->getLocation(), nm.size()));

        // converting parameter type
        auto par_type_spec = convert_type_spec(par_spec.get(),
                                               clang_par->getTypeSourceInfo()->getTypeLoc());
        par_spec->set_type(std::move(par_type_spec));

        type_spec->add_param(std::move(par_spec));
    }

    return type_spec;
}


std::unique_ptr<template_substitution_type_spec>
ast_converter::convert_template_subst_type_spec(ast_node * parent,
                                                const ::clang::TemplateSpecializationTypeLoc & loc) {
    // getting template substitution entity from code model
    auto clang_decl = loc.getType()->getAsCXXRecordDecl();
    auto ent = cm_conv_.get_cm_entity_as<context_entity>(clang_decl);

    if (auto inst = dynamic_cast<template_record_instantiation_type*>(ent)) {
        // template record instantiation

        // creating template instantiation specifier
        auto spec = std::make_unique<template_record_instantiation_type_spec>(parent);
        spec->set_entity(inst);
        spec->set_source_range(convert_source_range(loc.getSourceRange()));

        // converting template name and arguments for specifier
        convert_template_and_arguments(loc, spec.get(), inst->templ());

        return spec;
    } else if (auto rec = dynamic_cast<template_record*>(ent)) {
        // template record type itself inside template definition

        // creating template instantiation specifier
        auto spec = std::make_unique<template_record_type_spec>(parent);
        spec->set_entity(rec->this_type());
        spec->set_source_range(convert_source_range(loc.getSourceRange()));

        // converting template name and arguments for specifier
        convert_template_and_arguments(loc, spec.get(), rec);

        return spec;
    } else {
        std::cerr << "ENT: ";
        ent->print_desc(std::cerr);
        std::cerr << std::endl;
        assert(false && "unknown template specialization type entity");
        return {};
    }
}


std::unique_ptr<type_spec>
ast_converter::convert_record_type_spec(ast_node * parent, const ::clang::TagTypeLoc & loc) {
    auto clang_decl = loc.getDecl();
    assert(clang_decl != nullptr && "can't get decl from type location");

    auto clang_rec_decl = ::clang::dyn_cast<::clang::CXXRecordDecl>(clang_decl);
    assert(clang_rec_decl != nullptr && "unsupported tag type");

    if (loc.isDefinition()) {
        auto rec = cm_conv_.get_cm_entity_as<record_type>(clang_rec_decl);
        assert(rec && "can't find code model entity for record decl");

        auto spec = std::make_unique<record_decl_type_spec>(parent);
        spec->set_entity(rec);
        spec->set_source_range(convert_source_range(loc.getSourceRange()));
        spec->set_decl(convert_record_decl(spec.get(), clang_rec_decl));
        spec->adjust_source_range(spec->decl()->source_range().range());

        return spec;
    } else {
        auto rec = cm_conv_.get_cm_entity_as<named_record_type>(clang_rec_decl);
        assert(rec && "can't find code model entity for record decl");
    
        auto spec = std::make_unique<record_type_spec>(parent);
        spec->set_entity(rec);
        spec->set_source_range(convert_source_range(loc.getSourceRange()));
        spec->set_name(rec->name(), convert_source_range(loc.getNameLoc(), rec->name().size()));
        spec->adjust_source_range(spec->name()->source_range().range());

        return spec;
    }   
}


/// Converts elaborated type specified
std::unique_ptr<type_spec>
ast_converter::convert_elaborated_type_spec(ast_node * parent,
                                            const ::clang::ElaboratedTypeLoc & loc) {
    auto n_loc = loc.getNamedTypeLoc();
    assert(!n_loc.isNull() && "invalid named type loc in elaborated type loc");

    // special case for template specilization is required because template name
    // is not a type
    if (auto tspec_loc = n_loc.getAs<::clang::TemplateSpecializationTypeLoc>()) {
        // converting type specifier
        auto res = convert_template_subst_type_spec(parent, tspec_loc);

        // setting source range for type specifier
        res->set_source_range(convert_source_range(loc.getSourceRange()));

        // converting template scope
        if (loc.getQualifierLoc().hasQualifier()) {
            auto qloc = loc.getQualifierLoc();
            auto scope = convert_scope_spec(res.get(), qloc.getNestedNameSpecifier(), qloc);

            // adjusting template specifier range with qualifier
            res->templ()->set_source_range_start(scope->source_range().range().start());

            res->templ()->set_scope(std::move(scope));
        }

        return res;
    }

    // converting type specifier
    auto spec = convert_type_spec(parent, n_loc);

    // converting scope
    if (loc.getQualifierLoc().hasQualifier()) {
        // converted type specifier must be a context type specifier
        auto ctx_spec = dynamic_cast<context_type_spec*>(spec.get());
        assert(ctx_spec && "type specifier with scope must be a context type specifier");

        // converting scope
        auto qloc = loc.getQualifierLoc();
        auto scope = convert_scope_spec(ctx_spec, qloc.getNestedNameSpecifier(), qloc);

        // adjusting type specifier range with qualifier
        ctx_spec->adjust_source_range(scope->source_range().range());

        ctx_spec->set_scope(std::move(scope));
    }

    return spec;
}


std::unique_ptr<template_param_type_spec>
ast_converter::convert_template_param(ast_node * parent,
                                      const ::clang::TemplateTypeParmTypeLoc & loc) {
    auto clang_decl = loc.getDecl();
    auto par = cm_conv_.get_cm_entity_as<type_template_parameter>(clang_decl);
    assert(par != nullptr && "can't find template parameter in code model");
    auto range = convert_source_range(loc.getBeginLoc(), clang_decl->getName().size());

    auto t_spec = std::make_unique<template_param_type_spec>(parent);
    t_spec->set_entity(par);
    t_spec->set_source_range(range);
    t_spec->set_name(clang_decl->getName(), range);

    return t_spec;
}


std::unique_ptr<type_spec>
ast_converter::convert_type_spec(ast_node * parent, const ::clang::TypeLoc & loc) {

    if (auto bt_loc = loc.getAs<::clang::BuiltinTypeLoc>()) {
        return convert_builtin_type_spec(parent, bt_loc);
    } else if (auto td_loc = loc.getAs<::clang::TypedefTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto icls_loc = loc.getAs<::clang::InjectedClassNameTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto using_loc = loc.getAs<::clang::UnresolvedUsingTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto tag_loc = loc.getAs<::clang::TagTypeLoc>()) {
        return convert_record_type_spec(parent, tag_loc);
    } else if (auto tpar_loc = loc.getAs<::clang::TemplateTypeParmTypeLoc>()) {
        return convert_template_param(parent, tpar_loc);
    } else if (auto macro_loc = loc.getAs<::clang::MacroQualifiedTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto p_loc = loc.getAs<::clang::ParenTypeLoc>()) {
        return convert_parens_type_spec(parent, p_loc);
    } else if (auto adj_loc = loc.getAs<::clang::AdjustedTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto dec_loc = loc.getAs<::clang::DecayedTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto ptr_loc = loc.getAs<::clang::PointerTypeLoc>()) {
        return convert_pointer_type_spec(parent, ptr_loc);
    } else if (auto ref_loc = loc.getAs<::clang::LValueReferenceTypeLoc>()) {
        return convert_lvalue_reference_type_spec(parent, ref_loc);
    } else if (auto ref_loc = loc.getAs<::clang::RValueReferenceTypeLoc>()) {
        return convert_rvalue_reference_type_spec(parent, ref_loc);
    } else if (auto func_loc = loc.getAs<::clang::FunctionTypeLoc>()) {
        return convert_function_type_spec(parent, func_loc);
    } else if (auto mem_ptr_loc = loc.getAs<::clang::MemberPointerTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto func_loc = loc.getAs<::clang::FunctionNoProtoTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto arr_loc = loc.getAs<::clang::ArrayTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto vec_loc = loc.getAs<::clang::VectorTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto m_loc = loc.getAs<::clang::MatrixTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto compl_loc = loc.getAs<::clang::ComplexTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto tspec_loc = loc.getAs<::clang::TemplateSpecializationTypeLoc>()) {
        return convert_template_subst_type_spec(parent, tspec_loc);
    } else if (auto dep_space_loc = loc.getAs<::clang::DependentAddressSpaceTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto tof_expr_loc = loc.getAs<::clang::TypeOfExprTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto tof_loc = loc.getAs<::clang::TypeOfTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto dtype_loc = loc.getAs<::clang::DecltypeTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto ut_loc = loc.getAs<::clang::UnaryTransformTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto dt_loc = loc.getAs<::clang::DeducedTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto a_loc = loc.getAs<::clang::AutoTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto t_loc = loc.getAs<::clang::DeducedTemplateSpecializationTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto e_loc = loc.getAs<::clang::ElaboratedTypeLoc>()) {
        return convert_elaborated_type_spec(parent, e_loc);
    } else if (auto dn_loc = loc.getAs<::clang::DependentNameTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto dspec_loc = loc.getAs<::clang::DependentTemplateSpecializationTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto pack_loc = loc.getAs<::clang::PackExpansionTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto at_loc = loc.getAs<::clang::AtomicTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (auto p_loc = loc.getAs<::clang::PipeTypeLoc>()) {
        assert(false && "NYI");
        return {};
    } else if (loc.getAs<::clang::ObjCTypeParamTypeLoc>() ||
               loc.getAs<::clang::ObjCObjectTypeLoc>() ||
               loc.getAs<::clang::ObjCInterfaceTypeLoc>() ||
               loc.getAs<::clang::BlockPointerTypeLoc>() ||
               loc.getAs<::clang::ObjCObjectPointerTypeLoc>()) {
        assert (false && "Objective-C support is not implemented");
        return {};
    } else {
        std::cerr << "Unknown TypeLoc kind: " << loc.getTypeLocClass() << std::endl;
        assert(false && "unknown type location kind");
        return {};
    }
}



void ast_converter::convert_template_and_arguments(
        const ::clang::TemplateSpecializationTypeLoc & loc,
        template_substitution_spec * parent,
        template_name * templ) {

    // converting template name
    auto templ_spec = std::make_unique<template_spec>(parent);
    templ_spec->set_entity(templ);
    auto name_range = convert_source_range(loc.getTemplateNameLoc(), templ->name().size());
    templ_spec->set_name(templ->name(), name_range);
    templ_spec->set_source_range(name_range);
    parent->set_templ(std::move(templ_spec));

    // converting template arguments
    for (unsigned int i = 0, sz = loc.getNumArgs(); i < sz; ++i) {
        auto arg_loc = loc.getArgLoc(i);
        auto & arg = arg_loc.getArgument();
        template_argument_spec * arg_spec = nullptr;

        if (arg.getKind() == ::clang::TemplateArgument::Type) {
            auto t_arg_spec = std::make_unique<type_template_argument_spec>(parent);
            arg_spec = t_arg_spec.get();

            auto clang_arg = loc.getArgLocInfo(i);
            auto clang_type_loc = clang_arg.getAsTypeSourceInfo()->getTypeLoc();
            t_arg_spec->set_type(convert_type_spec(t_arg_spec.get(), clang_type_loc));
            parent->add_arg(std::move(t_arg_spec));

        } else if (arg.getKind() == ::clang::TemplateArgument::Integral ||
                   arg.getKind() == ::clang::TemplateArgument::Expression) {
            auto v_arg_spec = std::make_unique<value_template_argument_spec>(parent);
            arg_spec = v_arg_spec.get();
            parent->add_arg(std::move(v_arg_spec));

        } else {
            std::cerr << "KIND: " << arg.getKind() << std::endl;
            assert(false && "template arguments other than types and "
                            "expressions are not implemented yet");
        }

        arg_spec->set_source_range(convert_source_range(arg_loc.getSourceRange()));

        // adjusting end source positions for template argument with Lexer::getLocForEndOfToken
        auto end_loc = ::clang::Lexer::getLocForEndOfToken(arg_loc.getSourceRange().getEnd(),
                                                           0,
                                                           clang_ast_ctx_->getSourceManager(),
                                                           {});
        arg_spec->set_source_range_end(convert_source_pos(end_loc));
    }
}


source_position ast_converter::convert_source_pos(const ::clang::SourceLocation & loc) {
    auto start_ploc = clang_ast_ctx_->getSourceManager().getPresumedLoc(loc);
    return source_position{start_ploc.getLine(), start_ploc.getColumn()};
}


source_file_range ast_converter::convert_source_range(const ::clang::SourceLocation & start,
                                                      const ::clang::SourceLocation & end) {
    // TODO: macros support

    // converting start location
    auto start_ploc = clang_ast_ctx_->getSourceManager().getPresumedLoc(start);
    source_position start_pos{start_ploc.getLine(), start_ploc.getColumn()};
    auto start_file = scm_.code_mdl().source(start_ploc.getFilename());

    // converting end location
    auto end_ploc = clang_ast_ctx_->getSourceManager().getPresumedLoc(end);
    source_position end_pos{end_ploc.getLine(), end_ploc.getColumn()};
    auto end_file = scm_.code_mdl().source(end_ploc.getFilename());

    // TODO: support this case
    assert(start_file == end_file && "AST nodes splitted between source files are not supported");

    return source_file_range{start_file, source_range{start_pos, end_pos}};
}


source_file_range ast_converter::convert_source_range(const ::clang::SourceRange & range) {
    return convert_source_range(range.getBegin(), range.getEnd());
}


source_file_range ast_converter::convert_source_range(const ::clang::SourceLocation & start,
                                                      unsigned int offset) {
    auto end = start.getLocWithOffset(static_cast<int32_t>(static_cast<int>(offset)));
    return convert_source_range(start, end);
}


std::unique_ptr<function_parameter_decl>
ast_converter::convert_function_parameter(functional_decl * parent,
                                          function_parameter * par,
                                          const ::clang::ParmVarDecl * clang_decl) {
    auto par_decl = std::make_unique<function_parameter_decl>(parent);
    par_decl->set_entity(par);

    // converting parmaeter name
    if (auto nm = clang_decl->getName(); !nm.empty()) {
        par_decl->set_name(nm.str(), convert_source_range(clang_decl->getLocation(), nm.size()));
    }

    // converting parameter type
    par_decl->set_type(convert_type_spec(par_decl.get(),
                                         clang_decl->getTypeSourceInfo()->getTypeLoc()));

    // converting parameter location
    par_decl->set_source_range(convert_source_range(clang_decl->getSourceRange()));

    return par_decl;
}


}
