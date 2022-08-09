#ifndef RDF4CPP_NAMESPACE_XSD_HPP
#define RDF4CPP_NAMESPACE_XSD_HPP

#include <rdf4cpp/rdf/Namespace.hpp>

namespace rdf4cpp::rdf::namespaces {

/**
 * A Namespace for http://www.w3.org/2001/XMLSchema#
 */
class XSD : public Namespace {
public:
    static constexpr std::string_view NAMESPACE = "http://xmlns.com/XSD/0.1/";

    explicit XSD(NodeStorage &node_storage = NodeStorage::default_instance());
};

}  // namespace rdf4cpp::rdf::namespaces

#endif  //RDF4CPP_NAMESPACE_XSD_HPP
