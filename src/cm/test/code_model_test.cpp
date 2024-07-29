// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file code_model_test.cpp
/// Contains unit tests for the code_model class.

#include "pch.hpp"
#include "cm/code_model.hpp"
#include <boost/test/unit_test.hpp>


namespace cm {


struct code_model_test_fixture {
    code_model cm;
};


BOOST_FIXTURE_TEST_SUITE(code_model_test, code_model_test_fixture)


/// Tests creating pointer type
BOOST_AUTO_TEST_CASE(test_create_ptr) {
    auto res = cm.get_or_create_ptr_type(cm.bt_int());
    BOOST_REQUIRE(res != nullptr);

    BOOST_CHECK(!res->base().is_const());
    BOOST_CHECK(!res->base().is_volatile());
    BOOST_REQUIRE(res->base().type());

    BOOST_CHECK(res->base().type() == cm.bt_int());
}


///// Tests getting previously created pointer type
//BOOST_AUTO_TEST_CASE(test_get_ptr) {
//    auto ptr = cm.get_or_create_ptr_type(cm.bt_int());
//    auto ptr2 = cm.get_or_create_ptr_type(cm.bt_int());
//    auto ptr3 = cm.ptr_type(cm.bt_int());
//    BOOST_CHECK(ptr == ptr2);
//    BOOST_CHECK(ptr == ptr3);
//}


/// Tests creating different pointers
BOOST_AUTO_TEST_CASE(test_ptr_diff) {
    auto ptr = cm.get_or_create_ptr_type(cm.bt_int());
    auto ptr2 = cm.get_or_create_ptr_type(cm.bt_char());
    BOOST_CHECK(ptr != ptr2);
}


///// Tests throwing error when trying get pointer that does not exist
//BOOST_AUTO_TEST_CASE(test_ptr_error) {
//    try {
//        auto ptr = cm.ptr_type(cm.bt_char());
//    }
//    catch(std::runtime_error & err) {
//        return;
//    }

//    BOOST_CHECK(false);
//}


/// Tests creating reference type
BOOST_AUTO_TEST_CASE(test_create_ref) {
    auto res = cm.get_or_create_lvalue_ref_type(cm.bt_int());

    BOOST_REQUIRE(res != nullptr);

    BOOST_CHECK(!res->base().is_const());
    BOOST_CHECK(!res->base().is_volatile());
    BOOST_REQUIRE(res->base().type());

    BOOST_CHECK(res->base().type() == cm.bt_int());
}


///// Tests getting previously created pointer type
//BOOST_AUTO_TEST_CASE(test_get_ref) {
//    auto ref = cm.get_or_create_lvalue_ref_type(cm.bt_int());
//    auto ref2 = cm.get_or_create_lvalue_ref_type(cm.bt_int());
//    auto ref3 = cm.ref_type(cm.bt_int());
//    BOOST_CHECK(ref == ref2);
//    BOOST_CHECK(ref == ref3);
//}


/// Tests creating different references
BOOST_AUTO_TEST_CASE(test_ref_diff) {
    auto ref = cm.get_or_create_lvalue_ref_type(cm.bt_int());
    auto ref2 = cm.get_or_create_lvalue_ref_type(cm.bt_char());
    BOOST_CHECK(ref != ref2);
}


///// Tests throwing error when trying get reference that does not exist
//BOOST_AUTO_TEST_CASE(test_ref_error) {
//    try {
//        auto ptr = cm.ref_type(cm.bt_char());
//    }
//    catch(std::runtime_error & err) {
//        return;
//    }

//    BOOST_CHECK(false);
//}


/// Tests creating pointers with qualifiers
BOOST_AUTO_TEST_CASE(test_create_ptr_const) {
    auto int_type = cm.bt_int();
    auto ptr1 = cm.get_or_create_ptr_type(int_type);
    auto ptr2 = cm.get_or_create_ptr_type({int_type, true, false});
    auto ptr3 = cm.get_or_create_ptr_type({int_type, false, true});
    auto ptr4 = cm.get_or_create_ptr_type({int_type, true, true});
    BOOST_REQUIRE(ptr1 != nullptr);
    BOOST_REQUIRE(ptr2 != nullptr);
    BOOST_REQUIRE(ptr3 != nullptr);
    BOOST_REQUIRE(ptr4 != nullptr);

    BOOST_CHECK(ptr1 != ptr2);
    BOOST_CHECK(ptr1 != ptr3);
    BOOST_CHECK(ptr1 != ptr4);

    BOOST_CHECK(ptr2 != ptr3);
    BOOST_CHECK(ptr2 != ptr4);

    BOOST_CHECK(ptr3 != ptr4);
}


/// Tests creating typedef in global namespace
BOOST_AUTO_TEST_CASE(test_create_typedef_global) {
    auto tt = cm.create_typedef("my_int", cm.bt_int());
    BOOST_CHECK_EQUAL(tt->name(), "my_int");
}


/// Tests creating array type
BOOST_AUTO_TEST_CASE(create_arr) {
    auto at = cm.get_or_create_arr_type(cm.bt_int(), 20);
    BOOST_REQUIRE(at);
    BOOST_CHECK(at->base() == cm.bt_int());
    BOOST_CHECK_EQUAL(at->size(), 20);
}


/// Tests creating existing array type
BOOST_AUTO_TEST_CASE(create_arr_existing) {
    auto at = cm.get_or_create_arr_type(cm.bt_int(), 20);
    auto at2 = cm.get_or_create_arr_type(cm.bt_int(), 20);
    BOOST_CHECK(at == at2);
}


/// Tests creating array types with different sizes
BOOST_AUTO_TEST_CASE(create_arr_diff_size) {
    auto at = cm.get_or_create_arr_type(cm.bt_int(), 20);
    auto at2 = cm.get_or_create_arr_type(cm.bt_int(), 30);
    BOOST_CHECK(at != at2);
    BOOST_REQUIRE(at2);
    BOOST_CHECK_EQUAL(at2->size(), 30);
}


/// Tests creating function type
BOOST_AUTO_TEST_CASE(create_function) {
    auto ftype = cm.get_or_create_func_type(cm.bt_int(),
                                            qual_type{cm.bt_float(), true},
                                            cm.bt_short());
    BOOST_REQUIRE(ftype);
    BOOST_CHECK(ftype->ret_type() == qual_type{cm.bt_int()});
    BOOST_CHECK((ftype->params()[0] == qual_type{cm.bt_float(), true}));
    BOOST_CHECK((ftype->params()[1] == qual_type{cm.bt_short()}));
}


/// Tests creating existing function type
BOOST_AUTO_TEST_CASE(create_func_existing) {
    auto ftype = cm.get_or_create_func_type({cm.bt_int(), false, true},
                                            cm.bt_float(),
                                            cm.bt_short());

    auto ftype2 = cm.get_or_create_func_type({cm.bt_int(), false, true},
                                             cm.bt_float(),
                                             cm.bt_short());

    BOOST_CHECK(ftype);
    BOOST_CHECK(ftype == ftype2);
}


/// Tests creating function types with different CVs
BOOST_AUTO_TEST_CASE(create_func_diff_cv) {
    auto ftype = cm.get_or_create_func_type({cm.bt_int(), false, true},
                                            cm.bt_float(),
                                            cm.bt_short());

    auto ftype2 = cm.get_or_create_func_type({cm.bt_int(), false, true},
                                             qual_type{cm.bt_float(), true},
                                             cm.bt_short());

    BOOST_CHECK(ftype);
    BOOST_CHECK(ftype2);
    BOOST_CHECK(ftype != ftype2);
}


/// Tests creating pointer to function type
BOOST_AUTO_TEST_CASE(create_func_ptr_type) {
    auto ftype = cm.get_or_create_func_type(cm.bt_void());
    auto fptr = cm.get_or_create_ptr_type(ftype);
}


/// Tests creating template instantiation
BOOST_AUTO_TEST_CASE(create_templ_inst) {
    auto templ = cm.create_named_entity<template_record>("my_templ", record_kind::struct_);
    templ->add_type_template_param("T");
    auto inst = templ->create_instantiation(cm.bt_int());

    BOOST_CHECK(inst->templ() == templ);
    BOOST_REQUIRE_EQUAL(inst->args().size(), 1);
    auto arg = inst->args()[0];
    auto t_arg = dynamic_cast<type_template_argument*>(arg);
    BOOST_REQUIRE(t_arg);
    BOOST_CHECK(t_arg->type() == cm::qual_type{cm.bt_int()});
}


/// Tests creating base-recursive template instantiations
BOOST_AUTO_TEST_CASE(create_templ_inst_recursive) {
    auto rec = cm.create_named_record("my_record");
    auto base_templ = cm.create_named_entity<template_record>("base", record_kind::struct_);
    base_templ->add_type_template_param("T");
    auto base_inst = base_templ->create_instantiation(rec);
    rec->add_base(base_inst);
}


/// Tests creating record type with field of type of pointer to record itself
BOOST_AUTO_TEST_CASE(create_rec_ptr_recursive) {
    auto rec = cm.create_named_record("rec");
    auto ptr = cm.get_or_create_ptr_type(rec);
    rec->create_field("fld", ptr, access_level::public_);
}


/// Tests creating template with template instantiation parameter
BOOST_AUTO_TEST_CASE(create_templ_inst_templ_inst) {
    auto std = cm.get_or_create_namespace("std");
    auto placeholder = std->get_or_create_template_record("_Placeholder",
                                                    cm::record_kind::class_,
                                                    false,
                                                    std::make_tuple("N", cm.bt_int()));
    auto placeholder_inst = placeholder->create_instantiation(cm::value{1});

    auto foo = std->get_or_create_template_record("foo", cm::record_kind::class_, false, "T");
    auto foo_inst = foo->create_instantiation(placeholder_inst);
}


/// Tests basic replacing of all type uses
BOOST_AUTO_TEST_CASE(replace_type) {
    auto rec1 = cm.create_named_record("rec1");
    auto ptr = cm.get_or_create_ptr_type(rec1);    
    auto rec2 = cm.create_named_record("rec2");

    auto str = cm.create_named_record("str");
    auto ivar = str->create_field("x", ptr);

    cm.replace_type(rec1, rec2);

    auto ptr2 = ivar->type().cast<pointer_type>();
    BOOST_CHECK(ptr2.type() != ptr);
    BOOST_CHECK(ptr2->base().type() == rec2);
}


/// Tests creating record with base
BOOST_AUTO_TEST_CASE(record_base) {
    auto rec = cm.create_named_record("rec");
    auto base = cm.create_named_record("base");
    rec->add_base(base);
}


/// Tests creating record with base in namespace
BOOST_AUTO_TEST_CASE(reb_base_namespace) {
    auto ns = cm.get_or_create_namespace("ns");
    auto base = ns->create_named_record("base");
    auto rec = ns->create_named_record("rec");
    rec->add_base(base);
}


BOOST_AUTO_TEST_SUITE_END()


}
