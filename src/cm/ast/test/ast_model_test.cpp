// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file ast_model_test.cpp
/// Contains unit tests for the ast_model class.

#include "pch.hpp"
#include "cm/ast/ast_model.hpp"
#include <boost/test/unit_test.hpp>


namespace cm::ast::test {


struct ast_model_test_fixture {
    ast_model cm;
};


BOOST_FIXTURE_TEST_SUITE(ast_model_test, ast_model_test_fixture)


BOOST_AUTO_TEST_CASE(test_new) {
}


BOOST_AUTO_TEST_SUITE_END()


}
