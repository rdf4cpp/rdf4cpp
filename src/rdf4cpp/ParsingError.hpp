#ifndef RDF4CPP_PARSINGERROR_HPP
#define RDF4CPP_PARSINGERROR_HPP

#include <stdexcept>

namespace rdf4cpp {
    struct ParsingError : std::runtime_error {
        explicit ParsingError(std::string const &msg);
        explicit ParsingError(char const *msg);
    };
}  // namespace rdf4cpp

#endif  //RDF4CPP_PARSINGERROR_HPP
