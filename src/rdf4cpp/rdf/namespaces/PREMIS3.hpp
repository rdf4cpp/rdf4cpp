#ifndef RDF4CPP_PREMIS3_HPP
#define RDF4CPP_PREMIS3_HPP

#include <rdf4cpp/rdf/ClosedNamespace.hpp>

namespace rdf4cpp::rdf::namespaces {

/**
 * A ClosedNamespace for http://www.loc.gov/premis/rdf/v3/ version https://id.loc.gov/ontologies/premis-3-0-0
 */
class PREMIS3 : public ClosedNamespace {
public:
    static const std::string NAMESPACE;
    static const std::vector<std::string> SUFFIXES;

    explicit PREMIS3(NodeStorage &node_storage = NodeStorage::default_instance());
};

}  // namespace rdf4cpp::rdf::namespaces

#endif  //RDF4CPP_PREMIS3_HPP
