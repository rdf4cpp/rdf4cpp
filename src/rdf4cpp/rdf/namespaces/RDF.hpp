#ifndef RDF4CPP_NAMESPACE_RDF_HPP
#define RDF4CPP_NAMESPACE_RDF_HPP

#include <rdf4cpp/rdf/ClosedNamespace.hpp>

namespace rdf4cpp::rdf::namespaces {

/**
 * Prefix for http://www.w3.org/1999/02/22-rdf-syntax-ns#
 */
class RDF : public ClosedNamespace {
public:
    static const std::string NAMESPACE;
    static const std::vector<std::string> SUFFIXES;

    explicit RDF(NodeStorage &node_storage = NodeStorage::default_instance());
};

}  // namespace rdf4cpp::rdf::namespaces

#endif  //RDF4CPP_NAMESPACE_RDF_HPP
