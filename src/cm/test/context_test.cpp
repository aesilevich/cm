// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file context_test.cpp
/// Contains unit tests for the context class.

#include "pch.hpp"
#include "cm/code_model.hpp"
#include "cm/context.hpp"
#include <boost/test/unit_test.hpp>


namespace cm::test {


class mock_context: public context {
public:
    mock_context(): context(nullptr), context_entity(nullptr) {}

    void print_desc(std::ostream & str) const override {
        str << "mock_context";
    }
};


struct context_test_fixture {
    code_model cm;
    mock_context ctx;
};


BOOST_FIXTURE_TEST_SUITE(entity_context_test, context_test_fixture)


/// Tests decl context initialization
BOOST_AUTO_TEST_CASE(init) {
    BOOST_CHECK(ctx.is_root());
    BOOST_CHECK(ctx.entities().empty());
    BOOST_CHECK(ctx.vars().empty());
}


/// Tests creating typedef in decl context
BOOST_AUTO_TEST_CASE(create_typedef) {
    auto td = ctx.create_typedef("my_type", cm.bt_int());

    BOOST_CHECK_EQUAL(td->name(), "my_type");
    BOOST_CHECK(td->base() == qual_type{cm.bt_int()});
    BOOST_CHECK(!ctx.entities().empty());

    auto tit = std::begin(ctx.entities());
    BOOST_CHECK(tit != std::end(ctx.entities()));
    BOOST_CHECK(td == *tit);
}


/// Tests creating enum in decl context
BOOST_AUTO_TEST_CASE(create_enum) {
    auto en = ctx.create_enum("my_enum", cm.bt_int());
    BOOST_CHECK(en->items().empty());
}


BOOST_AUTO_TEST_SUITE_END()


}
