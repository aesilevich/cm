// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file source_file_position_test.cpp
/// Contains unit tests for source_file_position and source_file_position_desc classes.

#include "pch.hpp"
#include "cm/src/source_code_model.hpp"
#include <boost/test/unit_test.hpp>


namespace cm::src::test {


BOOST_AUTO_TEST_SUITE(source_file_position_test)


/// Tests parsing valid source position with column number
BOOST_AUTO_TEST_CASE(parse_with_col_number) {
    std::string str = "test.cpp:12:24";
    auto pos = source_file_position_desc::from_string(str);
    BOOST_CHECK_EQUAL(pos.path(), std::filesystem::path{"test.cpp"});
    BOOST_CHECK_EQUAL(pos.pos().line(), 12);
    BOOST_CHECK_EQUAL(pos.pos().column(), 24);
}


/// Tests parsing valid source position without column number
BOOST_AUTO_TEST_CASE(parse_no_col_number) {
    std::string str = "test.cpp:12";
    auto pos = source_file_position_desc::from_string(str);
    BOOST_CHECK_EQUAL(pos.path(), std::filesystem::path{"test.cpp"});
    BOOST_CHECK_EQUAL(pos.pos().line(), 12);
    BOOST_CHECK_EQUAL(pos.pos().column(), 1);
}


/// Tests parsing source position with empty path
BOOST_AUTO_TEST_CASE(parse_empty_path) {
    std::string str = ":12:23";
    bool is_err = false;

    try {
        auto pos = source_file_position_desc::from_string(str);
    }
    catch(std::runtime_error & err) {
        is_err = true;
        BOOST_CHECK_EQUAL(err.what(), "invalid source file position ':12:23': "
                                      "empty file path");
    }

    BOOST_CHECK(is_err);
}


/// Tests parsing source position with no line number
BOOST_AUTO_TEST_CASE(parse_no_line_number) {
    std::string str = "test.cpp";
    bool is_err = false;

    try {
        auto pos = source_file_position_desc::from_string(str);
    }
    catch(std::runtime_error & err) {
        is_err = true;
        BOOST_CHECK_EQUAL(err.what(), "invalid source file position 'test.cpp': "
                                      "can't find ':' character");
    }

    BOOST_CHECK(is_err);
}


/// Tests parsing source position with empty line number and no column number
BOOST_AUTO_TEST_CASE(parse_empty_line_no_col) {
    std::string str = "test.cpp:";
    bool is_err = false;

    try {
        auto pos = source_file_position_desc::from_string(str);
    }
    catch(std::runtime_error & err) {
        is_err = true;
        BOOST_CHECK_EQUAL(err.what(), "invalid source file position 'test.cpp:': "
                                      "empty line number");
    }

    BOOST_CHECK(is_err);
}


/// Tests parsing source position with empty line number and valid column number
BOOST_AUTO_TEST_CASE(parse_empty_line_with_col) {
    std::string str = "test.cpp::12";
    bool is_err = false;

    try {
        auto pos = source_file_position_desc::from_string(str);
    }
    catch(std::runtime_error & err) {
        is_err = true;
        BOOST_CHECK_EQUAL(err.what(), "invalid source file position 'test.cpp::12': "
                                      "empty line number");
    }

    BOOST_CHECK(is_err);
}


/// Tests parsing source position with invalid line number
BOOST_AUTO_TEST_CASE(parse_invalid_line) {
    std::string str = "test.cpp:4asd:1";
    bool is_err = false;

    try {
        auto pos = source_file_position_desc::from_string(str);
    }
    catch(std::runtime_error & err) {
        is_err = true;
        BOOST_CHECK_EQUAL(err.what(), "invalid source file position 'test.cpp:4asd:1': "
                                      "invalid line number: '4asd'");
    }

    BOOST_CHECK(is_err);
}


/// Tests parsing source position with invalid column number
BOOST_AUTO_TEST_CASE(parse_invalid_column) {
    std::string str = "test.cpp:1:882a";
    bool is_err = false;

    try {
        auto pos = source_file_position_desc::from_string(str);
    }
    catch(std::runtime_error & err) {
        is_err = true;
        BOOST_CHECK_EQUAL(err.what(), "invalid source file position 'test.cpp:1:882a': "
                                      "invalid column number: '882a'");
    }

    BOOST_CHECK(is_err);
}


BOOST_AUTO_TEST_SUITE_END()


}
