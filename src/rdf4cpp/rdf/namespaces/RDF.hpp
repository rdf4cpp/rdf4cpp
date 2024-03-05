#ifndef RDF4CPP_NAMESPACE_RDF_HPP
#define RDF4CPP_NAMESPACE_RDF_HPP

#include <rdf4cpp/rdf/ClosedNamespace.hpp>
#include <rdf4cpp/rdf/namespaces/util/NamespaceInstance.hpp>

namespace rdf4cpp::rdf::namespaces {

/**
 * A ClosedNamespace for http://www.w3.org/1999/02/22-rdf-syntax-ns# <br/>
 * All rdfs:ContainerMembershipProperty like rdf:_1, rdf:_2, rdf:_3, ... are also available.
 */
class RDF : public ClosedNamespace, public util::NamespaceInstance<RDF> {
public:
    static constexpr std::string_view NAMESPACE = "http://www.w3.org/1999/02/22-rdf-syntax-ns#";
    static constexpr std::array<std::string_view, 22> SUFFIXES = {"HTML", "langString", "PlainLiteral", "type", "Property", "Statement", "subject", "predicate", "object", "Bag", "Seq", "Alt", "value", "List", "nil", "first", "rest", "XMLLiteral", "JSON", "CompoundLiteral", "language", "direction"};

    explicit RDF(storage::DynNodeStorage node_storage = storage::default_node_storage);

    IRI operator+(std::string_view suffix) const override;
};

}  // namespace rdf4cpp::rdf::namespaces

#endif  //RDF4CPP_NAMESPACE_RDF_HPP
