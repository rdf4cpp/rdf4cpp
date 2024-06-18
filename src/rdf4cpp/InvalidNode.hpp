#ifndef RDF4CPP_INVALIDNODE_HPP
#define RDF4CPP_INVALIDNODE_HPP

#include <stdexcept>

namespace rdf4cpp {
    struct InvalidNode : std::runtime_error {
        explicit InvalidNode(std::string const &msg)
            : runtime_error(msg) {
        }
        explicit InvalidNode(char const *msg)
            : runtime_error(msg) {
        }
    };
}  // namespace rdf4cpp

#endif  //RDF4CPP_INVALIDNODE_HPP
