#ifndef RDF4CPP_FOAF_HPP
#define RDF4CPP_FOAF_HPP

#include <rdf4cpp/rdf/ClosedNamespace.hpp>

namespace rdf4cpp::rdf::namespaces {

/**
 * A ClosedNamespace for http://xmlns.com/foaf/0.1/ version https://xmlns.com/foaf/spec/20140114.rdf
 */
class FOAF : public ClosedNamespace {
public:
    static const std::string NAMESPACE;
    static const std::vector<std::string> SUFFIXES;

    explicit FOAF(NodeStorage &node_storage = NodeStorage::default_instance());
};

}  // namespace rdf4cpp::rdf::namespaces

#endif  //RDF4CPP_FOAF_HPP
