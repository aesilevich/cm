// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file concat.hpp
/// Concat view implementation for ranges

#pragma once

#include <cassert>
#include <iterator>
#include <utility>
#include <variant>


namespace cm::ranges {


/// Return first type from type list
template <typename First, typename ... Types>
struct first_type {
    using type = First;
};

template <typename ... Types>
using first_type_t = typename first_type<Types...>::type;


/// Check for equality of multiple types
template <typename First, typename ... Types>
struct is_same_multi {
    static constexpr auto value = std::is_same_v<First, first_type_t<Types...>> && is_same_multi<Types...>::value;
};

template <typename First>
struct is_same_multi<First> {
    static constexpr auto value = true;
};

template <typename ... Types>
constexpr inline auto is_same_multi_v = is_same_multi<Types...>::value;


/// View that concatenates multiple views in single view
template <typename ... Sources>
#if __cplusplus > 201703L
requires ((std::ranges::input_range<Sources> && ...) && (std::ranges::view<Sources> && ...))
#endif
class concat_view: public std::ranges::view_base {
    using sources_tuple_t = std::tuple<Sources...>;
    using first_source_t = std::tuple_element_t<0, sources_tuple_t>;

public:
    /// Number of sources must be greater than zero
    static_assert(sizeof...(Sources) != 0);

    static_assert(is_same_multi_v<std::ranges::range_reference_t<Sources>...> ||
                  is_same_multi_v<std::ranges::range_value_t<Sources>...>,
                  "Reference types or value types for all source ranges must be the same");

    /// Iterator over range
    template <bool Const>
    class iterator_t {
        using sources_tuple_t = std::conditional_t<Const, std::tuple<const Sources...>, std::tuple<Sources...>>;
        using first_source_t = std::tuple_element_t<0, sources_tuple_t>;
        using iterator_variant_t = std::conditional_t <
            Const,
            std::variant<std::ranges::iterator_t<const Sources>...>,
            std::variant<std::ranges::iterator_t<Sources>...>
        >;

        using view_t = std::conditional_t<Const, const concat_view<Sources...>, concat_view<Sources...>>;

    public:
        /// Iterator category
        using iterator_category = std::forward_iterator_tag;

        /// Iterator value type
        using value_type = std::ranges::range_value_t<first_source_t>;

        /// Iterator difference type
        using difference_type = ptrdiff_t;

        /// Iterator reference type
        using reference = std::conditional_t <
            is_same_multi_v<std::ranges::range_reference_t<Sources>...>,
            std::ranges::range_reference_t<first_source_t>,
            std::ranges::range_value_t<first_source_t>
        >;

        /// Iterator pointer type
        using pointer = value_type *;

        /// Default constructor
        iterator_t() = default;

        /// Constructs iterator with specified pointer to concat view.
        /// Sets position to the first element in range
        explicit iterator_t(view_t * v):
        view_{v} {
            set_src_begin<0>();
        }

        /// Constructs iterator with specified pointer to concat view. Sets position
        /// to the end of last range
        explicit iterator_t(view_t * v, bool):
        view_{v} {
            constexpr auto last_idx = sizeof...(Sources) - 1;
            src_idx_ = last_idx;
            it_.template emplace<last_idx>(std::ranges::end(std::get<last_idx>(view_->sources())));
        }

        /// Returns reference to current element
        reference operator*() const {
            return dereference<0>();
        }

        /// Moves iterator to next element
        auto & operator++() {
            move_next<0>();
            return *this;
        }

        /// Moves iterator to next element. Returns iterator pointing to previous element
        auto operator++(int) {
            auto curr = *this;
            ++(*this);
            return curr;
        }

        /// Compares this iterator with other
        bool operator==(const iterator_t<Const> & other) const {
            return view_ == other.view_ &&
                   src_idx_ == other.src_idx_ &&
                   it_ == other.it_;
        }

        /// Compares this iterator with other
        bool operator!=(const iterator_t<Const> & other) const {
            return !(*this == other);
        }

    private:
        /// Tries dereference current iterator with specified source index. Recursively invokes
        /// this function for rest of indexes if current index is not equal to index
        template <size_t Idx>
        reference dereference() const {
            if (Idx == src_idx_) {
                return *(std::get<Idx>(it_));
            } else {
                if constexpr (Idx < sizeof...(Sources) - 1) {
                    return dereference<Idx + 1>();
                } else {
                    assert(false && "invalid current source index value");
                    return *(std::get<0>(it_));
                }
            }
        }

        /// Tries move iterator to the next element in range for current source range
        /// index equal to specified template parameter
        template <size_t Idx>
        void move_next() {
            if (Idx == src_idx_) {
                auto & src_view = std::get<Idx>(view_->sources());
                auto src_it = std::get<Idx>(it_);
                ++src_it;

                if (src_it == std::ranges::end(src_view)) {
                    if constexpr (Idx < sizeof...(Sources) - 1) {
                        set_src_begin<Idx + 1>();
                    } else {
                        // end of all sources reached. Saving
                        // end iterator for last source in current iterator
                        it_.template emplace<Idx>(src_it);
                    }
                } else {
                    it_.template emplace<Idx>(src_it);
                }

            } else {
                if constexpr (Idx < sizeof...(Sources) - 1) {
                    move_next<Idx + 1>();
                } else {
                    assert(false && "invalid current source index value");
                }
            }
        }


        /// Moves iterator to the first element in source view with specified index.
        /// If view is empty then performs same operation for source view with the next index.
        template <size_t Idx>
        void set_src_begin() {
            auto & src_view = std::get<Idx>(view_->sources());
            auto src_it = std::ranges::begin(src_view);

            if (src_it != std::ranges::end(src_view)) {
                it_.template emplace<Idx>(src_it);
                src_idx_ = Idx;
            } else {
                if constexpr (Idx < sizeof...(Sources) - 1) {
                    set_src_begin<Idx + 1>();
                } else {
                    // end of all sources reached. Saving
                    // end iterator for last source in current iterator
                    it_.template emplace<Idx>(src_it);
                    src_idx_ = Idx;
                }
            }
        }

        view_t * view_ = nullptr;               ///< Pointer to concat view
        size_t src_idx_ = SIZE_MAX;             ///< Index of current source
        iterator_variant_t it_;                 ///< Current iterator in current view
    };

    using iterator = iterator_t<false>;
    using const_iterator = iterator_t<true>;

    /// Constructs empty view
    concat_view() = default;

    /// Constructs view with list of sources
    explicit concat_view(Sources ... sources):
        srcs_{std::move(sources)...} {}
    
    /// Constructs view from tuple of sources
    explicit concat_view(std::tuple<Sources...> srcs):
        srcs_{std::move(srcs)} {}

    /// Copy constructor
    concat_view(const concat_view &) = default;

    /// Move constructor
    concat_view(concat_view &&) = default;

    /// Copy assignment operator
    concat_view & operator=(const concat_view &) = default;

    /// Move assignment operator
    concat_view & operator=(concat_view &&) = default;

    /// Returns reference to tuple of source views
    auto & sources() { return srcs_; }

    /// Returns const reference to tuple of source views
    auto & sources() const { return srcs_; }

    /// Returns size of range
    auto size() const
#if __cplusplus > 201703L
    requires (std::ranges::sized_range<Sources> && ...)
#endif
    {
        return tuple_range_size<0>();
    }

    /// Returns iterator pointing to the first element in range
    auto begin() {
        return iterator{this};
    }

    /// Returns iterator pointing to one past end element in range
    auto end() {
        constexpr auto last_idx = sizeof...(Sources) - 1;
        return iterator{this, true};
    }


    /// Returns iterator pointing to the first element in range
    auto begin() const {
        return const_iterator{this};
    }

    /// Returns iterator pointing to one past end element in range
    auto end() const {
        constexpr auto last_idx = sizeof...(Sources) - 1;
        return const_iterator{this, true};
    }


private:
    /// Returns sum of size of ranges in source tuple starting from specified index
    template <size_t Idx>
    size_t tuple_range_size() const
#if __cplusplus > 201703L
    requires (std::ranges::sized_range<Sources> && ...)
#endif
    {
        if constexpr (Idx == std::tuple_size_v<std::tuple<Sources...>>) {
            return 0;
        } else {
            return std::ranges::size(std::get<Idx>(srcs_)) + tuple_range_size<Idx + 1>();
        }
    }

    std::tuple<Sources...> srcs_;
};


struct concat_fn {
    template <typename ... Sources>
#if __cplusplus > 201703L
    requires (std::ranges::input_range<Sources> && ...)
#endif
    constexpr auto operator()(Sources && ... srcs) const {
        return concat_view{(std::forward<Sources>(srcs) | std::ranges::views::all)...};
    }
};


template <typename First, typename ... Sources>
#if __cplusplus > 201703L
requires (std::ranges::input_range<First>)
#endif
constexpr auto operator|(First && first, const concat_view<Sources...> & cv) {
    auto srcs = std::tuple_cat(std::tuple{first | std::ranges::views::all}, cv.sources());
    return concat_view{std::move(srcs)};
}


namespace views {
    constexpr concat_fn concat = concat_fn{};
}


}
