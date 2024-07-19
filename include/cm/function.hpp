// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file function.hpp
/// Contains definition of the function class.

#pragma once

#include "context.hpp"
#include "function_type.hpp"
#include "named_context_entity.hpp"
#include "single_type_use.hpp"
#include "template_instantiation.hpp"
#include <list>
#include <sstream>


namespace cm {


class function;


/// Represents function parameter
class function_parameter: virtual public single_type_use, virtual public entity {
public:
    /// Constructs function parameter with specified pointer to function and type
    function_parameter(function * f, const qual_type & t);

    /// Returns pointer to function this parameter belongs to
    function * func();

    /// Returns const pointer to function this parameter belongs to
    const function * func() const;

    /// Prints function parameter desc to output stream
    void print_desc(std::ostream & str) const override {
        str << "(unnamed parameter)";
    }

    /// Dumps function parameter to output stream
    void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const override {
        type().print_desc(str);
    }

private:
    function * func_;           ///< Pointer to parent function
};


/// Represents function parameter with name
class named_function_parameter: public function_parameter, virtual public named_entity {
public:
    /// Constructs named function parameter with specified pointer to function,
    /// and parameter name and type
    named_function_parameter(function * f, const std::string & nm, const qual_type & t);

    /// Returns parameter name
    const std::string & name() const override { return name_; }

    /// Sets parameter name
    template <typename String>
    requires (std::assignable_from<std::string &, String>)
    void set_name(String && str) {
        name_ = std::forward<String>(str);
    }

    /// Prints function parameter desc to output stream
    void print_desc(std::ostream & str) const override {
        named_entity::print_desc(str);
    }

    /// Dumps function parameter to output stream
    void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const override {
        str << name() << ": ";
        type().print_desc(str);
    }

private:
    std::string name_;          ///< Parameter name
};


/// Represents function in code model
class function: virtual public context {
public:
    /// Constructs function with specified pointer to parent context
    /// and optional return type
    function(context * ctx):
    context{ctx} {
        assert(!dynamic_cast<function_type*>(ret_type_.type()) &&
               "function return type can't be a function type");

        if (ret_type_) {
            ret_type_->add_use(this);
        }
    }

    /// Destroys function. Removes use of function type
    ~function() override {
        if (ret_type_) {
            ret_type_->remove_use(this);
        }
    }

    /// Returns function return type
    qual_type ret_type() {
        return ret_type_;
    }

    /// Returns function const return type
    const_qual_type ret_type() const {
        return ret_type_;
    }

    /// Sets function return type. Removes use from the old
    /// type and add use for the new type.
    void set_ret_type(const qual_type & t) {
        if (ret_type_) {
            ret_type_->remove_use(this);
        }

        ret_type_ = t;

        if (ret_type_) {
            ret_type_->add_use(this);
        }
    }

    /// Adds unnamed function parameter with specified type
    void add_param(const qual_type & t);

    /// Adds named function parameter with specified name and type
    void add_param(const std::string & name, const qual_type & t);

    /// Removes function parameter
    void remove_param(function_parameter * par);

    /// Creates unnamed entity in function context
    template <typename Entity, typename ... Args>
    Entity * create_entity(Args && ... args) {
        return create_entity_impl<Entity>(this, std::forward<Args>(args)...);
    }

    /// Create named entity in function context
    template <typename Entity, typename ... Args>
    Entity * create_named_entity(const std::string & name, Args && ... args) {
        return create_named_entity_impl<Entity>(this, name, std::forward<Args>(args)...);
    }

    /// Returns range of const function parameters
    auto params() const {
        auto fn = [](auto && par) -> const function_parameter * { return par.get(); };
        return params_ | std::ranges::views::transform(fn);
    }

    /// Returns range of function parameters
    auto params() {
        auto fn = [](auto && par) -> function_parameter * { return par.get(); };
        return params_ | std::ranges::views::transform(fn);
    }

    /// Removes all parameters
    void remove_all_params() {
        params_.clear();
    }

    /// Dumps function to output stream
    void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const override;

    /// Dumps function parameters to output stream
    void dump_parameters(std::ostream & str) const;

    /// Dumps function parameters and return value
    void dump_parameters_and_ret_type(std::ostream & str) const;

protected:
    /// Dumps function with specified name to output stream
    void dump(std::ostream & str, unsigned int indent, const std::string_view & nm) const;

private:
    qual_type ret_type_;                        ///< Function return type

    /// List of function parameters
    std::list<std::unique_ptr<function_parameter>> params_;
};


/// Represents simple user defined function in code model
class named_function: virtual public function, virtual public named_context_entity {
public:
    /// Constructs named function with specified pointer to parent context,
    /// function name and return type
    named_function(context * ctx, const std::string & nm):
    function{ctx}, named_context_entity{ctx, nm}, context_entity{ctx}, context{ctx} {}

    /// Dumps function to output stream
    void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const override;
};


inline auto context::functions() const { return entities<function>(); }

inline auto context::functions() { return entities<function>(); }

inline auto context::named_functions() const { return entities<named_function>(); }

inline auto context::named_functions() { return entities<named_function>(); }


}
