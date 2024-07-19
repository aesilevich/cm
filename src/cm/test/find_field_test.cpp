// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file find_field_test.cpp
/// Contains unit tests for functions for searching fields in record types.

#include "pch.hpp"
#include "cm/code_model.hpp"
#include "cm/find_field.hpp"
#include <boost/test/unit_test.hpp>


namespace cm::test {


struct find_field_test_fixture {
    code_model cm;
    debug_info dbg{cm};
};


BOOST_FIXTURE_TEST_SUITE(find_field_test, find_field_test_fixture)


/// Tests simple search of field without offset
BOOST_AUTO_TEST_CASE(simple) {
    auto rec = cm.create_record();
    rec->create_field("aaa", cm.bt_int());
    rec->create_field("bbb", cm.bt_float());

    auto res = find_field(nullptr, rec, "bbb", false);
    BOOST_CHECK(std::get<0>(res)->type() == qual_type{cm.bt_float()});
    BOOST_CHECK(std::get<1>(res) == 0);
}


/// Tests simple search of non existing field
BOOST_AUTO_TEST_CASE(not_exist) {
    auto rec = cm.create_record();
    rec->create_field("aaa", cm.bt_int());
    rec->create_field("bbb", cm.bt_float());

    auto res = find_field(nullptr, rec, "bbc", false);
    BOOST_CHECK(std::get<0>(res) == nullptr);
    BOOST_CHECK(std::get<1>(res) == 0);
}


/// Tests simple search of field with offset
BOOST_AUTO_TEST_CASE(simple_offset) {
    auto rec = cm.create_record();
    rec->create_field("aaa", cm.bt_int());
    rec->create_field("bbb", cm.bt_float());
    auto dinfo = dbg.make_def_rec_layout(rec);

    auto res = find_field(&dbg, rec, "bbb", false);
    BOOST_CHECK(std::get<0>(res)->type() == qual_type{cm.bt_float()});
    BOOST_CHECK(std::get<1>(res) == dinfo->vars_offsets()[1]);
}


/// Tests searching field in base class
BOOST_AUTO_TEST_CASE(base) {
    auto base = cm.create_named_record("base");
    base->create_field("xxx", cm.bt_int());
    base->create_field("yyy", cm.bt_char());
    auto base_dinfo = dbg.make_def_rec_layout(base);

    auto rec = cm.create_record();
    rec->add_base(base);
    rec->create_field("aaa", cm.bt_int());
    rec->create_field("bbb", cm.bt_float());
    auto dinfo = dbg.make_def_rec_layout(rec);

    auto res = find_field(&dbg, rec, "yyy", true);
    BOOST_CHECK(std::get<0>(res)->type() == qual_type{cm.bt_char()});
    auto offs = dinfo->bases_offsets()[0] + base_dinfo->vars_offsets()[1];
    BOOST_CHECK(std::get<1>(res) == offs);
}


/// Tests searching field without taking into account base classes
BOOST_AUTO_TEST_CASE(base_no_search) {
    auto base = cm.create_named_record("base");
    base->create_field("xxx", cm.bt_int());
    base->create_field("yyy", cm.bt_char());
    auto base_dinfo = dbg.make_def_rec_layout(base);

    auto rec = cm.create_record();
    rec->add_base(base);
    rec->create_field("aaa", cm.bt_int());
    rec->create_field("bbb", cm.bt_float());
    auto dinfo = dbg.make_def_rec_layout(rec);

    auto res = find_field(&dbg, rec, "yyy", false);
    BOOST_CHECK(std::get<0>(res) == nullptr);
    BOOST_CHECK(std::get<1>(res) == 0);
}


/// Tests searching field inside anonymous record field
BOOST_AUTO_TEST_CASE(anon_field) {
    auto anon_rec = cm.create_record();
    anon_rec->create_field("xxx", cm.bt_int());
    anon_rec->create_field("yyy", cm.bt_short());
    auto anon_dinfo = dbg.make_def_rec_layout(anon_rec);

    auto rec = cm.create_record();
    rec->create_field("aaa", cm.bt_int());
    rec->create_field("", anon_rec);
    rec->create_field("bbb", cm.bt_float());
    auto dinfo = dbg.make_def_rec_layout(rec);

    auto res = find_field(&dbg, rec, "yyy", true);
    BOOST_CHECK(std::get<0>(res)->type() == qual_type{cm.bt_short()});
    auto offs = dinfo->vars_offsets()[1] + anon_dinfo->vars_offsets()[1];
    BOOST_CHECK(std::get<1>(res) == offs);
}


BOOST_AUTO_TEST_SUITE_END()


}
