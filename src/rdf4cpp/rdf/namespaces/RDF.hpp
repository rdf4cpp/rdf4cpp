#ifndef RDF4CPP_NAMESPACE_RDF_HPP
#define RDF4CPP_NAMESPACE_RDF_HPP

#include <rdf4cpp/rdf/ClosedNamespace.hpp>

namespace rdf4cpp::rdf::namespaces {

/**
 * A ClosedNamespace for http://www.w3.org/1999/02/22-rdf-syntax-ns# <br/>
 * All rdfs:ContainerMembershipProperty like rdf:_1, rdf:_2, rdf:_3, ... are also available.
 */
class RDF : public ClosedNamespace {
public:
    static const std::string NAMESPACE;
    static const std::vector<std::string> SUFFIXES;

    explicit RDF(NodeStorage &node_storage = NodeStorage::default_instance());

    IRI operator+(const std::string &suffix) override;
};

}  // namespace rdf4cpp::rdf::namespaces

#endif  //RDF4CPP_NAMESPACE_RDF_HPP
