#ifndef RDF4CPP_RDF_UTIL_PRIVATE_REGEXREPLACER_IMPL_HPP
#define RDF4CPP_RDF_UTIL_PRIVATE_REGEXREPLACER_IMPL_HPP

#include <rdf4cpp/regex/RegexReplacer.hpp>
#include "RegexImpl.hpp"

namespace rdf4cpp::rdf::regex {

struct RegexReplacer::Impl {
    Regex::Impl const *regex;
    std::string rewrite;

    Impl(Regex::Impl const &regex, std::string_view rewrite);
    void regex_replace(std::string &str) const noexcept;
};

}  //namespace rdf4cpp::rdf::regex

#endif   //RDF4CPP_RDF_UTIL_PRIVATE_REGEXREPLACER_IMPL_HPP
