// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file source_code_model_test.cpp
/// Contains unit tests for the source_code_model class.

#include "pch.hpp"
#include "cm/src/source_code_model.hpp"
#include <boost/test/unit_test.hpp>


namespace cm::src::test {


struct source_code_model_test_fixture {
    source_code_model cm;
};


BOOST_FIXTURE_TEST_SUITE(source_code_model_test, source_code_model_test_fixture)


BOOST_AUTO_TEST_CASE(test_new) {
}


BOOST_AUTO_TEST_SUITE_END()


}
