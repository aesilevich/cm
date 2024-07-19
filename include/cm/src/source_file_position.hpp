// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file source_file_position.hpp
/// Contains definition of the source_file_position class.

#pragma once

#include "source_position.hpp"
#include "../cm.hpp"
#include <charconv>
#include <sstream>


namespace cm::src {


/// Pair of source file and position in it
class source_file_position {
public:
    /// Constructs source file position
    source_file_position(const cm::source_file * file = nullptr, const source_position & p = {}):
        file_{file}, pos_{p} {}

    /// Returns true if source file position is valid
    bool is_valid() const { return file() != nullptr; }

    /// Returns true if source file position is valid
    explicit operator bool() const { return is_valid(); }

    /// Returns pointer to source file
    const cm::source_file * file() const { return file_; }

    /// Sets source file
    void set_file(const cm::source_file * file) { file_ = file; }

    /// Returns const reference to position in source file
    const source_position & pos() const { return pos_; }

    /// Returns reference to position in source file
    source_position & pos() { return pos_; }

    /// Sets pos in source file
    void set_pos(const source_position & p) { pos_ = p; }

    /// Prints source position to output stream
    void print(std::ostream & str) const {
        str << file_->path().string() << ':' << pos().line() << ':' << pos().column();
    }

private:
    const cm::source_file * file_;  ///< Pointer to source file object
    source_position pos_;           ///< Position in source file
};


/// Prints source file position to output stream
inline std::ostream & operator<<(std::ostream & str, const source_file_position & pos) {
    pos.print(str);
    return str;
}


/// Source file position descirption, contains path to source file and position.
class source_file_position_desc {
public:
    /// Constructs source file position description
    source_file_position_desc(const std::filesystem::path & path = {},
                              const source_position & pos = {}):
        path_{path}, pos_{pos} {}

    /// Returns true if source file position is valid
    bool is_valid() const { return !path().empty(); }

    /// Returns true if source file position is valid
    explicit operator bool() const { return is_valid(); }

    /// Returns path to source file
    const std::filesystem::path & path() const { return path_; }

    /// Sets path to source file
    void set_path(const std::filesystem::path & p) { path_ = p; }

    /// Returns const reference to position in source file
    const source_position & pos() const { return pos_; }

    /// Returns reference to position in source file
    source_position & pos() { return pos_; }

    /// Sets pos in source file
    void set_pos(const source_position & p) { pos_ = p; }

    /// Prints source position to output stream
    void print(std::ostream & str) const {
        str << path().string() << ':' << pos().line() << ':' << pos().column();
    }

    /// Parses source file position description from string. Throws exception on error
    static source_file_position_desc from_string(const std::string_view & str) {
        auto curr_str = str;
        source_file_position_desc desc;

        // parsing file path
        
        auto first_colon_pos = curr_str.find(':');
        if (first_colon_pos == std::string_view::npos) {
            std::ostringstream msg;
            msg << "invalid source file position '" << str << "': "
                << "can't find ':' character";
            throw std::runtime_error{msg.str()};
        }

        if (first_colon_pos == 0) {
            std::ostringstream msg;
            msg << "invalid source file position '" << str << "': "
                << "empty file path";
            throw std::runtime_error{msg.str()};
        }

        desc.set_path(std::filesystem::path{curr_str.substr(0, first_colon_pos)});


        // parsing line number

        curr_str = curr_str.substr(first_colon_pos + 1);
        
        auto line_num_end_pos = curr_str.find(':');
        if (line_num_end_pos == std::string_view::npos) {
            line_num_end_pos = curr_str.size();
        }

        if (line_num_end_pos == 0) {
            std::ostringstream msg;
            msg << "invalid source file position '" << str << "': "
                << "empty line number";
            throw std::runtime_error{msg.str()};
        }

        unsigned int line_num = 0;
        auto line_num_str = curr_str.substr(0, line_num_end_pos);
        if (!read_num(line_num_str, line_num)) {
            std::ostringstream msg;
            msg << "invalid source file position '" << str << "': "
                << "invalid line number: '" << line_num_str << "'";
            throw std::runtime_error{msg.str()};
        }

        // checking for empty column number
        if (line_num_end_pos == curr_str.size()) {
            desc.set_pos({line_num, 1});
            return desc;
        }


        // parsing column number

        unsigned int col_num = 0;
        curr_str = curr_str.substr(line_num_end_pos + 1);
        
        if (!read_num(curr_str, col_num)) {
            std::ostringstream msg;
            msg << "invalid source file position '" << str << "': "
                << "invalid column number: '" << curr_str << "'";
            throw std::runtime_error{msg.str()};
        }

        desc.set_pos({line_num, col_num});
        return desc;
    }

private:
    /// Reads line or column number from string
    static bool read_num(const std::string_view & str, unsigned int & num) {
        auto res = std::from_chars(str.data(), str.data() + str.size(), num);
        return res.ec == std::errc{} && res.ptr == str.data() + str.size();
    }

    std::filesystem::path path_;    ///< Path to source file
    source_position pos_;           ///< Position in source file
};


/// Prints source file position description to output stream
inline std::ostream & operator<<(std::ostream & str, const source_file_position_desc & pos) {
    pos.print(str);
    return str;
}


}
