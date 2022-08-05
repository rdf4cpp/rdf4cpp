#ifndef RDF4CPP_BFLC_HPP
#define RDF4CPP_BFLC_HPP

#include <rdf4cpp/rdf/ClosedNamespace.hpp>

namespace rdf4cpp::rdf::namespaces {

/**
 * A ClosedNamespace for http://id.loc.gov/ontologies/bflc/ version http://id.loc.gov/ontologies/bflc-1-2-0/
 */
class BFLC : public ClosedNamespace {
public:
    static const std::string NAMESPACE;
    static const std::vector<std::string> SUFFIXES;

    explicit BFLC(NodeStorage &node_storage = NodeStorage::default_instance());
};

}  // namespace rdf4cpp::rdf::namespaces

#endif  //RDF4CPP_BFLC_HPP
