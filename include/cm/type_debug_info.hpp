// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file type_debug_info.hpp
/// Contains definition of the type_debug_info class.

#pragma once

#include <memory>


namespace cm {


/// Represents debug info for type in code model
class type_debug_info {
public:
    /// Constructor, makes debug info with specified type size
    explicit type_debug_info(unsigned int sz = 0):
        size_{sz} {}

    /// Destructor, destroys debug info
    virtual ~type_debug_info() = default;

    /// Returns type size
    auto size() const { return size_; }

    /// Sets type size
    void set_size(unsigned int sz) { size_ = sz; }

private:
    unsigned int size_;         ///< Type size
};


/// Shared pointer to type debug info
using type_debug_info_sp = std::shared_ptr<type_debug_info>;

/// Unique pointer to type debug info
using type_debug_info_up = std::unique_ptr<type_debug_info>;


}
