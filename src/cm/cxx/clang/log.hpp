// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file log.hpp
/// Contains macro definitions for logging in clang code model converter

#pragma once

#include "cm/log/log.hpp"


#define CM_CLANG_LOG_TRACE          CM_LOG(cm-cxx-clang, ::boost::log::trivial::trace)
#define CM_CLANG_LOG_DEBUG          CM_LOG(cm-cxx-clang, ::boost::log::trivial::debug)
#define CM_CLANG_LOG_INFO           CM_LOG(cm-cxx-clang, ::boost::log::trivial::info)
#define CM_CLANG_LOG_WARNING        CM_LOG(cm-cxx-clang, ::boost::log::trivial::warning)
#define CM_CLANG_LOG_ERROR          CM_LOG(cm-cxx-clang, ::boost::log::trivial::error)
#define CM_CLANG_LOG_FATAL          CM_LOG(cm-cxx-clang, ::boost::log::trivial::fatal)
