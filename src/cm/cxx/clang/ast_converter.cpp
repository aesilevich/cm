// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file ast_converter.cpp
/// Contains implementation of the ast_converter class.

#include "cm/cxx/clang/ast_converter.hpp"
#include "cm/namespace.hpp"
#include "cm/record_kind.hpp"
#include "cm/record_type.hpp"
#include "cm/template_instantiation.hpp"
#include <clang/AST/Decl.h>
#include <clang/AST/DeclBase.h>
#include <clang/AST/Type.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/DeclTemplate.h>
#include <clang/Basic/Specifiers.h>
#include <clang/Basic/SourceManager.h>
#include <ranges>


namespace cm::clang {


/// Converts clang tag kind to record kind
record_kind clang_tag_kind_to_record_kind(::clang::TagTypeKind clang_knd) {
    switch (clang_knd) {
    case ::clang::TagTypeKind::Struct:
        return record_kind::struct_;
    case ::clang::TagTypeKind::Class:
        return record_kind::class_;
    case ::clang::TagTypeKind::Union:
        return record_kind::union_;
    case ::clang::TagTypeKind::Enum:
        assert(false && "should not be enum here");
        // fall through in release mode
    default:
        // unknown record kind. Leave struct
        return record_kind::struct_;
    }
}


/// Returns access specifier for declaration
member_access_spec get_clang_decl_acc_spec(const ::clang::Decl * decl) {
    switch (decl->getAccess()) {
    case ::clang::AS_none:
    case ::clang::AS_public:
        return member_access_spec::public_;
    case ::clang::AS_protected:
        return member_access_spec::protected_;
    case ::clang::AS_private:
        return member_access_spec::private_;
    default:
        assert(false && "unknown clang access specifier");
        return member_access_spec::public_;
    }
}


void ast_converter::convert(const ::clang::ASTContext & ctx) {
    clang_ast_ctx_ = &ctx;

    // traversing over all top level declarations in translation unit
    auto tu_decl = ctx.getTranslationUnitDecl();

    context_setter csetter{*this, &mdl_, tu_decl};

    for (auto && decl : tu_decl->decls()) {
        // converting namespaces separately from other declarations
        if (auto ns = ::clang::dyn_cast<::clang::NamespaceDecl>(decl)) {
            convert_ns(ns);
        } else {
            convert_decl(decl);
        }
    }

    clang_ast_ctx_ = nullptr;
}


qual_type ast_converter::convert_type(const ::clang::QualType & clang_qual_type) {
    // converting base type
    auto clang_type = clang_qual_type.getTypePtr();
    type_t * type = nullptr;
    if (auto clang_bt_type = ::clang::dyn_cast<::clang::BuiltinType>(clang_type)) {
        type = convert_builtin_type(clang_bt_type);
    } else if (auto clang_ptr_type = ::clang::dyn_cast<::clang::PointerType>(clang_type)) {
        type = convert_pointer_type(clang_ptr_type);
    } else if (auto clang_ref_type = ::clang::dyn_cast<::clang::LValueReferenceType>(clang_type)) {
        type = convert_lvalue_reference_type(clang_ref_type);
    } else if (auto clang_ref_type = ::clang::dyn_cast<::clang::RValueReferenceType>(clang_type)) {
        type = convert_rvalue_reference_type(clang_ref_type);
    } else if (auto clang_func_type = ::clang::dyn_cast<::clang::FunctionType>(clang_type)) {
        type = convert_function_type(clang_func_type);
    } else if (auto clang_rec_type = ::clang::dyn_cast<::clang::RecordType>(clang_type)) {
        type = convert_record_type(clang_rec_type);
    } else if (auto clang_arr_type = ::clang::dyn_cast<::clang::ConstantArrayType>(clang_type)) {
        type = convert_array_type(clang_arr_type);
    } else if (auto clang_elab_type = ::clang::dyn_cast<::clang::ElaboratedType>(clang_type)) {
        type = convert_type(clang_elab_type->getNamedType()).type();
    } else if (auto clang_td_type = ::clang::dyn_cast<::clang::TypedefType>(clang_type)) {
        // typedef type must already exist in code model, getting it
        auto typedef_decl = clang_td_type->getDecl();
        type = get_cm_entity_as<typedef_type>(typedef_decl);
        assert(type && "typedef type must already exist in code model");
    } else if (auto clang_tpar_type = ::clang::dyn_cast<::clang::TemplateTypeParmType>(clang_type)) {
        type = convert_type_template_param_type(clang_tpar_type);
    } else if (auto clang_templ_spec = ::clang::dyn_cast<::clang::TemplateSpecializationType>(clang_type)) {
        type = convert_template_spec_type(clang_templ_spec);
    } else if (auto clang_subst_tpar = ::clang::dyn_cast<::clang::SubstTemplateTypeParmType>(clang_type)) {
        // TODO: do we need store this info in code model?
        type = convert_type(clang_subst_tpar->getReplacementType()).type();
    } else if (auto dep_type = ::clang::dyn_cast<::clang::DependentNameType>(clang_type)) {
        type = convert_dependent_type(dep_type);
    } else if (auto dt_type = ::clang::dyn_cast<::clang::DecltypeType>(clang_type)) {
        type = convert_decltype_type(dt_type);
    } else if (auto p_type = ::clang::dyn_cast<::clang::ParenType>(clang_type)) {
        // ignoring parens in code model
        return convert_type(p_type->getInnerType());
    } else {
        std::cerr << "DON'T NOT KNOW HOW TO CONVERT TYPE:\n";
        clang_type->dump();
        // TODO: diagnostics
        assert(false && "Don't not know how to convert type");
    }

    // adding cv-qualifiers
    assert(type && "type should not be null here");
    return qual_type{type, clang_qual_type.isLocalConstQualified(), clang_qual_type.isLocalVolatileQualified()};
}


type_t * ast_converter::convert_builtin_type(const ::clang::BuiltinType * clang_bt_type) {
    switch (clang_bt_type->getKind()) {
    case ::clang::BuiltinType::Void:
        return mdl_.bt_void();

    case ::clang::BuiltinType::Bool:
        return mdl_.bt_bool();

    // integer types
    case ::clang::BuiltinType::Char_U:
        return mdl_.bt_char();
    case ::clang::BuiltinType::UChar:
        return mdl_.bt_unsigned_char();
    case ::clang::BuiltinType::WChar_U:
        return mdl_.bt_wchar_t();
    case ::clang::BuiltinType::Char8:
        return mdl_.bt_char8_t();
    case ::clang::BuiltinType::Char16:
        return mdl_.bt_char16_t();
    case ::clang::BuiltinType::Char32:
        return mdl_.bt_char32_t();
    case ::clang::BuiltinType::UShort:
        return mdl_.bt_unsigned_short();
    case ::clang::BuiltinType::UInt:
        return mdl_.bt_unsigned_int();
    case ::clang::BuiltinType::ULong:
        return mdl_.bt_unsigned_long();
    case ::clang::BuiltinType::ULongLong:
        return mdl_.bt_unsigned_long_long();
    case ::clang::BuiltinType::UInt128:
        return mdl_.bt_uint128();
    case ::clang::BuiltinType::Char_S:
        return mdl_.bt_char();
    case ::clang::BuiltinType::SChar:
        return mdl_.bt_signed_char();
    case ::clang::BuiltinType::WChar_S:
        return mdl_.bt_wchar_t();
    case ::clang::BuiltinType::Short:
        return mdl_.bt_short();
    case ::clang::BuiltinType::Int:
        return mdl_.bt_int();
    case ::clang::BuiltinType::Long:
        return mdl_.bt_long();
    case ::clang::BuiltinType::LongLong:
        return mdl_.bt_long_long();
    case ::clang::BuiltinType::Int128:
        return mdl_.bt_int128();

    // GNU C extension fixed point types. Just treat them as integer types for now
    // TODO: support fixed point types
    case ::clang::BuiltinType::ShortAccum:
        return mdl_.bt_short();
    case ::clang::BuiltinType::Accum:
        return mdl_.bt_int();
    case ::clang::BuiltinType::LongAccum:
        return mdl_.bt_long();
    case ::clang::BuiltinType::UShortAccum:
        return mdl_.bt_unsigned_short();
    case ::clang::BuiltinType::UAccum:
        return mdl_.bt_unsigned_int();
    case ::clang::BuiltinType::ULongAccum:
        return mdl_.bt_unsigned_long();
    case ::clang::BuiltinType::ShortFract:
        return mdl_.bt_short();
    case ::clang::BuiltinType::Fract:
        return mdl_.bt_int();
    case ::clang::BuiltinType::LongFract:
        return mdl_.bt_long();
    case ::clang::BuiltinType::UShortFract:
        return mdl_.bt_unsigned_short();
    case ::clang::BuiltinType::UFract:
        return mdl_.bt_unsigned_int();
    case ::clang::BuiltinType::ULongFract:
        return mdl_.bt_unsigned_long();
    case ::clang::BuiltinType::SatShortAccum:
        return mdl_.bt_short();
    case ::clang::BuiltinType::SatAccum:
        return mdl_.bt_int();
    case ::clang::BuiltinType::SatLongAccum:
        return mdl_.bt_long();
    case ::clang::BuiltinType::SatUShortAccum:
        return mdl_.bt_unsigned_short();
    case ::clang::BuiltinType::SatUAccum:
        return mdl_.bt_unsigned_int();
    case ::clang::BuiltinType::SatULongAccum:
        return mdl_.bt_unsigned_long();
    case ::clang::BuiltinType::SatShortFract:
        return mdl_.bt_short();
    case ::clang::BuiltinType::SatFract:
        return mdl_.bt_int();
    case ::clang::BuiltinType::SatLongFract:
        return mdl_.bt_long();
    case ::clang::BuiltinType::SatUShortFract:
        return mdl_.bt_unsigned_short();
    case ::clang::BuiltinType::SatUFract:
        return mdl_.bt_unsigned_int();
    case ::clang::BuiltinType::SatULongFract:
        return mdl_.bt_unsigned_long();

    // float types
    case ::clang::BuiltinType::Half:
        // TODO: support OpenCL half float
        return mdl_.bt_float();
    case ::clang::BuiltinType::Float:
        return mdl_.bt_float();
    case ::clang::BuiltinType::Double:
        return mdl_.bt_double();
    case ::clang::BuiltinType::LongDouble:
        return mdl_.bt_long_double();
    case ::clang::BuiltinType::Float16:
        // TODO: support float16
        assert(false && "don't know how to convert float16 type");
        return nullptr;
    case ::clang::BuiltinType::Float128:
        // TODO: support float128
        assert(false && "don't know how to convert float128 type");
        return nullptr;

    // other types
    case ::clang::BuiltinType::NullPtr:
        assert(false && "don't know how to convet nullptr type");
        return nullptr;

    case ::clang::BuiltinType::ObjCId:
    case ::clang::BuiltinType::ObjCClass:
    case ::clang::BuiltinType::ObjCSel:
    case ::clang::BuiltinType::OCLSampler:
    case ::clang::BuiltinType::OCLEvent:
    case ::clang::BuiltinType::OCLClkEvent:
    case ::clang::BuiltinType::OCLQueue:
    case ::clang::BuiltinType::OCLReserveID:
        // TODO: support objective-c types
        return mdl_.get_or_create_ptr_type(mdl_.bt_void());

    case ::clang::BuiltinType::Dependent:
        // TODO: check if we need it
        assert(false && "don't know how to convert dependent type");
        return nullptr;

    case ::clang::BuiltinType::Overload:
        // TODO: check if we need it
        assert(false && "don't know how to convert overload type");
        return nullptr;

    case ::clang::BuiltinType::BoundMember:
        // TODO: check if we need it
        assert(false && "don't know how to convert bound member type");
        return nullptr;

    case ::clang::BuiltinType::PseudoObject:
        // TODO: check if we need it
        assert(false && "don't know how to convert pseudo object type");
        return nullptr;

    case ::clang::BuiltinType::UnknownAny:
        // TODO: check if we need it
        assert(false && "don't know how to convert unknown any type");
        return nullptr;

    case ::clang::BuiltinType::BuiltinFn:
        // TODO: check if we need it
        assert(false && "don't know how to convert builtin fn type");
        return nullptr;

    case ::clang::BuiltinType::ARCUnbridgedCast:
        // TODO: add support of objective-c
        return mdl_.bt_int();

    case ::clang::BuiltinType::OMPArraySection:
        // TODO: add support of openmp
        return mdl_.bt_int();

    case ::clang::BuiltinType::SveInt8:
        return mdl_.bt_arm_sve_int8x1();
    case ::clang::BuiltinType::SveInt8x2:
        return mdl_.bt_arm_sve_int8x2();
    case ::clang::BuiltinType::SveInt8x3:
        return mdl_.bt_arm_sve_int8x3();
    case ::clang::BuiltinType::SveInt8x4:
        return mdl_.bt_arm_sve_int8x4();
    case ::clang::BuiltinType::SveInt16:
        return mdl_.bt_arm_sve_int16x1();
    case ::clang::BuiltinType::SveInt16x2:
        return mdl_.bt_arm_sve_int16x2();
    case ::clang::BuiltinType::SveInt16x3:
        return mdl_.bt_arm_sve_int16x3();
    case ::clang::BuiltinType::SveInt16x4:
        return mdl_.bt_arm_sve_int16x4();
    case ::clang::BuiltinType::SveInt32:
        return mdl_.bt_arm_sve_int32x1();
    case ::clang::BuiltinType::SveInt32x2:
        return mdl_.bt_arm_sve_int32x2();
    case ::clang::BuiltinType::SveInt32x3:
        return mdl_.bt_arm_sve_int32x3();
    case ::clang::BuiltinType::SveInt32x4:
        return mdl_.bt_arm_sve_int32x4();
    case ::clang::BuiltinType::SveInt64:
        return mdl_.bt_arm_sve_int64x1();
    case ::clang::BuiltinType::SveInt64x2:
        return mdl_.bt_arm_sve_int64x2();
    case ::clang::BuiltinType::SveInt64x3:
        return mdl_.bt_arm_sve_int64x3();
    case ::clang::BuiltinType::SveInt64x4:
        return mdl_.bt_arm_sve_int64x4();

    case ::clang::BuiltinType::SveUint8:
        return mdl_.bt_arm_sve_uint8x1();
    case ::clang::BuiltinType::SveUint8x2:
        return mdl_.bt_arm_sve_uint8x2();
    case ::clang::BuiltinType::SveUint8x3:
        return mdl_.bt_arm_sve_uint8x3();
    case ::clang::BuiltinType::SveUint8x4:
        return mdl_.bt_arm_sve_uint8x4();
    case ::clang::BuiltinType::SveUint16:
        return mdl_.bt_arm_sve_uint16x1();
    case ::clang::BuiltinType::SveUint16x2:
        return mdl_.bt_arm_sve_uint16x2();
    case ::clang::BuiltinType::SveUint16x3:
        return mdl_.bt_arm_sve_uint16x3();
    case ::clang::BuiltinType::SveUint16x4:
        return mdl_.bt_arm_sve_uint16x4();
    case ::clang::BuiltinType::SveUint32:
        return mdl_.bt_arm_sve_uint32x1();
    case ::clang::BuiltinType::SveUint32x2:
        return mdl_.bt_arm_sve_uint32x2();
    case ::clang::BuiltinType::SveUint32x3:
        return mdl_.bt_arm_sve_uint32x3();
    case ::clang::BuiltinType::SveUint32x4:
        return mdl_.bt_arm_sve_uint32x4();
    case ::clang::BuiltinType::SveUint64:
        return mdl_.bt_arm_sve_uint64x1();
    case ::clang::BuiltinType::SveUint64x2:
        return mdl_.bt_arm_sve_uint64x2();
    case ::clang::BuiltinType::SveUint64x3:
        return mdl_.bt_arm_sve_uint64x3();
    case ::clang::BuiltinType::SveUint64x4:
        return mdl_.bt_arm_sve_uint64x4();

    case ::clang::BuiltinType::SveFloat16:
        return mdl_.bt_arm_sve_float16x1();
    case ::clang::BuiltinType::SveFloat16x2:
        return mdl_.bt_arm_sve_float16x2();
    case ::clang::BuiltinType::SveFloat16x3:
        return mdl_.bt_arm_sve_float16x3();
    case ::clang::BuiltinType::SveFloat16x4:
        return mdl_.bt_arm_sve_float16x4();
    case ::clang::BuiltinType::SveFloat32:
        return mdl_.bt_arm_sve_float32x1();
    case ::clang::BuiltinType::SveFloat32x2:
        return mdl_.bt_arm_sve_float32x2();
    case ::clang::BuiltinType::SveFloat32x3:
        return mdl_.bt_arm_sve_float32x3();
    case ::clang::BuiltinType::SveFloat32x4:
        return mdl_.bt_arm_sve_float32x4();
    case ::clang::BuiltinType::SveFloat64:
        return mdl_.bt_arm_sve_float64x1();
    case ::clang::BuiltinType::SveFloat64x2:
        return mdl_.bt_arm_sve_float64x2();
    case ::clang::BuiltinType::SveFloat64x3:
        return mdl_.bt_arm_sve_float64x3();
    case ::clang::BuiltinType::SveFloat64x4:
        return mdl_.bt_arm_sve_float64x4();

    case ::clang::BuiltinType::SveBFloat16:
        return mdl_.bt_arm_sve_bfloat16x1();
    case ::clang::BuiltinType::SveBFloat16x2:
        return mdl_.bt_arm_sve_bfloat16x2();
    case ::clang::BuiltinType::SveBFloat16x3:
        return mdl_.bt_arm_sve_bfloat16x3();
    case ::clang::BuiltinType::SveBFloat16x4:
        return mdl_.bt_arm_sve_bfloat16x4();

    case ::clang::BuiltinType::SveBool:
        return mdl_.bt_arm_sve_boolx1();
    case ::clang::BuiltinType::SveBoolx2:
        return mdl_.bt_arm_sve_boolx2();
    case ::clang::BuiltinType::SveBoolx4:
        return mdl_.bt_arm_sve_boolx4();

    case ::clang::BuiltinType::SveCount:
        return mdl_.bt_arm_sve_count();

    // case ::clang::BuiltinType::SveBFloat16x2:
    //     return mdl_.bt_arm_sve_bfloat16x2();
    // case ::clang::BuiltinType::SveBFloat16x3:
    //     return mdl_.bt_arm_sve_bfloat16x3();

    // case ::clang::BuiltinType::sve:
    //     return mdl_.bt_arm_sve_bfloat16x4();

    default:
        std::cout << "UNKNOWN BUILTIN TYPE:\n" << std::endl;
        clang_bt_type->dump();
        assert(false && "Unknown builtin type");
        return nullptr;
    }
}


pointer_type * ast_converter::convert_pointer_type(const ::clang::PointerType * clang_ptr_type) {
    auto pointee_type = convert_type(clang_ptr_type->getPointeeType());
    return mdl_.get_or_create_ptr_type(pointee_type);
}


lvalue_reference_type *
ast_converter::convert_lvalue_reference_type(const ::clang::LValueReferenceType * clang_ref_type) {
    auto base_type = convert_type(clang_ref_type->getPointeeType());
    return mdl_.get_or_create_lvalue_ref_type(base_type);
}


rvalue_reference_type *
ast_converter::convert_rvalue_reference_type(const ::clang::RValueReferenceType * clang_ref_type) {
    auto base_type = convert_type(clang_ref_type->getPointeeType());
    return mdl_.get_or_create_rvalue_ref_type(base_type);
}


array_type * ast_converter::convert_array_type(const ::clang::ConstantArrayType * clang_arr_type) {
    auto elt_type = convert_type(clang_arr_type->getElementType());
    return mdl_.get_or_create_arr_type(elt_type.type(), clang_arr_type->getSize().getLimitedValue());
}


type_template_parameter * ast_converter::convert_type_template_param_type(
        const ::clang::TemplateTypeParmType * type) {

    templated_entity * templ = nullptr;

    // searching for template declaration in the chain of parent declaration contexts,

    auto c_decl_ctx = clang_ctx_;
    while (true) {
        if (auto decl = ::clang::dyn_cast<::clang::Decl>(c_decl_ctx)) {
            if (auto pars = decl->getDescribedTemplateParams(); pars != nullptr) {
                assert(pars->getDepth() >= type->getDepth() &&
                       "missed template parameter list in parents chain");
                if (pars->getDepth() == type->getDepth()) {
                    templ = get_cm_entity_as<templated_entity>(decl);
                    assert(templ && "can't find templated entity");
                    break;
                }
            }
        }

        // moving to the parent context
        c_decl_ctx = c_decl_ctx->getParent();
    }

    // getting template parameter from templated entity
    auto pars = templ->template_params();
    assert(type->getIndex() < std::ranges::distance(pars) && "invalid template parameter index");
    auto it = std::ranges::begin(pars);
    std::advance(it, type->getIndex());
    auto par = *it;

    // checking parameter type
    auto type_par = dynamic_cast<type_template_parameter*>(par);
    assert(type_par && "template parameter is not a type");
    return type_par;

    // // getting declaration context for template parameter type
    // auto type_decl = clang_type->getDecl();
    // auto decl_ctx = type_decl->getDeclContext();
    // auto parent_decl = ::clang::dyn_cast<::clang::Decl>(decl_ctx);

    // // getting CM template associated with clang record
    // auto rec = get_cm_entity_as<template_>(parent_decl);
    // assert(rec != nullptr && "no CM template associated with template parameter context");

    // // getting template parameter
    // auto idx = clang_type->getIndex();
    // assert(idx < std::ranges::size(rec->template_params()) && "invalid template parameter index");
    // auto tpar = dynamic_cast<type_template_parameter*>(rec->template_params()[idx]);
    // assert(tpar && "template parameter is not a type template parameter in code model");
    // return tpar;
}


type_t *
ast_converter::convert_template_spec_type(const ::clang::TemplateSpecializationType * type) {
    if (type->isTypeAlias()) {
        assert(false && "type aliases are not implemented yet");
        return nullptr;
    } else {
        // getting record declaration for template specialization or template declaration
        auto rec_decl = type->getAsCXXRecordDecl();
        if (rec_decl) {
            if (auto templ_decl = rec_decl->getDescribedClassTemplate()) {
                // template instantiation is just a template declaration being processed now
                auto rec = get_cm_entity_as<template_record>(rec_decl);
                return rec->this_type();
            } else {
                // std::cout << "TEMPLATE SPECIALIZATION TYPE: " << rec_decl << std::endl;
                // rec_decl->dump();

                // looking for existing CM entity associated with specialization record decl
                auto rec = get_cm_entity_as<template_record_instantiation_type>(rec_decl);
                assert(rec != nullptr && "no CM record associated with template specialization type");
                return rec;
            }
        } else {
            // template specialization without record declaration must be a dependent type
            assert(type->isDependentType() && "required dependent type here");

            // getting template declaration
            auto templ_decl = type->getTemplateName().getAsTemplateDecl();
            assert(templ_decl && "template delcaration is null for dependent template name");
            auto templ_rec_decl = templ_decl->getTemplatedDecl();
            assert(templ_rec_decl && "record delcaration is null for dependent template name");

            // getting code model template associated with template declaration
            auto templ = get_cm_entity_as<template_record>(templ_rec_decl);
            assert(templ != nullptr && "can't find CM template for tempalte decl");

            // converting template arguments
            auto args = convert_template_arguments(type->template_arguments());

            // looking for existing instantiation
            auto inst = templ->find_dependent_instantiation(args);

            if (inst) {
                return inst;
            } else {
                // creating template dependent instantiation
                return templ->create_dependent_instantiation(args);
            }
        }
    }
}


function_type *
ast_converter::convert_function_type(const ::clang::FunctionType * clang_func_type) {
    assert(clang_func_type && "clang function type should not be null here");

    // converting return type
    auto ret_type = convert_type(clang_func_type->getReturnType());

    if (::clang::dyn_cast<::clang::FunctionNoProtoType>(clang_func_type)) {
        // K&R function without parameters
        return mdl_.get_or_create_func_type(ret_type);
    }

    auto clang_func_proto_type = ::clang::dyn_cast<::clang::FunctionProtoType>(clang_func_type);
    assert(clang_func_proto_type && "function type must be FunctionNoProtoType or FunctionProtoType");

    // creating range view that contains clang parameter types converted to code model types
    auto params_idxs = std::ranges::views::iota(0U, clang_func_proto_type->getNumParams());
    auto clang_params = params_idxs | std::ranges::views::transform([clang_func_proto_type](auto idx) {
        return clang_func_proto_type->getParamType(idx);
    });

    auto convert_param = [this](auto && clang_param) {
        return convert_type(clang_param);
    };
    auto converted_params = clang_params | std::ranges::views::transform(convert_param);

    return mdl_.get_or_create_func_type_r(ret_type, converted_params);
}


record_type * ast_converter::convert_record_type(const ::clang::RecordType * clang_rec_type) {
    // looking for existing code model type for recrod declaration
    if (auto rec = get_cm_entity_as<record_type>(clang_rec_type->getDecl())) {
        return rec;
    }

    // creating new empty record for declaration
    // NOTE: this case is supposed to be used only for some builtin implicit record declarations
    // created by compiler such as __NSConstantString_tag

    // // record declaration that does not exist must be in translation unit declaration context
    // assert(clang_rec_type->getDecl()->getDeclContext()->getDeclKind() == ::clang::Decl::TranslationUnit &&
    //        "not existing record declaration must be in translation unit decl context");

    return create_new_record(clang_rec_type->getDecl());
}


dependent_type *
ast_converter::convert_dependent_type(const ::clang::DependentNameType * clang_type) {
    return ctx_->create_entity<dependent_type>();
}


decltype_type *
ast_converter::convert_decltype_type(const ::clang::DecltypeType * clang_type) {
    return ctx_->create_entity<decltype_type>();
}


void ast_converter::convert_decl(const ::clang::Decl * clang_decl) {
    // std::cout << "DECL: " << clang_decl << ", canonical = "
    //           << clang_decl->getCanonicalDecl() << std::endl    ;
    // clang_decl->dump();

    if (auto * ns = ::clang::dyn_cast<::clang::NamespaceDecl>(clang_decl)) {
        // should not be namespace here
        assert(false && "namespace decl should not be passed here");
    }

    if (auto * clang_class_templ_decl = ::clang::dyn_cast<::clang::ClassTemplateDecl>(clang_decl)) {
        convert_template_class(clang_class_templ_decl);
    } else if (auto p_spec =
               ::clang::dyn_cast<::clang::ClassTemplatePartialSpecializationDecl>(clang_decl)) {
        convert_template_partial_specialization(p_spec);
    } else if (auto * clang_record_decl = ::clang::dyn_cast<::clang::RecordDecl>(clang_decl)) {
        convert_record(clang_record_decl);
    } else if (auto * clang_typedef_decl = ::clang::dyn_cast<::clang::TypedefNameDecl>(clang_decl)) {
        convert_typedef(clang_typedef_decl);
    } else if (auto * clang_func_decl = ::clang::dyn_cast<::clang::FunctionDecl>(clang_decl)) {
        convert_function(clang_func_decl);
    } else if (auto * clang_var_decl = ::clang::dyn_cast<::clang::VarDecl>(clang_decl)) {
        convert_variable(clang_var_decl);
    } else if (auto * tfunc_decl = ::clang::dyn_cast<::clang::FunctionTemplateDecl>(clang_decl)) {
        convert_template_function(tfunc_decl);
    } else if (auto lspec_decl = ::clang::dyn_cast<::clang::LinkageSpecDecl>(clang_decl)) {
        convert_linkage_spec(lspec_decl);
    } else {
        std::cerr << "UNKNOWN DECL:\n";
        clang_decl->dump();
    }
}


namespace_ * ast_converter::convert_ns(const ::clang::NamespaceDecl * clang_ns) {
    auto parent_ns = dynamic_cast<namespace_*>(ctx_);
    assert(parent_ns && "parent decl context for namespace is not a namespace");

    // getting existing or creating new namespace in code model
    namespace_ * ns = nullptr;
    if (clang_ns->getName().empty()) {
        ns = parent_ns->create_anon_namespace();
    } else {
        ns = parent_ns->get_or_create_namespace(clang_ns->getNameAsString());
    }

    // setting new decl context
    context_setter csetter{*this, ns, clang_ns};

    // adding namespace into map of entitites
    add_cm_entity(clang_ns, ns);

    // converting top level declarations in namespace
    for (auto && decl : clang_ns->decls()) {
        if (auto decl_ns = ::clang::dyn_cast<::clang::NamespaceDecl>(decl)) {
            convert_ns(decl_ns);
        } else {
            convert_decl(decl);
        }
    }

    return ns;
}


record_type * ast_converter::convert_record(const ::clang::RecordDecl * clang_record_decl) {
    // looking for existing CM context_entity associated with clang declaration
    auto rec = get_cm_entity_as<record_type>(clang_record_decl);
    if (rec == nullptr) {
        rec = create_new_record(clang_record_decl);
    }

    // filling record decl context
    assert(rec && "record type should not be null here");
    fill_record_contents(rec, clang_record_decl);
    return rec;
}


typedef_type * ast_converter::convert_typedef(const ::clang::TypedefNameDecl * clang_typedef_decl) {
    // looking for existing typedef
    if (auto td_type = get_cm_entity_as<typedef_type>(clang_typedef_decl)) {
        // TODO: check equality of new declaration with existing type
        return td_type;
    }

    auto clang_base_type = clang_typedef_decl->getUnderlyingType();
    auto base_type = convert_type(clang_base_type);

    // creating typedef type in namespace or record
    auto nm = clang_typedef_decl->getNameAsString();
    typedef_type * td_type = exec_ns_or_rec([nm, base_type, this](auto && ns) {
        return ns->create_typedef(nm, base_type);
    }, [nm, base_type, clang_typedef_decl, this](auto && rec) {
        return rec->create_typedef(nm, base_type, get_clang_decl_acc_spec(clang_typedef_decl));
    });

    td_type->set_loc(convert_loc(clang_typedef_decl->getCanonicalDecl()->getLocation()));

    add_cm_entity(clang_typedef_decl, td_type);
    return td_type;
}


function * ast_converter::convert_function(const ::clang::FunctionDecl * clang_func_decl) {
    // looking for existing function for declaration
    auto func = get_cm_entity_as<named_function>(clang_func_decl);

    if (func != nullptr) {
        // TODO: check equality of existing function type with another declaration
        return func;
    }

    // creating new function
    auto nm = clang_func_decl->getNameAsString();
    func = exec_ns_or_rec([nm](auto && ns) {
        return ns->create_function(nm);
    }, [nm, clang_func_decl](auto && rec) {
        auto acc = get_clang_decl_acc_spec(clang_func_decl);
        auto method_decl = ::clang::dyn_cast<::clang::CXXMethodDecl>(clang_func_decl);
        if (!method_decl || method_decl->isStatic()) {
            // static function
            return rec->create_function(nm, acc);
        } else {
            // intstance function
            return static_cast<named_function*>(rec->create_method(nm, acc));
        }
    });

    func->set_loc(convert_loc(clang_func_decl->getCanonicalDecl()->getLocation()));

    // adding function entity mapping
    add_cm_entity(clang_func_decl, func);

    // setting function as current decl context
    context_setter csetter{*this, func, clang_func_decl};

    // converting function return type and parameters
    convert_function_ret_type_and_params(func, clang_func_decl);

    return func;
}


variable * ast_converter::convert_variable(const ::clang::VarDecl * clang_var_decl) {
    // looking for existing variable for declaration
    auto var = get_cm_entity_as<variable>(clang_var_decl);
    if (var != nullptr) {
        // TODO: check equality of existing variable type
        return var;
    }

    // converting variable type
    auto var_type = convert_type(clang_var_decl->getType());

    // creating new variable
    auto nm = clang_var_decl->getNameAsString();
    var = exec_ns_or_rec([nm, var_type](auto && ns) {
        // variable in namespace
        return ns->create_var(nm, var_type);
    }, [nm, var_type, clang_var_decl](auto && rec) -> variable* {
        // static variable in record
        return rec->create_var(nm, var_type, get_clang_decl_acc_spec(clang_var_decl));
    });

    add_cm_entity(clang_var_decl, var);
    return var;
}


field * ast_converter::convert_field(cm::record * rec,
                                     const ::clang::FieldDecl * clang_field_decl) {
    // looking for existing variable for declaration
    auto var = get_cm_entity_as<field>(clang_field_decl);
    if (var != nullptr) {
        // TODO: check equality of existing variable type
        return var;
    }

    // converting variable type
    auto var_type = convert_type(clang_field_decl->getType());

    // creating new variable
    auto nm = clang_field_decl->getNameAsString();
    auto acc = get_clang_decl_acc_spec(clang_field_decl);
    var = rec->create_field(nm, var_type, acc);
    var->set_loc(convert_loc(clang_field_decl->getCanonicalDecl()->getLocation()));
    add_cm_entity(clang_field_decl, var);
    return var;
}


template_record *
ast_converter::convert_template_class(const ::clang::ClassTemplateDecl * clang_templ_decl) {
    auto clang_rec_decl = clang_templ_decl->getTemplatedDecl();

    // setter for setting current contexts
    context_setter csetter{*this};

    // looking for existing CM declaration associated with clang template declaration
    auto rec = get_cm_entity_as<template_record>(clang_rec_decl);
    if (rec != nullptr) {
        csetter.set(rec, clang_rec_decl);
    } else {
        auto clang_rec_def = clang_rec_decl;
        if (auto def = clang_rec_decl->getDefinition()) {
            clang_rec_def = ::clang::dyn_cast<::clang::CXXRecordDecl>(def);
        }

        assert(clang_rec_def != nullptr && "can't get clang template record definition");
        auto clang_templ_def = clang_rec_def->getDescribedTemplate();
        assert(clang_templ_def != nullptr && "can't get clang template class definition");

        // creating new template record
        auto knd = clang_tag_kind_to_record_kind(clang_rec_decl->getTagKind());
        auto trec = ctx_->create_template_record(clang_rec_decl->getNameAsString(), knd);
        trec->set_loc(convert_loc(clang_rec_def->getLocation()));
        trec->this_type()->set_loc(trec->loc());
        rec = trec;

        // adding clang decl -> record_type association
        add_cm_entity(clang_rec_decl, rec);

        // setting new context before converting tempalte parameters
        csetter.set(rec, clang_rec_decl);

        // converting template parameters
        auto clang_templ_pars = clang_templ_def->getTemplateParameters();
        convert_template_params(rec, clang_templ_pars);
    }

    // filling template record contents
    fill_record_contents(rec, clang_rec_decl);

    // converting template specializations only for canonical decl
    // (canonical decl is only one in all translation unit)
    if (clang_templ_decl->isCanonicalDecl()) {
        for (auto && spec : clang_templ_decl->specializations()) {
            convert_template_class_spec(rec, spec);
        }
    }

    return rec;
}


/// Converts class template partial specialization
template_record_partial_specialization * ast_converter::convert_template_partial_specialization(
        const ::clang::ClassTemplatePartialSpecializationDecl * clang_decl) {

    // skipping declaration without definition
    if (clang_decl->getDefinition() != clang_decl) {
        return nullptr;
    }

    // context setter for template specialization
    context_setter csetter{*this};

    // looking for existing CM entity associated with clang declaration
    auto spec = get_cm_entity_as<template_record_partial_specialization>(clang_decl);
    if (spec != nullptr) {
        // setting current decl context to existing CM entity
        csetter.set(spec, clang_decl);
    } else {
        // creating new CM entity for template specialization

        // getting code model tempalte record entity
        auto clang_templ_decl = clang_decl->getSpecializedTemplate()->getTemplatedDecl();
        assert(clang_templ_decl && "can't get clang template record decl for specialization");
        auto templ = get_cm_entity_as<template_record>(clang_templ_decl);
        assert(templ && "can't find template record for partial template specialization");

        // creating new template specialization record
        spec = templ->create_partial_specialization();

        // converting template parameters
        convert_template_params(spec, clang_decl->getTemplateParameters());

        // adding clang decl -> specialization association before converting template
        // arguments because they can depend on temple context
        add_cm_entity(clang_decl, spec);

        // setting current context before converting template arguments
        csetter.set(spec, clang_decl);

        // converting template arguments
        auto args = convert_template_arguments(clang_decl->getTemplateArgs().asArray());
        for (auto && arg : args) {
            spec->add_arg(arg);
        }
    }

    // filling template record contents
    fill_record_contents(spec, clang_decl);

    return spec;
}


record * ast_converter::convert_template_class_spec(
        cm::template_record * templ,
        const ::clang::ClassTemplateSpecializationDecl * clang_spec_decl) {

    // looking for existing CM declaration associated with clang declaration
    record * rec = get_cm_entity_as<template_record_instantiation_type>(clang_spec_decl);
    if (rec == nullptr) {
        auto knd = clang_tag_kind_to_record_kind(clang_spec_decl->getTagKind());
        auto clang_args = clang_spec_decl->getTemplateArgs().asArray();

        // creating new template instantiation, specialization or partial specialization
        if (auto p_spec = ::clang::dyn_cast<::clang::ClassTemplatePartialSpecializationDecl>(
                clang_spec_decl)) {
            assert(false && "should not reach here");
        } else if (clang_spec_decl->isExplicitSpecialization()) {
            auto args = convert_template_arguments(clang_args);
            rec = templ->create_specialization(args);
        } else {
            auto args = convert_template_arguments(clang_args);
            rec = templ->create_instantiation(args);
        }

        auto templ = clang_spec_decl->getSpecializedTemplate();
        rec->set_loc(convert_loc(templ->getLocation()));

        add_cm_entity(clang_spec_decl, rec);
    }

    // filling record content
    assert(rec && "record type should not be null here");
    fill_record_contents(rec, clang_spec_decl);

    return rec;
}


template_function *
ast_converter::convert_template_function(const ::clang::FunctionTemplateDecl * clang_decl) {
    auto clang_func_decl = clang_decl->getTemplatedDecl();
    auto clang_templ_pars = clang_decl->getTemplateParameters();

    // looking for existing CM declaration associated with clang template declaration
    auto func = get_cm_entity_as<template_function>(clang_func_decl);
    if (func != nullptr) {
        if (clang_decl->isThisDeclarationADefinition()) {
            // updating template parameters to match definition
            update_template_params(func, clang_templ_pars);

            // updating function parameters to match definition
            update_function_params(func, clang_func_decl);
        }

        // TODO: check equality of function declarations
        return func;
    }

    // creating new function
    auto nm = clang_func_decl->getNameAsString();
    func = exec_ns_or_rec([nm](auto && ns) {
        return ns->create_template_function(nm);
    }, [nm, clang_func_decl](auto && rec) {
        auto acc = get_clang_decl_acc_spec(clang_func_decl);
        auto method_decl = ::clang::dyn_cast<::clang::CXXMethodDecl>(clang_func_decl);
        if (!method_decl || method_decl->isStatic()) {
            // static function
            return rec->create_template_function(nm, acc);
        } else {
            // intstance function
            return static_cast<template_function*>(rec->create_template_method(nm, acc));
        }
    });

    // adding entity before converting template parameters, return type and
    // function parameters
    add_cm_entity(clang_func_decl, func);

    // setting current context to function
    context_setter csetter{*this, func, clang_func_decl};

    // converting template parameters
    convert_template_params(func, clang_templ_pars);

    // converting function return type and parameters
    convert_function_ret_type_and_params(func, clang_func_decl);

    // converting template specializations only for canonical decl
    // (canonical decl is only one in all translation unit)
    if (clang_func_decl->isCanonicalDecl()) {
        for (auto && spec : clang_decl->specializations()) {
            convert_template_function_inst(func, spec);
        }
    }

    return func;
}


template_function_instantiation *
ast_converter::convert_template_function_inst(cm::template_function * templ,
                                              const ::clang::FunctionDecl * clang_func) {
    assert(clang_func->isTemplateInstantiation() && "function is not a template instantiation");

    // looking for existing code model entity associated with clang declaration
    auto func = get_cm_entity_as<template_function_instantiation>(clang_func);
    if (func) {
        return func;
    }

    // converting template arguments
    auto clang_args = clang_func->getTemplateSpecializationArgs();
    assert(clang_args != nullptr && "no template arguments for function instantiation");
    auto args = convert_template_arguments(clang_args->asArray());

    // creating new template function instantiation
    func = templ->create_instantiation(args);

    // converting function return type and parameters
    convert_function_ret_type_and_params(func, clang_func);

    add_cm_entity(clang_func, func);

    return func;
}


void ast_converter::convert_linkage_spec(const ::clang::LinkageSpecDecl * decl) {
    // ignore linkage specification for now
    // TODO: implement in code model

    // converting nested declarations
    for (auto && nested_decl : decl->decls()) {
        if (auto nested_ns = ::clang::dyn_cast<::clang::NamespaceDecl>(nested_decl)) {
            convert_ns(nested_ns);
        } else {
            convert_decl(nested_decl);
        }
    }
}


source_location ast_converter::convert_loc(const ::clang::SourceLocation & loc) const {
    auto ploc = clang_ast_ctx_->getSourceManager().getPresumedLoc(loc);
    if (!ploc.isValid()) {
        return {};
    }

    auto file = mdl_.source(ploc.getFilename());
    return source_location{file, ploc.getLine(), ploc.getColumn()};
}


void ast_converter::fill_record_contents(cm::record * rec,
                                         const ::clang::RecordDecl * clang_record_decl) {
    // skipping declarations without definition
    if (!clang_record_decl->isCompleteDefinition()) {
        return;
    }

    // setting current declaration context
    context_setter csetter{*this, rec, clang_record_decl};

    // adding record bases
    if (auto clang_cxx_record_decl = ::clang::dyn_cast<::clang::CXXRecordDecl>(clang_record_decl)) {
        for (auto && base : clang_cxx_record_decl->bases()) {
            auto clang_base_type = base.getType().getTypePtr();
            auto base_type = convert_type(::clang::QualType{clang_base_type, 0});
            rec->add_base(base_type.type());
        }
    }

    // converting record members
    for (auto && decl : clang_record_decl->decls()) {
        // skipping "injected class name" records
        if (auto rec_decl = ::clang::dyn_cast<::clang::RecordDecl>(decl)) {
            if (rec_decl->isInjectedClassName()) {
                continue;
            }
        }

        if (auto acc_decl = ::clang::dyn_cast<::clang::AccessSpecDecl>(decl)) {
            // doing nothing for access specifiers
        } else if (auto fld_decl = ::clang::dyn_cast<::clang::FieldDecl>(decl)) {
            // field
            convert_field(rec, fld_decl);
        } else {
            // other declaration are converted via standard convert_decl
            convert_decl(decl);
        }
    }
}


void ast_converter::convert_template_params(templated_entity * templ,
                                            const ::clang::TemplateParameterList * clang_params) {
    templ->set_is_variadic(clang_params->hasParameterPack());

    for (auto && par : *clang_params) {
        // adding template parameter to code model template
        auto par_name = par->getNameAsString();
        template_parameter * cm_par = nullptr;
        if (auto clang_type_par = ::clang::dyn_cast<::clang::TemplateTypeParmDecl>(par)) {
            cm_par = templ->add_type_template_param(par_name);
        } else if (auto clang_val_par = ::clang::dyn_cast<::clang::NonTypeTemplateParmDecl>(par)) {
            auto par_type = convert_type(clang_val_par->getType());
            cm_par = templ->add_value_template_param(par_name, par_type.type());
        } else if (auto clang_templ_par = ::clang::dyn_cast<::clang::TemplateTemplateParmDecl>(par)) {
            // TODO: implement template template parameters
            cm_par = templ->add_type_template_param(par_name);
        
        // C++20 support in latest clang
        // } else if (auto clang_obj_par = ::clang::dyn_cast<::clang::TemplateParamObjectDecl>(par)) {
        //     // TODO: check if we need something special for template object parameters other
        //     // than name and type
        //     auto par_type = convert_type(clang_val_par->getType());
        //     rec->add_value_template_param(par_name, par_type.type());

        } else {
            assert(false && "Unknown template parameter type");
        }

        cm_par->set_loc(convert_loc(par->getLocation()));

        // ading entity map for template parameter
        add_cm_entity(par, cm_par);
    }
}


void ast_converter::update_template_params(templated_entity * templ,
                                           const ::clang::TemplateParameterList * clang_params) {
    auto templ_params = templ->template_params();
    auto par_it = std::ranges::begin(templ_params);

    for (auto && clang_par : *clang_params) {
        assert(par_it != std::ranges::end(templ_params) &&
               "template parameters inconsisteny between declarations");
        auto par = *par_it;

        par->ctx()->rename_entity(par, clang_par->getName());
        par->set_loc(convert_loc(clang_par->getLocation()));

        ++par_it;
    }
}


void ast_converter::convert_function_ret_type_and_params(
            function * func, const ::clang::FunctionDecl * clang_func) {
    // converting function return type
    func->set_ret_type(convert_type(clang_func->getReturnType()));

    // converting function parameters
    for (auto && par : clang_func->parameters()) {
        if (!par->getName().empty()) {
            func->add_param(par->getNameAsString(), convert_type(par->getType()));
        } else {
            func->add_param(convert_type(par->getType()));
        }
    }
}


void ast_converter::update_function_params(function * func,
                                           const ::clang::FunctionDecl * clang_func) {
    auto params = func->params();
    auto par_it = std::ranges::begin(params);

    for (auto && clang_par : clang_func->parameters()) {
        assert(par_it != std::ranges::end(params) && "function parameters inconsistency");
        auto named_par = dynamic_cast<named_function_parameter*>(*par_it);
        if (!named_par) {
            continue;
        }

        named_par->set_name(clang_par->getName());

        // TODO: support locations in function parameters
        //named_par->set_loc(convert_loc(clang_par->getLocation()));

        ++par_it;
    }
}


template_argument_desc_vector ast_converter::convert_template_arguments(
        const ::clang::ArrayRef<::clang::TemplateArgument> & args) {

    // converting template arguments
    template_argument_desc_vector res;
    for (unsigned i = 0, e = args.size(); i < e; ++i) {
        const auto & targ = args[i];

        switch (targ.getKind()) {
        case ::clang::TemplateArgument::ArgKind::Type: {
            res.push_back(convert_type(targ.getAsType()));
            break;
        }
        case ::clang::TemplateArgument::ArgKind::Integral: {
            ::llvm::SmallString<16> sstr;
            targ.getAsIntegral().toString(sstr, 10);
            res.push_back(value{sstr.str().str()});
            break;
        }
        default:
            assert(false && "Unknown template argument type");
        }
    }

    return res;
}


context_entity * ast_converter::get_cm_entity(const ::clang::Decl * clang_decl) {
    auto canon_decl = clang_decl->getCanonicalDecl();

    auto it = decls_.find(canon_decl);
    if (it == decls_.end()) {
        return nullptr;
    }

    return it->second;
}


void ast_converter::add_cm_entity(const ::clang::Decl * clang_decl, context_entity * cm_ent) {
    auto canon_decl = clang_decl->getCanonicalDecl();
    auto [it, inserted] = decls_.emplace(canon_decl, cm_ent);
    assert(inserted && "code model context_entity is already associated with clang declaration");
}


record_type * ast_converter::create_new_record(const ::clang::RecordDecl * clang_rec_decl) {
    // getting record kind
    auto knd = clang_tag_kind_to_record_kind(clang_rec_decl->getTagKind());

    // creating new record
    record_type * rec = nullptr;
    if (clang_rec_decl->getName().empty()) {
        rec = exec_ns_or_rec([knd, this](auto && ns) {
            return ns->create_record(knd);
        }, [knd, clang_rec_decl, this](auto && rec) {
            return rec->create_record(knd, get_clang_decl_acc_spec(clang_rec_decl));
        });
    } else {
        auto nm = clang_rec_decl->getNameAsString();
        rec = ctx_->find_named_entity<named_record_type>(nm);
        if (!rec) {
            rec = exec_ns_or_rec([nm, knd, this](auto && ns) {
                return ns->create_named_record(nm, knd);
            }, [nm, knd, clang_rec_decl, this](auto && rec) {
                return rec->create_named_record(nm, knd, get_clang_decl_acc_spec(clang_rec_decl));
            });
        }
    }

    const ::clang::TagDecl * clang_def_rec = clang_rec_decl->getDefinition();
    if (!clang_def_rec) {
        clang_def_rec = clang_rec_decl->getCanonicalDecl();
    }
    rec->set_loc(convert_loc(clang_def_rec->getSourceRange().getBegin()));

    // adding clang decl -> record_type association
    add_cm_entity(clang_rec_decl, rec);

    return rec;
}


const ::clang::TemplateTypeParmDecl *
ast_converter::find_type_template_parameter_decl(const ::clang::Decl * decl_ctx,
                                                 const ::clang::TemplateTypeParmType * type) {

    // auto c_decl_ctx = clang_ctx_;
    // while (true) {
    //     // getting list of template parameters depending on type of declaration context
    //     const ::clang::TemplateParameterList * pars = nullptr;
    //     if (auto spec =
    //             ::clang::dyn_cast<::clang::ClassTemplatePartialSpecializationDecl>(c_decl_ctx)) {
    //         pars = spec->getTemplateParameters();
    //     } else if (auto rec = ::clang::dyn_cast<::clang::RecordDecl>(c_decl_ctx)) {
    //         pars = rec->getDescribedTemplateParams();
    //     } else {
    //         // moving to the parent context
    //         c_decl_ctx = c_decl_ctx->getParent();
    //         continue;
    //     }

    //     // checking parameter list depnt
    //     assert(pars != nullptr && "invalid template parameter list");
    //     assert(pars->getDepth() >= type->getDepth() &&
    //            "missed template parameter list in parents chain");
    //     if (pars->getDepth() != type->getDepth()) {
    //         // moving to the parent context
    //         c_decl_ctx = c_decl_ctx->getParent();
    //         continue;
    //     }

    //     // getting parameter with index
    //     assert(type->getIndex() < pars->size() && "invalid template parameter index");
    //     return pars->getParam(type->getIndex());
    // }

    // // checking for equality of template deth for type and template parameter list
    // auto params = templ->getTemplateParameters();
    // assert(params != nullptr && "invalid clang template parameters");
    // if (params->getDepth() == type->getDepth()) {
    //     assert(type->getIndex() < params->size() && "invalid template parameter type index");
    //     auto decl = params->getParam(type->getIndex());
    //     auto type_decl = ::clang::dyn_cast<const ::clang::TemplateTypeParmDecl>(decl);
    //     assert(type_decl && "invalid type template parameter declaration");
    // }

    // searching for parent template

    return nullptr;
}


}
