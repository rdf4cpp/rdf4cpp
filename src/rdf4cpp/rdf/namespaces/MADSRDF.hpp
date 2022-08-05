#ifndef RDF4CPP_MADSRDF_HPP
#define RDF4CPP_MADSRDF_HPP

#include <rdf4cpp/rdf/ClosedNamespace.hpp>

namespace rdf4cpp::rdf::namespaces {

/**
 * A ClosedNamespace for http://www.loc.gov/mads/rdf/v1# version http://id.loc.gov/ontologies/madsrdf/v1-4-0
 */
class MADSRDF : public ClosedNamespace {
public:
    static const std::string NAMESPACE;
    static const std::vector<std::string> SUFFIXES;

    explicit MADSRDF(NodeStorage &node_storage = NodeStorage::default_instance());
};

}  // namespace rdf4cpp::rdf::namespaces

#endif  //RDF4CPP_MADSRDF_HPP
