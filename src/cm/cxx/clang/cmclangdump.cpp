// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file cmclangdump.cpp
/// Contains code for the cmclangdump utility for parsing and dumping code model.

#include "pch.hpp"
#include "cm/cxx/clang/cmclang.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <boost/program_options.hpp>


namespace fs = std::filesystem;
namespace po = boost::program_options;


/// Prses command line, splits regular options from --args'-like option list.
/// Returns true if --args option is found
template <typename Iterator>
bool parse_args_command_line(Iterator it,
                             Iterator end,
                             std::vector<std::string> & regular_options,
                             std::vector<std::string> & args_options) {

    bool args_parsed = false;

    static const std::string s_args = "--args";
    static const std::string s_dash = "--";

    for (; it != end; ++it) {
        if (args_parsed) {
            args_options.push_back(*it);
        } else {
            if (s_args == *it || s_dash == *it) {
                args_parsed = true;
            } else {
                regular_options.push_back(*it);
            }
        }
    }

    return args_parsed;
}


// Compares string with content of specified file. Thorws exception if content does not match
void compare_string_with_file(const std::string & str, const fs::path & file_path) {
    std::ifstream file_istr{file_path.string()};
    std::istringstream str_istr{str};

    if (!file_istr.is_open()) {
        std::ostringstream msg;
        msg << "can't open code model file: " << file_path;
        throw std::runtime_error{msg.str()};
    }

    std::string file_line;
    std::string str_line;
    unsigned int line_num = 1;
    while (std::getline(file_istr, file_line)) {
        if (!std::getline(str_istr, str_line)) {
            std::ostringstream msg;
            msg << "code model line " << line_num
                << " does not match: expected '" << file_line << "', got EOF";
            throw std::runtime_error{msg.str()};
        }

        if (file_line != str_line) {
            std::ostringstream msg;
            msg << "code model line " << line_num
                << " does not match: expected '" << file_line << "', got '" << str_line << "'";
            throw std::runtime_error{msg.str()};
        }

        ++line_num;
    }

    if (std::getline(str_istr, str_line)) {
        std::ostringstream msg;
        msg << "code model line " << line_num
            << " oes not match: expected EOF, got '" << str_line << "'";
        throw std::runtime_error{msg.str()};
    }
}


int main(int argc, char * argv[]) {
    try {
        po::options_description opt_desc;
        opt_desc.add_options()
            ("help", "produce help message and exit")
            ("input-file,i", po::value<fs::path>(), "input source file")
            ("compare", po::value<fs::path>(), "compare dump of parsed code model with file")
            ("dump-builtins", "dump builtins")
            ("dump-locations", "dump definition locations");

        po::positional_options_description pos_opt_desc;
        pos_opt_desc.add("input-file", 1);

        // parsing -- options
        std::vector<std::string> opts;
        std::vector<std::string> compile_opts;

        bool args_parsed = parse_args_command_line(argv + 1,
                                                   argv + argc,
                                                   opts,
                                                   compile_opts);

        po::variables_map var_map;
        po::store(
            po::command_line_parser(opts).options(opt_desc).positional(pos_opt_desc).run(),
            var_map);

        // checking for help option
        if (var_map.count("help") > 0) {
            opt_desc.print(std::cout);
            return 1;
        }

        // checking that input file is specified in command line
        if (var_map.count("input-file") == 0) {
            throw std::runtime_error("no input file specified in command line");
        }

        // creating and parsing code model
        cm::code_model mdl;
        cm::clang::parse_source_file(mdl, var_map["input-file"].as<fs::path>(), compile_opts);

        cm::dump_options dump_opts;
        dump_opts.builtins = var_map.count("dump-builtins") > 0;
        dump_opts.locations = var_map.count("dump-locations") > 0;

        if (var_map.count("compare") != 0) {
            // comparing code model dump with file
            std::ostringstream str;
            mdl.dump(str, dump_opts);
            compare_string_with_file(str.str(), var_map["compare"].as<fs::path>());
        } else {
            // dumping code model to stdout
            mdl.dump(std::cout, dump_opts);
        }
    }
    catch (std::exception & err) {
        std::cerr << "ERROR: " << err.what() << std::endl;
        return 2;
    }
    catch (...) {
        std::cerr << "ERROR: unknown error" << std::endl;
        return 2;
    }

    return 0;
}
