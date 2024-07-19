// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file range_utils.hpp
/// Cotnains definition of utility functions for working with ranges.

#pragma once

#include <ranges>


namespace util {


/// Functor that returns raw pointer from smart pointer
struct get_ptr {
    template <typename SmartPtr>
    auto operator()(SmartPtr && p) const {
        return p.get();
    }
};


/// Functor that returns raw const pointer of second member of pair
struct get_second_ptr {
    template <typename Pair>
    auto operator()(Pair && p) const {
        return get_ptr()(p.second);
    }
};


/// Functor that returns raw const pointer from smart pointer
struct get_const_ptr {
    template <typename SmartPtr>
    auto operator()(SmartPtr && p) const {
        using pointee_type = std::decay_t<decltype(*p.get())>;
        return const_cast<const pointee_type*>(p.get());
    }
};


/// Functor that converts pointer to const pointer
struct get_const_raw_ptr {
    template <typename Ptr>
    auto operator()(Ptr && p) const {
        using pointee_type = std::decay_t<decltype(*p)>;
        return const_cast<const pointee_type*>(p);
    }
};


/// Functor that returns raw const pointer of second member of pair
struct get_const_second_ptr {
    template <typename Pair>
    auto operator()(Pair && p) const {
        return get_const_ptr()(p.second);
    }
};


/// Functor that returns const pointer of second member or pair which is non-const raw pointer
struct get_const_second_ptr_raw {
    template <typename Pair>
    auto operator()(Pair && p) const {
        return get_const_raw_ptr()(p.second);
    }
};


/// Constructs range of raw pointers from range of smart pointers
template <typename Range>
constexpr auto make_ptr_range(Range && r) {
    return r | std::ranges::views::transform(get_ptr());
}


/// Constructs range of raw const pointers from range of smart pointers
template <typename Range>
constexpr auto make_const_ptr_range(Range && r) {
    return r | std::ranges::views::transform(get_const_ptr());
}


/// Constructs range of raw const pointers from range of pointers
template <typename Range>
constexpr auto make_const_raw_ptr_range(Range && r) {
    return r | std::ranges::views::transform(get_const_raw_ptr());
}


/// Constructs range of raw pointers from range of pairs containing smart pointers
/// in the second field
template <typename Range>
constexpr auto make_second_ptr_range(Range && r) {
    return r | std::ranges::views::transform(get_second_ptr());
}


/// Constructs range of raw const pointers from range of pairs containing smart pointers
/// in the second field
template <typename Range>
constexpr auto make_const_second_ptr_range(Range && r) {
    return r | std::ranges::views::transform(get_const_second_ptr());
}


/// Constructs range of const pointers from range of pairs containing pointers in second element
template <typename Range>
constexpr auto make_const_second_raw_ptr_range(Range && r) {
    return r | std::ranges::views::transform(get_const_second_ptr_raw());
}


}
