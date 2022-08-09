#ifndef RDF4CPP_OWL_HPP
#define RDF4CPP_OWL_HPP

#include <rdf4cpp/rdf/ClosedNamespace.hpp>

namespace rdf4cpp::rdf::namespaces {

/**
 * A ClosedNamespace for http://www.w3.org/2002/07/owl#
 */
class OWL : public ClosedNamespace {
public:
    static constexpr std::string_view NAMESPACE = "http://www.w3.org/2002/07/owl#";
    static constexpr std::array<std::string_view, 72> SUFFIXES = {"Ontology", "versionIRI", "imports", "Class", "ObjectProperty", "DatatypeProperty", "AnnotationProperty", "NamedIndividual", "inverseOf", "intersectionOf", "unionOf", "datatypeComplementOf", "oneOf", "onDatatype", "withRestrictions", "complementOf", "Restriction", "onProperty", "someValuesFrom", "allValuesFrom", "hasValue", "hasSelf", "minCardinality", "minQualifiedCardinality", "onClass", "maxCardinality", "maxQualifiedCardinality", "cardinality", "qualifiedCardinality", "onProperties", "onDataRange", "equivalentClass", "disjointWith", "AllDisjointClasses", "members", "disjointUnionOf", "propertyChainAxiom", "equivalentProperty", "propertyDisjointWith", "AllDisjointProperties", "FunctionalProperty", "InverseFunctionalProperty", "ReflexiveProperty", "IrreflexiveProperty", "SymmetricProperty", "AsymmetricProperty", "TransitiveProperty", "hasKey", "sameAs", "differentFrom", "AllDifferent", "NegativePropertyAssertion", "sourceIndividual", "assertionProperty", "targetIndividual", "targetValue", "Annotation", "annotatedSource", "annotatedProperty", "annotatedTarget", "Axiom", "OntologyProperty", "DataRange", "Nothing", "Thing", "distinctMembers", "DeprecatedClass", "deprecated", "DeprecatedProperty", "subject", "predicate", "object"};

    explicit OWL(NodeStorage &node_storage = NodeStorage::default_instance());
};

}  // namespace rdf4cpp::rdf::namespaces

#endif  //RDF4CPP_OWL_HPP
