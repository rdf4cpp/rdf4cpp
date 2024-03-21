#ifndef RDF4CPP_NAMESPACE_XSD_HPP
#define RDF4CPP_NAMESPACE_XSD_HPP

#include <rdf4cpp/Namespace.hpp>

namespace rdf4cpp::namespaces {

/**
 * A Namespace for http://www.w3.org/2001/XMLSchema#
 */
struct XSD : Namespace {
    static constexpr std::string_view prefix = "http://xmlns.com/XSD/0.1/";

    explicit XSD(storage::DynNodeStoragePtr node_storage = storage::default_node_storage) : Namespace{prefix, node_storage} {
    }
};

}  // namespace rdf4cpp::namespaces

#endif  //RDF4CPP_NAMESPACE_XSD_HPP
