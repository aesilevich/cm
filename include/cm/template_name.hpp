// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_name.hpp
/// Contains definition of the template_name class.

#pragma once


namespace cm {


/// Represents a template name, i.e. a reference to a template.
/// Can be real template or dependent template name
class template_name: virtual public context_entity {
public:
    /// Returns template name as string
    virtual const std::string & name() const = 0;
};


}
