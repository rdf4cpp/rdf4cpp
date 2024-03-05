#ifndef RDF4CPP_OWL_HPP
#define RDF4CPP_OWL_HPP

#include <rdf4cpp/ClosedNamespace.hpp>

namespace rdf4cpp::namespaces {

/**
 * A ClosedNamespace for http://www.w3.org/2002/07/owl#
 */
struct OWL : ClosedNamespace {
    static constexpr std::string_view prefix = "http://www.w3.org/2002/07/owl#";
    static constexpr std::array<std::string_view, 83> suffixes = {"AllDifferent", "AllDisjointClasses", "AllDisjointProperties",
                                                                  "allValuesFrom", "annotatedProperty", "annotatedSource",
                                                                  "annotatedTarget", "Annotation", "AnnotationProperty",
                                                                  "assertionProperty", "AsymmetricProperty", "Axiom",
                                                                  "backwardCompatibleWith", "bottomDataProperty", "bottomObjectProperty",
                                                                  "cardinality", "Class", "complementOf", "DataRange",
                                                                  "datatypeComplementOf", "DatatypeProperty", "dateTime",
                                                                  "deprecated", "DeprecatedClass", "DeprecatedProperty",
                                                                  "differentFrom", "disjointUnionOf", "disjointWith",
                                                                  "distinctMembers", "equivalentClass", "equivalentProperty",
                                                                  "FunctionalProperty", "hasKey", "hasSelf", "hasValue",
                                                                  "imports", "incompatibleWith", "intersectionOf",
                                                                  "InverseFunctionalProperty", "inverseOf", "IrreflexiveProperty",
                                                                  "maxCardinality", "maxQualifiedCardinality", "members", "minCardinality",
                                                                  "minQualifiedCardinality", "NamedIndividual", "NegativePropertyAssertion",
                                                                  "Nothing", "object", "ObjectProperty", "onClass", "onDataRange", "onDatatype",
                                                                  "oneOf", "onProperties", "onProperty", "Ontology", "OntologyProperty",
                                                                  "predicate", "priorVersion", "propertyChainAxiom", "propertyDisjointWith",
                                                                  "qualifiedCardinality", "rational", "real", "ReflexiveProperty",
                                                                  "Restriction", "sameAs", "someValuesFrom", "sourceIndividual",
                                                                  "subject", "SymmetricProperty", "targetIndividual", "targetValue",
                                                                  "Thing", "topDataProperty", "topObjectProperty", "TransitiveProperty",
                                                                  "unionOf", "versionInfo", "versionIRI", "withRestrictions"};

    explicit OWL(storage::DynNodeStorage node_storage = storage::default_node_storage) : ClosedNamespace{prefix, suffixes, node_storage} {
    }
};

}  // namespace rdf4cpp::namespaces

#endif  //RDF4CPP_OWL_HPP
