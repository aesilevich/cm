// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file parse_simple_tests.cpp
/// Contains unit tests for parsing simple source files

#include "parse_fixture.hpp"
#include "cm/cxx/clang/cmclang.hpp"
#include <boost/test/unit_test.hpp>
#include <ranges>


namespace fs = std::filesystem;


namespace cm::clang::test {


BOOST_FIXTURE_TEST_SUITE(parse_simple_tests, parse_fixture)


/// Tests parsing function
BOOST_AUTO_TEST_CASE(func) {
    parse_source_file(mdl, test_src_path() / "func.cpp", {});

    auto fn = dynamic_cast<named_function*>(mdl.find_named_entity("foo"));
    BOOST_REQUIRE(fn);

    auto fn_type = mdl.get_or_create_func_type(fn);
    BOOST_REQUIRE(fn_type);
    BOOST_REQUIRE(!fn_type->ret_type().is_null());
    BOOST_CHECK(!fn_type->ret_type().is_const());
    BOOST_CHECK(!fn_type->ret_type().is_volatile());
    BOOST_CHECK(fn_type->ret_type().ctype() == mdl.bt_float());
}


/// Tests parsing global variable
BOOST_AUTO_TEST_CASE(var) {
    parse_source_file(mdl, test_src_path() / "var.cpp", {});

    auto mdl_vars = mdl.vars();
    BOOST_REQUIRE_EQUAL(std::ranges::distance(mdl_vars), 1);
    auto var = *std::ranges::begin(mdl_vars);
    BOOST_REQUIRE(var);
    BOOST_CHECK_EQUAL(var->name(), "my_var");

    auto var_type = var->type();
    BOOST_REQUIRE(!var_type.is_null());
    BOOST_CHECK(var_type.is_const());
    BOOST_CHECK(!var_type.is_volatile());
    BOOST_CHECK(var_type.ctype() == mdl.bt_int());
}


/// Tests parsing namespace
BOOST_AUTO_TEST_CASE(parse_namespace) {
    parse_source_file(mdl, test_src_path() / "namespace.cpp", {});

    BOOST_REQUIRE_EQUAL(std::ranges::size(mdl.namespaces()), 1);
    auto ns = *std::ranges::begin(mdl.namespaces());
    BOOST_REQUIRE(ns);
    BOOST_CHECK_EQUAL(ns->name(), "foo");

    BOOST_REQUIRE_EQUAL(std::ranges::size(ns->namespaces()), 1);
    auto ns2 = *std::ranges::begin(ns->namespaces());
    BOOST_REQUIRE(ns2);
    BOOST_CHECK_EQUAL(ns2->name(), "bar");
}


/// Tests parsing typedef
BOOST_AUTO_TEST_CASE(parse_typedef) {
    parse_source_file(mdl, test_src_path() / "typedef.cpp", {});

    auto n_type = mdl.find_named_entity("my_type");
    BOOST_REQUIRE(n_type);

    auto td_type = dynamic_cast<typedef_type*>(n_type);
    BOOST_REQUIRE(td_type);

    BOOST_CHECK(!td_type->base().is_null());
    BOOST_CHECK(!td_type->base().is_const());
    BOOST_CHECK(!td_type->base().is_volatile());
    BOOST_CHECK(td_type->base().ctype() == mdl.bt_int());
}


/// Tests parsing class definition
BOOST_AUTO_TEST_CASE(parse_class) {
    parse_source_file(mdl, test_src_path() / "class.cpp", {});

    auto n_type = mdl.find_named_entity("my_class");
    BOOST_REQUIRE(n_type);

    auto rtype = dynamic_cast<named_record_type*>(n_type);
    BOOST_REQUIRE(rtype);

    BOOST_CHECK_EQUAL(rtype->name(), "my_class");
    BOOST_CHECK(rtype->kind() == record_kind::class_);
    BOOST_CHECK_EQUAL(std::ranges::size(rtype->bases()), 0);
    BOOST_CHECK_EQUAL(std::ranges::distance(rtype->fields()), 0);
}


/// Tests parsing struct definition
BOOST_AUTO_TEST_CASE(parse_struct) {
    parse_source_file(mdl, test_src_path() / "struct.cpp", {});

    auto n_type = mdl.find_named_entity("my_struct");
    BOOST_REQUIRE(n_type);

    auto rtype = dynamic_cast<named_record_type*>(n_type);
    BOOST_REQUIRE(rtype);

    BOOST_CHECK_EQUAL(rtype->name(), "my_struct");
    BOOST_CHECK(rtype->kind() == record_kind::struct_);
    BOOST_CHECK_EQUAL(std::ranges::size(rtype->bases()), 0);
    BOOST_CHECK_EQUAL(std::ranges::distance(rtype->fields()), 0);
}


/// Tests parsing union definition
BOOST_AUTO_TEST_CASE(parse_union) {
    parse_source_file(mdl, test_src_path() / "union.cpp", {});

    auto n_type = mdl.find_named_entity("my_union");
    BOOST_REQUIRE(n_type);

    auto rtype = dynamic_cast<named_record_type*>(n_type);
    BOOST_REQUIRE(rtype);

    BOOST_CHECK_EQUAL(rtype->name(), "my_union");
    BOOST_CHECK(rtype->kind() == record_kind::union_);
    BOOST_CHECK_EQUAL(std::ranges::size(rtype->bases()), 0);
    BOOST_CHECK_EQUAL(std::ranges::distance(rtype->fields()), 0);
}


/// Tests parsing class static variable
BOOST_AUTO_TEST_CASE(parse_class_static_var) {
    parse_source_file(mdl, test_src_path() / "class_static_var.cpp", {});

    auto n_type = mdl.find_named_entity("my_class");
    BOOST_REQUIRE(n_type);

    auto rtype = dynamic_cast<named_record_type*>(n_type);
    BOOST_REQUIRE(rtype);

    BOOST_CHECK_EQUAL(rtype->name(), "my_class");
    BOOST_CHECK(rtype->kind() == record_kind::class_);
    BOOST_CHECK_EQUAL(std::ranges::size(rtype->bases()), 0);

    auto rtype_vars = rtype->vars();
    BOOST_REQUIRE_EQUAL(std::ranges::distance(rtype_vars), 1);
    auto var = *std::ranges::begin(rtype_vars);
    BOOST_REQUIRE(var);
    BOOST_CHECK_EQUAL(var->name(), "x");
    BOOST_CHECK(rtype->access_spec(var) == member_access_spec::protected_);
}


/// Tests parsing class field
BOOST_AUTO_TEST_CASE(parse_class_var) {
    parse_source_file(mdl, test_src_path() / "field.cpp", {});

    auto n_type = mdl.find_named_entity("my_class");
    BOOST_REQUIRE(n_type);

    auto rtype = dynamic_cast<named_record_type*>(n_type);
    BOOST_REQUIRE(rtype);

    BOOST_CHECK_EQUAL(rtype->name(), "my_class");
    BOOST_CHECK(rtype->kind() == record_kind::class_);
    BOOST_CHECK_EQUAL(std::ranges::size(rtype->bases()), 0);
    BOOST_CHECK_EQUAL(std::ranges::distance(rtype->fields()), 1);

    auto rtype_ivars = rtype->fields();
    BOOST_REQUIRE_EQUAL(std::ranges::distance(rtype_ivars), 1);
    auto var = *std::ranges::begin(rtype_ivars);
    BOOST_REQUIRE(var);
    BOOST_CHECK_EQUAL(var->name(), "x");
    BOOST_CHECK(rtype->access_spec(var) == member_access_spec::protected_);  
}


/// Tests parsing class static function
BOOST_AUTO_TEST_CASE(parse_class_static_func) {
    parse_source_file(mdl, test_src_path() / "class_static_func.cpp", {});

    auto n_type = mdl.find_named_entity("my_class");
    BOOST_REQUIRE(n_type);

    auto rtype = dynamic_cast<named_record_type*>(n_type);
    BOOST_REQUIRE(rtype);

    BOOST_CHECK_EQUAL(rtype->name(), "my_class");
    BOOST_CHECK(rtype->kind() == record_kind::class_);
    BOOST_CHECK_EQUAL(std::ranges::size(rtype->bases()), 0);
    BOOST_CHECK_EQUAL(std::ranges::distance(rtype->vars()), 0);

    auto dfunc = dynamic_cast<named_function*>(rtype->find_named_entity("foo"));
    BOOST_CHECK_EQUAL(dfunc->name(), "foo");
    BOOST_CHECK(rtype->access_spec(dfunc) == member_access_spec::private_);  

    auto fn_type = mdl.get_or_create_func_type(dfunc);
    BOOST_REQUIRE(fn_type);
    BOOST_REQUIRE(!fn_type->ret_type().is_null());
    BOOST_CHECK(!fn_type->ret_type().is_const());
    BOOST_CHECK(!fn_type->ret_type().is_volatile());
    BOOST_CHECK(fn_type->ret_type().ctype() == mdl.bt_void());

    BOOST_REQUIRE_EQUAL(std::ranges::size(fn_type->params()), 1);
    auto par_type = *std::ranges::begin(fn_type->params());
    BOOST_REQUIRE(par_type);
    BOOST_REQUIRE(!par_type.is_null());
    BOOST_CHECK(!par_type.is_const());
    BOOST_CHECK(!par_type.is_volatile());
    BOOST_CHECK(par_type.ctype() == mdl.bt_int());
}


/// Tests parsing class method
BOOST_AUTO_TEST_CASE(parse_class_func) {
    parse_source_file(mdl, test_src_path() / "method.cpp", {});

    auto n_type = mdl.find_named_entity("my_class");
    BOOST_REQUIRE(n_type);

    auto rtype = dynamic_cast<named_record_type*>(n_type);
    BOOST_REQUIRE(rtype);

    BOOST_CHECK_EQUAL(rtype->name(), "my_class");
    BOOST_CHECK(rtype->kind() == record_kind::class_);
    BOOST_CHECK_EQUAL(std::ranges::size(rtype->bases()), 0);
    BOOST_CHECK_EQUAL(std::ranges::distance(rtype->vars()), 0);

    auto func = dynamic_cast<named_function*>(rtype->find_named_entity("foo"));
    BOOST_REQUIRE(func);
    BOOST_CHECK_EQUAL(func->name(), "foo");
    BOOST_CHECK(rtype->access_spec(func) == member_access_spec::private_);  

    auto fn_type = mdl.get_or_create_func_type(func);
    BOOST_REQUIRE(fn_type);
    BOOST_REQUIRE(!fn_type->ret_type().is_null());
    BOOST_CHECK(!fn_type->ret_type().is_const());
    BOOST_CHECK(!fn_type->ret_type().is_volatile());
    BOOST_CHECK(fn_type->ret_type().ctype() == mdl.bt_void());

    BOOST_REQUIRE_EQUAL(std::ranges::size(fn_type->params()), 1);
    auto par_type = *std::ranges::begin(fn_type->params());
    BOOST_REQUIRE(par_type);
    BOOST_REQUIRE(!par_type.is_null());
    BOOST_CHECK(!par_type.is_const());
    BOOST_CHECK(!par_type.is_volatile());
    BOOST_CHECK(par_type.ctype() == mdl.bt_int());
}


/// Tests parsing template class definition
BOOST_AUTO_TEST_CASE(parse_template_class) {
    parse_source_file(mdl, test_src_path() / "template_class.cpp", {});

    auto ent = mdl.find_named_entity("my_class");
    auto rtype = dynamic_cast<template_record*>(ent);
    BOOST_REQUIRE(rtype);

    BOOST_CHECK_EQUAL(rtype->name(), "my_class");
    BOOST_CHECK(rtype->kind() == record_kind::class_);
    BOOST_CHECK_EQUAL(std::ranges::size(rtype->bases()), 0);
    BOOST_CHECK_EQUAL(std::ranges::distance(rtype->fields()), 1);

    BOOST_CHECK(!rtype->is_variadic());
    BOOST_CHECK_EQUAL(std::ranges::distance(rtype->template_params()), 2);
    
    auto params = rtype->template_params();
    auto it = std::ranges::begin(params);
    BOOST_REQUIRE(it != std::ranges::end(params));
    BOOST_CHECK_EQUAL((*it)->name(), "T");
    auto tpar1 = dynamic_cast<const type_template_parameter*>(*it);
    BOOST_REQUIRE(tpar1);

    ++it;
    BOOST_REQUIRE(it != std::ranges::end(params));
    BOOST_CHECK_EQUAL((*it)->name(), "N");
    auto vpar2 = dynamic_cast<const value_template_parameter*>(*it);
    BOOST_REQUIRE(vpar2);
    BOOST_CHECK(vpar2->type() == tpar1);

    ++it;
    BOOST_CHECK(it == std::ranges::end(params));
}


// /// Tests parsing typedef with record definition
// BOOST_AUTO_TEST_CASE(typedef_record) {
//     parse_source_file(mdl, test_src_path() / "typedef_record.cpp", {});
// }


// BOOST_AUTO_TEST_CASE(simple) {
    
//     parse_source_file(mdl, test_src_path() / "simple.cpp", {});
// }


BOOST_AUTO_TEST_SUITE_END();


}
