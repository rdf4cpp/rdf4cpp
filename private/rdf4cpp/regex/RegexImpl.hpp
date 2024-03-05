#ifndef RDF4CPP_RDF_UTIL_PRIVATE_REGEX_IMPL_HPP
#define RDF4CPP_RDF_UTIL_PRIVATE_REGEX_IMPL_HPP

#include <rdf4cpp/regex/Regex.hpp>

#include <re2/re2.h>

namespace rdf4cpp::rdf::regex {

struct Regex::Impl {
    re2::RE2 regex;
    Regex::flag_type flags;

    Impl(std::string_view regex, Regex::flag_type flags);
    [[nodiscard]] bool regex_match(std::string_view str) const noexcept;
    [[nodiscard]] bool regex_search(std::string_view str) const noexcept;
};

}  //namespace rdf4cpp::rdf::regex

#endif  //RDF4CPP_RDF_UTIL_PRIVATE_REGEX_IMPL_HPP
