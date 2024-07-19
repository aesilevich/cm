// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file debug_info_test.cpp
/// Contains unit tests for the debug_info class and related classes.

#include "pch.hpp"
#include "cm/code_model.hpp"
#include "cm/debug_info.hpp"
#include "cm/record_type_debug_info.hpp"
#include <boost/test/unit_test.hpp>


namespace cm::test {


struct debug_info_test_fixture {
    code_model cm;
    debug_info dbg{cm};
};


BOOST_FIXTURE_TEST_SUITE(debug_info_test, debug_info_test_fixture)


/// Tests default layout of fields of record type
BOOST_AUTO_TEST_CASE(default_layout) {
    auto rec = cm.create_record();
    rec->create_field("var1", cm.bt_int(), member_access_spec::public_);
    rec->create_field("var2", cm.bt_int(), member_access_spec::public_);
    dbg.make_def_rec_layout(rec);

    BOOST_CHECK_EQUAL(dbg.type_size(rec), 8);

    auto rec_dbg = dynamic_cast<const record_type_debug_info*>(dbg.type(rec));
    BOOST_REQUIRE(rec_dbg);

    auto & vars_offs = rec_dbg->vars_offsets();
    BOOST_REQUIRE_EQUAL(vars_offs.size(), 2);
    BOOST_CHECK_EQUAL(vars_offs[0], 0);
    BOOST_CHECK_EQUAL(vars_offs[1], 32);
}


/// Tests default recursive layout of fields of record type
BOOST_AUTO_TEST_CASE(default_recursive_layout) {
    auto fld_type = cm.create_named_record("TTTT");
    fld_type->create_field("x", cm.bt_int());
    auto td = cm.create_typedef("my_td", fld_type);

    auto rec = cm.create_record();
    rec->create_field("var1", td, member_access_spec::public_);
    rec->create_field("var2", cm.bt_int(), member_access_spec::public_);
    dbg.make_def_rec_layout(rec);

    BOOST_CHECK_EQUAL(dbg.type_size(rec), 8);

    auto rec_dbg = dynamic_cast<const record_type_debug_info*>(dbg.type(rec));
    BOOST_REQUIRE(rec_dbg);

    auto & vars_offs = rec_dbg->vars_offsets();
    BOOST_REQUIRE_EQUAL(vars_offs.size(), 2);
    BOOST_CHECK_EQUAL(vars_offs[0], 0);
    BOOST_CHECK_EQUAL(vars_offs[1], 32);
}


/// Tests default layout of base classes of record type
BOOST_AUTO_TEST_CASE(default_layout_bases) {
    auto base1 = cm.create_named_record("base1");
    base1->create_field("var1", cm.bt_int(), member_access_spec::public_);
    base1->create_field("var2", cm.bt_int(), member_access_spec::public_);
    dbg.make_def_rec_layout(base1);

    auto base2 = cm.create_named_record("base2");
    base2->create_field("varx", cm.bt_float(), member_access_spec::private_);
    dbg.make_def_rec_layout(base2);

    auto rec = cm.create_record();
    rec->add_base(base1);
    rec->add_base(base2);
    rec->create_field("xxx", cm.bt_double(), member_access_spec::protected_);

    dbg.make_def_rec_layout(rec);

    BOOST_CHECK_EQUAL(dbg.type_size(rec), 20);

    auto rec_dbg = dynamic_cast<const record_type_debug_info*>(dbg.type(rec));
    BOOST_REQUIRE(rec_dbg);

    auto & bases_offs = rec_dbg->bases_offsets();
    BOOST_REQUIRE_EQUAL(bases_offs.size(), 2);
    BOOST_CHECK_EQUAL(bases_offs[0], 0);
    BOOST_CHECK_EQUAL(bases_offs[1], 64);

    auto & vars_offs = rec_dbg->vars_offsets();
    BOOST_REQUIRE_EQUAL(vars_offs.size(), 1);
    BOOST_CHECK_EQUAL(vars_offs[0], 96);
}


/// Tests recursive default layout of base classes of record type
BOOST_AUTO_TEST_CASE(default_recursive_layout_bases) {
    auto base1 = cm.create_named_record("base1");
    base1->create_field("var1", cm.bt_int(), member_access_spec::public_);
    base1->create_field("var2", cm.bt_int(), member_access_spec::public_);

    auto base2 = cm.create_named_record("base2");
    base2->create_field("varx", cm.bt_float(), member_access_spec::private_);

    auto rec = cm.create_record();
    rec->add_base(base1);
    rec->add_base(base2);
    rec->create_field("xxx", cm.bt_double(), member_access_spec::protected_);

    dbg.make_def_rec_layout(rec);

    BOOST_CHECK_EQUAL(dbg.type_size(rec), 20);

    auto rec_dbg = dynamic_cast<const record_type_debug_info*>(dbg.type(rec));
    BOOST_REQUIRE(rec_dbg);

    auto & bases_offs = rec_dbg->bases_offsets();
    BOOST_REQUIRE_EQUAL(bases_offs.size(), 2);
    BOOST_CHECK_EQUAL(bases_offs[0], 0);
    BOOST_CHECK_EQUAL(bases_offs[1], 64);

    auto & vars_offs = rec_dbg->vars_offsets();
    BOOST_REQUIRE_EQUAL(vars_offs.size(), 1);
    BOOST_CHECK_EQUAL(vars_offs[0], 96);
}


/// Tests adding and searching debug info for function
BOOST_AUTO_TEST_CASE(add_func) {
    auto func = cm.create_function("foo");
    func->set_ret_type(cm.bt_int());
    dbg.add_object(func, 0x20000000, 100);
    auto fdbg = dbg.object(func);

    BOOST_REQUIRE(fdbg);
    BOOST_CHECK_EQUAL(fdbg->addr(), 0x20000000);
    BOOST_CHECK_EQUAL(fdbg->size(), 100);
}


/// Tests searching for not existing function located address
BOOST_AUTO_TEST_CASE(func_at_addr_not_exist) {
    auto func = cm.create_function("foo");
    func->set_ret_type(cm.bt_int());
    dbg.add_object(func, 0x20000000, 100);

    auto res = dbg.entity_at_addr(0x20000000 + 100);
    BOOST_CHECK(!res);
}


/// Tests searching for existing function located at address
BOOST_AUTO_TEST_CASE(func_at_addr) {
    auto func = cm.create_function("foo");
    func->set_ret_type(cm.bt_int());
    dbg.add_object(func, 0x20000000, 100);

    auto res = dbg.entity_at_addr(0x20000001);
    BOOST_CHECK(res == func);
}


BOOST_AUTO_TEST_SUITE_END()


}
