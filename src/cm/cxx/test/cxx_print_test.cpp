// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file cxx_print_test.cpp
/// Contains unit tests for pretty printing of declarations for C++ language

#include "../code_model.hpp"
#include "../cxx/print.hpp"
#include <sstream>
#include <boost/test/unit_test.hpp>


namespace cm::test {


struct cxx_print_test_fixture {
    code_model cm;
    std::ostringstream str;
};


BOOST_FIXTURE_TEST_SUITE(cxx_print_test, cxx_print_test_fixture)


/// Tests printing user defined qualified type
BOOST_AUTO_TEST_CASE(qual_type_name_write_user) {
    auto rec = cm.create_named_record("cls");
    qual_type qt{rec, true, false};
    cxx::print_type(str, qt);

    BOOST_CHECK_EQUAL(str.str(), "const cls");
}


/// Tests printing builtin qualified type
BOOST_AUTO_TEST_CASE(qual_type_name_write_builtin) {
    qual_type t{cm.bt_int(), true};
    cxx::print_type(str, t);
    BOOST_CHECK_EQUAL(str.str(), "int const");
}


/// Tests printing record type
BOOST_AUTO_TEST_CASE(record_type_name_write) {
    auto rtype = cm.create_named_record("type");
    cxx::print_type(str, rtype);
    BOOST_CHECK_EQUAL(str.str(), "type");
}


/// Tests printing record with scope
BOOST_AUTO_TEST_CASE(record_type_name_write_scope) {
    auto ns = cm.create_namespace("std");
    auto rec = ns->create_named_record("string");
    cxx::print_type(str, rec);
    BOOST_CHECK_EQUAL(str.str(), "std::string");
}


/// Tests printing array type
BOOST_AUTO_TEST_CASE(array_type_name_write) {
    auto arr = cm.get_or_create_arr_type(cm.bt_int(), 10);
    cxx::print_type(str, arr);
    BOOST_CHECK_EQUAL(str.str(), "int [10]");
}


/// Tests printing qualified array of user defined types
BOOST_AUTO_TEST_CASE(array_type_write_const_user) {
    auto rec = cm.create_named_record("cls");
    auto arr = cm.get_or_create_arr_type(rec, 10);
    qual_type qt{arr, true};
    cxx::print_type(str, qt);
    BOOST_CHECK_EQUAL(str.str(), "const cls [10]");
}


/// Tests printing qualified array of builtin types
BOOST_AUTO_TEST_CASE(array_type_write_const_builtin) {
    auto arr = cm.get_or_create_arr_type(cm.bt_int(), 10);
    qual_type qt{arr, true};
    cxx::print_type(str, qt);
    BOOST_CHECK_EQUAL(str.str(), "int const [10]");
}


/// Tests printing pointer type name to output stream
BOOST_AUTO_TEST_CASE(pointer_type_name_write) {
    auto rec = cm.create_named_record("foo");
    auto ptr = cm.get_or_create_ptr_type(rec);
    cxx::print_type(str, ptr);
    BOOST_CHECK_EQUAL(str.str(), "foo*");
}


/// Tests printing pointer to array type name
BOOST_AUTO_TEST_CASE(pointer_type_name_write_array) {
    auto arr = cm.get_or_create_arr_type(cm.bt_int(), 20);
    auto ptr = cm.get_or_create_ptr_type(arr);
    cxx::print_type(str, ptr);
    BOOST_CHECK_EQUAL(str.str(), "int (*)[20]");
}


/// Tests printing pointer to qualified array of user defined types
BOOST_AUTO_TEST_CASE(pointer_type_name_write_array_const_user) {
    auto rec = cm.create_named_record("cls");
    auto arr = cm.get_or_create_arr_type(rec, 20);
    auto ptr = cm.get_or_create_ptr_type({arr, true});
    cxx::print_type(str, ptr);
    BOOST_CHECK_EQUAL(str.str(), "const cls (*)[20]");
}


/// Tests printing pointer to qualified array of builtin types
BOOST_AUTO_TEST_CASE(pointer_type_name_write_array_const_builtin) {
    auto arr = cm.get_or_create_arr_type(cm.bt_int(), 20);
    auto ptr = cm.get_or_create_ptr_type({arr, true});
    cxx::print_type(str, ptr);
    BOOST_CHECK_EQUAL(str.str(), "int const (*)[20]");
}


/// Tests printing reference type name to output stream
BOOST_AUTO_TEST_CASE(reference_type_name_write) {
    auto rec = cm.create_named_record("foo");
    auto ref = cm.get_or_create_lvalue_ref_type(rec);
    cxx::print_type(str, ref);
    BOOST_CHECK_EQUAL(str.str(), "foo&");
}


/// Tests printing template type name to output stream
BOOST_AUTO_TEST_CASE(template_type_name) {
    auto templ = cm.create_named_entity<template_record>("vector", record_kind::struct_);
    templ->add_type_template_param("T");
    auto inst = templ->create_instantiation(cm.bt_int());
    cxx::print_type(str, inst);
    BOOST_CHECK_EQUAL(str.str(), "vector<int>");
}


/// Tests printing template type name in namespace to output stream
BOOST_AUTO_TEST_CASE(template_type_name_namespace) {
    auto ns = cm.create_namespace("std");
    auto templ = ns->create_named_entity<template_record>("vector", record_kind::struct_);
    templ->add_type_template_param("T");
    auto inst = templ->create_instantiation(cm.bt_int());
    cxx::print_type(str, inst);
    BOOST_CHECK_EQUAL(str.str(), "std::vector<int>");
}


/// Tests printing template type name with template parameter
BOOST_AUTO_TEST_CASE(template_type_name_write_templ) {
    auto templ2 = cm.create_named_entity<template_record>("foo", record_kind::struct_);
    templ2->add_type_template_param("T");

    auto templ = cm.create_named_entity<template_record>("vector", record_kind::struct_);
    templ->add_type_template_param("T");

    auto templ2_inst = templ2->create_instantiation(cm.bt_int());
    auto templ_inst = templ->create_instantiation(templ2_inst);

    cxx::print_type(str, templ_inst);

    BOOST_CHECK_EQUAL(str.str(), "vector<foo<int> >");
}


/// Tests printing function type name
BOOST_AUTO_TEST_CASE(function_type_name_write) {
    auto ftype = cm.get_or_create_func_type(cm.bt_int(), cm.bt_int(), cm.bt_float());
    cxx::print_type(str, ftype);
    BOOST_CHECK_EQUAL(str.str(), "int(int, float)");
}


/// Tests printing member pointer type name
BOOST_AUTO_TEST_CASE(mem_ptr_type_name_write) {
    auto rtype = cm.create_named_record("cls");
    auto mtype = cm.get_or_create_mem_ptr_type(rtype, cm.bt_int());
    cxx::print_type(str, mtype);
    BOOST_CHECK_EQUAL(str.str(), "int cls::*");
}


/// Tests printing member function pointer type name
BOOST_AUTO_TEST_CASE(mem_func_ptr_type_name_write) {
    auto rtype = cm.create_named_record("cls");
    auto ftype = cm.get_or_create_func_type(cm.bt_int(), cm.bt_int(), cm.bt_float());
    auto mtype = cm.get_or_create_mem_ptr_type(rtype, ftype);

    cxx::print_type(str, mtype);
    BOOST_CHECK_EQUAL(str.str(), "int (cls::*)(int, float)");
}


/// Tests printing pointer to array of poiners to arrays
BOOST_AUTO_TEST_CASE(write_ptr_arr_ptr_arr) {
    auto ar = cm.get_or_create_arr_type(cm.bt_int(), 30);
    auto ptr = cm.get_or_create_ptr_type(ar);
    auto ar2 = cm.get_or_create_arr_type(ptr, 20);
    auto ptr2 = cm.get_or_create_ptr_type(ar2);

    cxx::print_type(str, ptr2);
    BOOST_CHECK_EQUAL(str.str(), "int (* (*)[20])[30]");
}


/// Tests printing pointer to function
BOOST_AUTO_TEST_CASE(write_func_ptr) {
    auto ftype = cm.get_or_create_func_type(cm.bt_int());
    auto ptr_type = cm.get_or_create_ptr_type(ftype);

    cxx::print_type(str, ptr_type);
    BOOST_CHECK_EQUAL(str.str(), "int (*)()");
}


/// Tests printing pointer to function returning pointer
BOOST_AUTO_TEST_CASE(write_func_ptr_ret_ptr) {
    auto ret_type = cm.get_or_create_ptr_type(cm.bt_int());
    auto func_type = cm.get_or_create_func_type(ret_type);
    auto ptr_type = cm.get_or_create_ptr_type(func_type);

    cxx::print_type(str, ptr_type);
    BOOST_CHECK_EQUAL(str.str(), "int* (*)()");
}


/// Tests printing function type
BOOST_AUTO_TEST_CASE(write_func) {
    auto ft = cm.get_or_create_func_type(cm.bt_int());

    cxx::print_type(str, ft);
    BOOST_CHECK_EQUAL(str.str(), "int()");
}


/// Tests printing template type name with 2 parameters
BOOST_AUTO_TEST_CASE(template_type_name_write_2) {
    auto t2 = cm.create_named_entity<template_record>("foo", record_kind::struct_);
    t2->add_type_template_param("T");
    auto t = cm.create_named_entity<template_record>("vector", record_kind::struct_);
    t->add_type_template_param("T1");
    t->add_type_template_param("T2");

    auto t2_inst = t2->create_instantiation(cm.bt_int());
    auto t_inst = t->create_instantiation(t2_inst, cm.bt_void());

    cxx::print_type(str, t_inst);
    BOOST_CHECK_EQUAL(str.str(), "vector<foo<int>, void>");
}


/// Tests printing array of pointers to functions
BOOST_AUTO_TEST_CASE(func_ptr_array) {
    auto ftype = cm.get_or_create_func_type(cm.bt_void());
    auto fptr_type = cm.get_or_create_ptr_type(ftype);
    auto arr_type = cm.get_or_create_arr_type(fptr_type, 10);

    cxx::print_type(str, arr_type);
    BOOST_CHECK_EQUAL(str.str(), "void (* [10])()");
}


/// Tests printing function returnin"g pointer to function
BOOST_AUTO_TEST_CASE(func_ret_func) {
    auto f_ret_type = cm.get_or_create_func_type(cm.bt_void());
    auto fptr_ret_type = cm.get_or_create_ptr_type(f_ret_type);
    auto f_type = cm.get_or_create_func_type(fptr_ret_type);

    cxx::print_type(str, f_type);
    BOOST_CHECK_EQUAL(str.str(), "void (*())()");
}


/// Tests printing template function
BOOST_AUTO_TEST_CASE(templ_func) {
    auto templ = cm.create_template_function("foo");
    templ->add_type_template_param("T");
    auto func = templ->create_instantiation(cm.bt_int());
    func->set_ret_type(cm.bt_void());

    cxx::print_entity(str, func);
    BOOST_CHECK_EQUAL(str.str(), "void foo<int>()");
}


/// Tests printing function with named parameters
BOOST_AUTO_TEST_CASE(func_named_params) {
    auto func = cm.create_function("func");
    func->set_ret_type(cm.bt_void());
    func->add_param("x", cm.bt_int());

    cxx::print_entity(str, func);
    BOOST_CHECK_EQUAL(str.str(), "void func(int x)");
}


BOOST_AUTO_TEST_SUITE_END()


}
