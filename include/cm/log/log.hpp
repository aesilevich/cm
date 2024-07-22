// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file log.hpp
/// Main include file for logging utilities in code model library.

#include <boost/log/trivial.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>


namespace cm::log {


using logger_t = boost::log::sources::severity_logger_mt<boost::log::trivial::severity_level>;

/// Returns reference to logger
inline logger_t & get_logger() {
    static logger_t logger;
    return logger;
}

#define CM_LOG(cat, level) \
    BOOST_LOG_SEV(::cm::log::get_logger(), level) \
        << ::boost::log::add_value("Category", #cat)

#define CM_LOG_TRACE(cat)       CM_LOG(cat, ::boost::log::trivial::trace)
#define CM_LOG_DEBUG(cat)       CM_LOG(cat, ::boost::log::trivial::debug)
#define CM_LOG_INFO(cat)        CM_LOG(cat, ::boost::log::trivial::info)
#define CM_LOG_WARNING(cat)     CM_LOG(cat, ::boost::log::trivial::warning)
#define CM_LOG_ERROR(cat)       CM_LOG(cat, ::boost::log::trivial::error)
#define CM_LOG_FATAL(cat)       CM_LOG(cat, ::boost::log::trivial::fatal)


#define CM_LOG_SCAT(cat, scat, level) \
    BOOST_LOG_SEV(::cm::log::get_logger(), level) \
        << ::boost::log::add_value("Category", #cat) \
        << ::boost::log::add_value("Subcategory", #scat)

#define CM_LOG_SCAT_TRACE(cat, scat)    CM_LOG_SCAT(cat, scat, ::boost::log::trivial::trace)
#define CM_LOG_SCAT_DEBUG(cat, scat)    CM_LOG_SCAT(cat, scat, ::boost::log::trivial::debug)
#define CM_LOG_SCAT_INFO(cat, scat)     CM_LOG_SCAT(cat, scat, ::boost::log::trivial::info)
#define CM_LOG_SCAT_WARNING(cat, scat)  CM_LOG_SCAT(cat, scat, ::boost::log::trivial::warning)
#define CM_LOG_SCAT_ERROR(cat, scat)    CM_LOG_SCAT(cat, scat, ::boost::log::trivial::error)
#define CM_LOG_SCAT_FATAL(cat, scat)    CM_LOG_SCAT(cat, scat, ::boost::log::trivial::fatal)


}
