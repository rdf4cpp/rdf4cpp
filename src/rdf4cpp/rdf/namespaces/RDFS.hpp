#ifndef RDF4CPP_RDFS_HPP
#define RDF4CPP_RDFS_HPP

#include <rdf4cpp/rdf/ClosedNamespace.hpp>

namespace rdf4cpp::rdf::namespaces {

/**
 * Prefix for https://www.w3.org/TR/rdf-schema/
 */
class RDFS : public ClosedNamespace {
public:
    static const std::string NAMESPACE;
    static const std::vector<std::string> SUFFIXES;

    explicit RDFS(NodeStorage &node_storage = NodeStorage::default_instance());
};

}  // namespace rdf4cpp::rdf::namespaces

#endif  //RDF4CPP_RDFS_HPP
