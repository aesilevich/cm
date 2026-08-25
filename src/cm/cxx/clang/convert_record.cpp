// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file convert_record.hpp
/// Contains implementations of functions for converting clang records to code model records.

#include "pch.hpp"

#include "convert_record.hpp"
#include "convert_loc.hpp"
#include "log.hpp"
#include "utils.hpp"

#include <clang/AST/DeclCXX.h>


namespace cm::cxx::clang {


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


access_level get_clang_decl_acc_level(const ::clang::Decl * decl) {
    switch (decl->getAccess()) {
    case ::clang::AS_none:
    case ::clang::AS_public:
        return access_level::public_;
    case ::clang::AS_protected:
        return access_level::protected_;
    case ::clang::AS_private:
        return access_level::private_;
    default:
        assert(false && "unknown clang access level");
        return access_level::public_;
    }
}


record_type * create_new_record(converter_impl & conv,
                                context * parent_ctx,
                                const ::clang::RecordDecl * clang_rec_decl) {
    CM_CLANG_LOG_SCAT_DECL(decl, "creating new record for decl", clang_rec_decl);

    // getting record kind
    auto knd = clang_tag_kind_to_record_kind(clang_rec_decl->getTagKind());

    // creating new record
    record_type * rec = nullptr;
    if (clang_rec_decl->getName().empty()) {
        rec = parent_ctx->create_record(knd);
    } else {
        auto nm = clang_rec_decl->getNameAsString();
        rec = parent_ctx->create_named_record(nm, knd);
    }

    rec->set_access_lev(get_clang_decl_acc_level(clang_rec_decl));

    const ::clang::TagDecl * clang_def_rec = clang_rec_decl->getDefinition();
    if (!clang_def_rec) {
        clang_def_rec = clang_rec_decl->getCanonicalDecl();
    }
    rec->set_loc(convert_loc(conv, clang_def_rec->getSourceRange().getBegin()));

    return rec;
}


static field * convert_field(converter_impl & conv,
                             cm::record * rec,
                             const ::clang::FieldDecl * clang_field_decl) {
    // looking for existing variable for declaration
    auto var = conv.get_decl_entity_as<field>(clang_field_decl);
    if (var != nullptr) {
        // TODO: check equality of existing variable type
        return var;
    }

    // converting variable type
    auto var_type = conv.type(clang_field_decl->getType());

    // creating new variable
    auto nm = clang_field_decl->getNameAsString();
    auto acc = get_clang_decl_acc_level(clang_field_decl);
    var = rec->create_field(nm, var_type, acc);
    var->set_loc(convert_loc(conv, clang_field_decl->getCanonicalDecl()->getLocation()));
    conv.add_decl_entity(clang_field_decl, var);
    return var;
}


void fill_record_contents(converter_impl & conv,
                          cm::record * rec,
                          const ::clang::RecordDecl * clang_record_decl) {

    // skipping declarations without definition
    if (!clang_record_decl->isCompleteDefinition()) {
        return;
    }

    // setting current declaration context
    converter_impl::decl_context_setter csetter{conv, rec, clang_record_decl};

    // adding record bases
    if (auto clang_cxx_record_decl = ::clang::dyn_cast<::clang::CXXRecordDecl>(clang_record_decl)) {
        for (auto && base : clang_cxx_record_decl->bases()) {
            auto clang_base_type = base.getType().getTypePtr();
            auto base_type = conv.type(clang_base_type);
            rec->add_base(base_type);
        }
    }

    // converting record members
    for (auto && decl : clang_record_decl->decls()) {
        // skipping "injected class name" records
        if (auto rec_decl = ::clang::dyn_cast<::clang::CXXRecordDecl>(decl)) {
            if (rec_decl->isInjectedClassName()) {
                continue;
            }
        }

        if (auto acc_decl = ::clang::dyn_cast<::clang::AccessSpecDecl>(decl)) {
            // doing nothing for access levels
        } else if (auto fld_decl = ::clang::dyn_cast<::clang::FieldDecl>(decl)) {
            // field
            convert_field(conv, rec, fld_decl);
        } else {
            // other declaration are converted via standard convert_decl
            conv.convert_decl(decl);
        }
    }
}


record_type * convert_record(converter_impl & conv, const ::clang::RecordDecl * clang_record_decl) {
    // getting existing or creating new record entity
    auto rec = conv.decl_entity_as<record_type>(clang_record_decl);

    // filling record contents
    assert(rec && "record type should not be null here");
    fill_record_contents(conv, rec, clang_record_decl);
    return rec;
}


}
