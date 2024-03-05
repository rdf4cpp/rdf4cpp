#ifndef RDF4CPP_RDF_REGEX_REGEXERROR_HPP
#define RDF4CPP_RDF_REGEX_REGEXERROR_HPP

#include <stdexcept>
#include <string>

namespace rdf4cpp::regex {

/**
 * The type of exception thrown by functions in the rdf4cpp::regex library.
 */
struct RegexError : std::runtime_error {
    explicit RegexError(char const *msg) : std::runtime_error{msg} {}
    explicit RegexError(std::string const &msg) : std::runtime_error{msg} {}
};

}  //namespace rdf4cpp::regex

#endif  //RDF4CPP_RDF_REGEX_REGEXERROR_HPP
