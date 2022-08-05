#ifndef RDF4CPP_OWL_HPP
#define RDF4CPP_OWL_HPP

#include <rdf4cpp/rdf/ClosedNamespace.hpp>

namespace rdf4cpp::rdf::namespaces {

/**
 * A ClosedNamespace for http://www.w3.org/2002/07/owl#
 */
class OWL : public ClosedNamespace {
public:
    static const std::string NAMESPACE;
    static const std::vector<std::string> SUFFIXES;

    explicit OWL(NodeStorage &node_storage = NodeStorage::default_instance());
};

}  // namespace rdf4cpp::rdf::namespaces

#endif  //RDF4CPP_OWL_HPP
