// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file builder_test.cpp
/// Contains unit tests for the builder class.

#include "pch.hpp"
#include "cm/builder.hpp"
#include "cm/code_model.hpp"
#include "cm/debug_info.hpp"
#include <boost/test/unit_test.hpp>


namespace cm::test {


struct builder_test_fixture {
    code_model cm;
    debug_info dbg{cm};
    builder b{cm, &dbg};
};


BOOST_FIXTURE_TEST_SUITE(builder_test, builder_test_fixture)


/// Tests building empty CM
BOOST_AUTO_TEST_CASE(empty) {
    b.build();
}


/// Tests creating namespace
BOOST_AUTO_TEST_CASE(create_ns) {
    b.ns("test")
     .end();

    b.build();

    BOOST_CHECK(cm.find_namespace("test") != nullptr);
}


/// Tests creating nested namespace
BOOST_AUTO_TEST_CASE(nested_ns) {
    auto res = b
        .ns("ns1")
            .ns("ns2")
            .end()
        .end()
    .build();

    auto ns1 = cm.find_namespace("ns1");
    BOOST_REQUIRE(ns1 != nullptr);
    auto ns2 = ns1->find_namespace("ns2");
    BOOST_CHECK(ns2 != nullptr);
    BOOST_CHECK(ns1 != ns2);
}


/// Tests creating record type
BOOST_AUTO_TEST_CASE(create_record) {
    auto res = b
        .record("myrec", record_kind::struct_, "xx")
        .end()
    .build();

    auto rec = cm.find_named_record("myrec");
    BOOST_REQUIRE(rec != nullptr);
    BOOST_CHECK_EQUAL(rec->name(), "myrec");
    BOOST_CHECK(rec->kind() == record_kind::struct_);
    BOOST_CHECK(rec == res.type("xx"));
}


/// Tests building existing record type
BOOST_AUTO_TEST_CASE(create_existing_record) {
    auto rec = cm.create_named_record("myrec", record_kind::struct_);

    auto res = b
        .record("myrec", record_kind::struct_, "recid")
        .end()
    .build();

    auto rec2 = res.type("recid");
    BOOST_CHECK(rec2 != nullptr);
    BOOST_CHECK(rec == rec2);
}


/// Tests creating typedef in record
BOOST_AUTO_TEST_CASE(typedef_in_rec) {
    auto res = b
        .record("myrec")
            .tdef("mytype", b.typeref("rec2"))
        .end()
        .ns("nest_ns")
            .record("rrrr", "rec2")
            .end()
        .end()
    .build();

    auto myrec = cm.find_named_record("myrec");
    auto nest_ns = cm.find_namespace("nest_ns");
    BOOST_REQUIRE(nest_ns);
    auto rrrr = nest_ns->find_named_record("rrrr");
    BOOST_REQUIRE(rrrr);

    auto tdef = myrec->find_typedef("mytype");
    BOOST_REQUIRE(tdef);
    BOOST_CHECK(tdef->base().type() == rrrr);

    auto rrrr_from_id = res.type("rec2");
    BOOST_CHECK(rrrr_from_id == rrrr);
}


/// Tests creating nested record
BOOST_AUTO_TEST_CASE(nested_record) {
    auto res = b
        .record("rec")
            .record("nested", "nr")
            .end()
        .end()
    .build();

    auto rec = cm.find_named_record("rec");
    BOOST_REQUIRE(rec);
    auto nested = rec->find_named_record("nested");
    BOOST_CHECK(nested);
    BOOST_CHECK(nested == res.type("nr"));
}


/// Tests creating field in record
BOOST_AUTO_TEST_CASE(record_ivar) {
    auto res = b
        .record("rec")
            .ivar("x", cm.bt_int())
            .ivar("y", cm.bt_float())
        .end()
    .build();

    auto rec = cm.find_named_record("rec");
    BOOST_REQUIRE(rec);

    auto ivars = rec->fields();
    auto it = std::begin(ivars);
    BOOST_REQUIRE(it != std::end(ivars));
    auto x = *it;
    BOOST_CHECK_EQUAL(x->name(), "x");
    BOOST_CHECK(x->type() == qual_type{cm.bt_int()});

    ++it;
    BOOST_REQUIRE(it != std::end(ivars));
    auto y = *it;
    BOOST_CHECK_EQUAL(y->name(), "y");
    BOOST_CHECK(y->type() == qual_type{cm.bt_float()});

    ++it;
    BOOST_CHECK(it == std::end(ivars));
}


/// Tests creating method in record
BOOST_AUTO_TEST_CASE(record_ifunc) {
    auto res = b
        .record("rec")
            .ifunc("foo", b.ftype(cm.bt_int(), cm.bt_float(), cm.bt_short()))
        .end()
    .build();

    auto rec = cm.find_named_record("rec");
    BOOST_REQUIRE(rec);

    auto rec_entities = rec->entities();
    auto it = std::begin(rec_entities);
    auto end = std::end(rec_entities);

    BOOST_REQUIRE(it != end);
    auto f = dynamic_cast<cm::function*>(*it);
    BOOST_REQUIRE(f);
    BOOST_CHECK(f->ret_type() == qual_type{cm.bt_int()});

    auto params = f->params();
    BOOST_REQUIRE_EQUAL(std::ranges::size(params), 2);
    auto par_it = std::ranges::begin(params);
    auto par1 = *par_it;
    ++par_it;
    auto par2 = *par_it;

    BOOST_CHECK(par1->type() == qual_type{cm.bt_float()});
    BOOST_CHECK(par2->type() == qual_type{cm.bt_short()});

    ++it;
    BOOST_CHECK(it == end);
}


/// Tests creating template record instance
BOOST_AUTO_TEST_CASE(template_record) {
    auto res = b
        .templ("ttt", "T1", "T2")
            .record("myinst", cm.bt_int(), cm.bt_float())
            .end()
        .end()
    .build();

    auto ttt = cm.get_or_create_template_record("ttt", record_kind::class_, false);
    auto inst = ttt->find_instantiation(cm.bt_int(), cm.bt_float());
    BOOST_REQUIRE(inst);
    BOOST_REQUIRE(inst->args().size() == 2);
    BOOST_CHECK(inst == res.type("myinst"));
}


/// Tests creating template record instance with use
BOOST_AUTO_TEST_CASE(template_record_use) {
    auto res = b
        .templ("ttt", "T1", "T2")
            .record("myinst", cm.bt_int(), cm.bt_float())
            .end()
        .end()

        .record("rec2").base(b.typeref("myinst"))
        .end()
    .build();

    auto ttt = cm.get_or_create_template_record("ttt", record_kind::class_, false);
    auto inst = ttt->find_instantiation(cm.bt_int(), cm.bt_float());
    BOOST_REQUIRE(inst);
    BOOST_REQUIRE(inst->args().size() == 2);
    BOOST_CHECK(inst == res.type("myinst"));
}



/// Tests creating record with base
BOOST_AUTO_TEST_CASE(record_base) {
    auto res = b
        .record("base", "b")
        .end()
        .record("rec").base(b.typeref("b"))
        .end()
    .build();

    auto rec = cm.find_named_record("rec");
    auto base = cm.find_named_record("base");
    BOOST_REQUIRE(rec);
    BOOST_REQUIRE(rec->bases().size() == 1);
    BOOST_CHECK(rec->bases()[0] == base);
}


/// Tests creating record with instance var twice
BOOST_AUTO_TEST_CASE(record_ivar_twice) {
    auto res = b
        .record("rec")
            .ivar("x", cm.bt_int())
        .end()
    .build();

    {
        builder b2{cm, &dbg};
        b2
            .record("rec")
                .ivar("x", cm.bt_int())
            .end()
        .build();
    }

    auto rec = cm.find_named_record("rec");
    BOOST_REQUIRE(rec);

    auto ivars = rec->fields();
    auto it = std::begin(ivars);
    BOOST_REQUIRE(it != std::end(ivars));
    auto x = *it;
    BOOST_CHECK_EQUAL(x->name(), "x");
    BOOST_CHECK(x->type() == qual_type{cm.bt_int()});

    ++it;
    BOOST_CHECK(it == std::end(ivars));
}


/// Tests creating record with instance var twice
/// and referencing nested record type in new type
BOOST_AUTO_TEST_CASE(record_ivar_twice_nested_ref) {
    auto res = b
        .record("rec")
            .record("nested", "nested")
            .end()
            .ivar("x", cm.bt_int())
        .end()
    .build();

    {
        builder b2{cm, &dbg};
        b2
            .record("rec")
                .record("nested", "nested")
                .end()
                .ivar("x", cm.bt_int())
            .end()
            .record("new_rec")
                .ivar("a", b2.typeref("nested"))
            .end()
        .build();
    }

    auto rec = cm.find_named_record("rec");
    BOOST_REQUIRE(rec);

    auto ivars = rec->fields();
    auto it = std::begin(ivars);
    BOOST_REQUIRE(it != std::end(ivars));
    auto x = *it;
    BOOST_CHECK_EQUAL(x->name(), "x");
    BOOST_CHECK(x->type() == qual_type{cm.bt_int()});

    ++it;
    BOOST_CHECK(it == std::end(ivars));

    auto nested = rec->find_named_record("nested");
    BOOST_CHECK(nested != nullptr);

    auto new_rec = cm.find_named_record("new_rec");
    BOOST_REQUIRE(new_rec != nullptr);

    auto nr_ivars = new_rec->fields();
    auto nr_it = std::begin(nr_ivars);
    BOOST_REQUIRE(nr_it != std::end(nr_ivars));
    auto a = *nr_it;
    BOOST_CHECK_EQUAL(a->name(), "a");
    BOOST_CHECK(a->type() == qual_type{nested});

    ++nr_it;
    BOOST_CHECK(nr_it == std::end(nr_ivars));
}


static record_type * nested_builder_func(code_model & cm,
                                         debug_info & dbg,
                                         const qual_type & type) {
    cm::builder b{cm, &dbg};
    auto res = b
        .ns("__gnu_cxx")
            .templ("__aligned_membuf", "T").record("membuf", type)
                .ivar("_M_storage", b.atype(cm.bt_char(), dbg.type_size(type)))
            .end().end()
        .end()
    .build();

    return res.type("membuf")->cast<cm::record_type>();
}



/// Tests building with pointer to ref type + nested builder that
/// should remove it's own ref types after build
BOOST_AUTO_TEST_CASE(ptr_type_ref) {
    auto elt_type = cm.bt_int();

    auto res = b
        .ns("std")
            .record("_Fwd_list_node_base", "node_base")
            .end()

            .templ("_Fwd_list_node", "T").record("node", elt_type)
                .ivar("_M_storage", nested_builder_func(cm, dbg, elt_type))
            .end().end()

            .templ("_Fwd_list_iterator", "T").record("iterator", elt_type)
                .ivar("_M_node", b.ptype(b.typeref("node_base")))
            .end().end()
        .end()
    .build();
}


/// Tests creating static function inside record
BOOST_AUTO_TEST_CASE(static_mem_fun) {
    auto res = b
        .record("myrec", "myrec")
            .func("foo", b.ftype(cm.bt_int(), cm.bt_int()))
        .end()
    .build();

    auto rec = res.rtype("myrec");
    BOOST_REQUIRE(rec != nullptr);

    auto rec_entities = rec->entities();
    auto it = std::begin(rec_entities);
    BOOST_REQUIRE(it != std::end(rec_entities));

    auto f = dynamic_cast<function*>(*it);
    BOOST_CHECK(cm.get_or_create_func_type(f) == cm.get_or_create_func_type(cm.bt_int(), cm.bt_int()));

    ++it;
    BOOST_CHECK(it == std::end(rec_entities));
}


/// Incremental build test
BOOST_AUTO_TEST_CASE(incremental) {
    auto res = b
        .record("rec", "rec")
        .end()
    .build();

    auto rec = cm.find_named_record("rec");
    BOOST_REQUIRE(rec != nullptr);
    BOOST_CHECK(rec == res.rtype("rec"));

    auto res2 = b
        .templ("templ", "T").record("templ", rec)
        .end().end()
    .build();

    auto templ = res2.rtype("templ");
    BOOST_CHECK(templ != nullptr);
    auto rec2 = res.rtype("rec");
    BOOST_CHECK(rec2 == rec);
}


/// Tests creating bitfield in record
BOOST_AUTO_TEST_CASE(record_bitfield) {
    auto res = b
        .record("rec")
            .ivar("x", cm.bt_int(), member_access_spec::public_, 3)
        .end()
    .build();

    auto rec = cm.find_named_record("rec");
    BOOST_REQUIRE(rec);

    auto ivars = rec->fields();
    auto it = std::begin(ivars);
    BOOST_REQUIRE(it != std::end(ivars));
    auto x = *it;
    BOOST_CHECK_EQUAL(x->name(), "x");
    BOOST_CHECK(x->type() == qual_type{cm.bt_int()});
    BOOST_CHECK_EQUAL(x->bit_size(), 3);

    ++it;
    BOOST_CHECK(it == std::end(ivars));
}


/// Test for crash in formatter test for std list
BOOST_AUTO_TEST_CASE(fmt_list_test_crash) {
    auto elt_type = b.bt_int();
    auto list_node_ivar_name = "_M_storage";
    auto list_node_ivar_type = elt_type;

    auto res = b
        .ns("std")
            .record("_List_node_base", "stdcpp_node_base")
                .ivar("_M_prev", b.ptype(b.typeref("stdcpp_node_base")))
                .ivar("_M_next", b.ptype(b.typeref("stdcpp_node_base")))
            .end()

            .templ("_List_node", "T").record("stdcpp_node", elt_type)
                .base(b.typeref("stdcpp_node_base"))
                .ivar(list_node_ivar_name, list_node_ivar_type)
            .end().end()

            .templ("_List_base", "T").record("stdcpp_list_base", elt_type)
                .record("_List_impl", "stdcpp_impl")
                    .ivar("_M_node", b.typeref("stdcpp_node_base"))
                .end()
            .end().end()

            .templ("list", "T").record("stdcpp_list", elt_type)
                .ivar("_M_impl", b.typeref("stdcpp_impl"))
            .end().end()

            .ns("__cxx11")
                .templ("_List_base", "T").record("stdcpp_list_base_cxx11", elt_type)
                    .record("_List_impl", "stdcpp_impl_cxx11")
                        .ivar("_M_node", b.typeref("stdcpp_node_base"))
                    .end()
                .end().end()

                .templ("list", "T").record("stdcpp_list_cxx11", elt_type)
                    .ivar("_M_impl", b.typeref("stdcpp_impl_cxx11"))
                .end().end()
            .end()

            .templ("_List_iterator", "T").record("stdcpp_iterator", elt_type)
                .ivar("_M_node", b.ptype(b.typeref("stdcpp_node")))
            .end().end()


            // libc++ std::list types
            .ns("__1")
                .templ("__list_node_base", "T").record("libcxx_node_base", elt_type)
                    .ivar("__prev_", b.ptype(b.typeref("libcxx_node_base")))
                    .ivar("__next_", b.ptype(b.typeref("libcxx_node_base")))
                .end().end()

                .templ("__list_impl", "T").record("libcxx_impl", elt_type)
                    .ivar("__end_", b.typeref("libcxx_node_base"))
                    .ivar("__size_alloc_", b.bt_unsigned_long())
                .end().end()

                .templ("list", "T").record("libcxx_list", elt_type)
                    .base(b.typeref("libcxx_impl"))
                .end().end()

                .templ("__list_node", "T", "Ptr").record("libcxx_node", elt_type, b.ptype(b.bt_void()))
                    .base(b.typeref("libcxx_node_base"))
                    .ivar("__value_", elt_type)
                .end().end()

                .templ("__list_iterator", "T", "Ptr").record("libcxx_iterator", elt_type, b.ptype(b.bt_void()))
                    .ivar("__ptr_", b.ptype(b.typeref("libcxx_node_base")))
                .end().end()
            .end()
        .end()
    .build();
}


/// Test case for crahs in libc++ tree test in fmt
BOOST_AUTO_TEST_CASE(libcxx_tree_test) {
    auto first = b.bt_int();
    auto second = b.bt_int();

    builder b2{cm, &dbg};
    auto res = b2.ns("std").ns("__1")
        .templ("pair", "First", "Second").record("pair", first, second)
            .ivar("first", first)
            .ivar("second", second)
        .end().end()
    .end().end().build();

    auto elt_type = res.type("pair");

    b.ns("std").ns("__1")
            .templ("__tree_node_base", "Ptr").record("node_base", b.ptype(b.bt_void()))
                .ivar("__left_", b.ptype(b.typeref("node_base")))
                .ivar("__right_", b.ptype(b.typeref("node_base")))
                .ivar("__parent_", b.ptype(b.typeref("end_node")))
            .end().end()

            .templ("__tree_node", "T", "Ptr").record("node", elt_type, b.ptype(b.bt_void()))
                .base(b.typeref("node_base"))
                .ivar("__value_", elt_type)
            .end().end()

            .record("__tree_end_node", "end_node")
                .ivar("__left_", b.ptype(b.typeref("node_base")))
            .end()

            .record({}, "end_node_pair")
                .ivar("__value_", b.typeref("end_node"))
            .end()

            .record({}, "size_pair")
                .ivar("__value_", b.bt_unsigned_long())
            .end()

            .templ("__tree", "T").record("tree", elt_type)
                .ivar("__begin_node_", b.ptype(b.typeref("end_node")))
                .ivar("__pair1_", b.typeref("end_node_pair"))
                .ivar("__pair3_", b.typeref("size_pair"))
            .end().end()

            // .templ("__tree_const_iterator", "Val", "NodePtr")
            //     .record("tree_const_iterator", elt_type, b.ptype(b.typeref("node")))
            //         .ivar("__ptr_", b.ptype(b.typeref("end_node")))
            //     .end()
            // .end()

            // .templ("__tree_iterator", "Val", "NodePtr")
            //     .record("tree_iterator", elt_type, b.ptype(b.typeref("node")))
            //         .ivar("__ptr_", b.ptype(b.typeref("end_node")))
            //     .end()
            // .end()

            // .templ("__map_iterator", "TreeIt").record("map_iterator", b.typeref("tree_iterator"))
            //     .ivar("__i_", b.typeref("tree_iterator"))
            // .end().end()
        .end().end()
    .build();
}


/// Test case for libc++ pair
BOOST_AUTO_TEST_CASE(libcxx_pair) {
    auto first = b.bt_int();
    auto second = b.bt_int();

    b.ns("std").ns("__1")
        .templ("pair", "First", "Second").record("pair", first, second)
            .ivar("first", first)
            .ivar("second", second)
        .end().end()
    .end().end()
    .build();
}


BOOST_AUTO_TEST_SUITE_END()


}
