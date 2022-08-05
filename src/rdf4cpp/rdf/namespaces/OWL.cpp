#include "OWL.hpp"

namespace rdf4cpp::rdf::namespaces {

const std::string OWL::NAMESPACE = "http://www.w3.org/2002/07/owl#";
const std::vector<std::string> OWL::SUFFIXES = {"Ontology", "versionIRI", "imports", "Class", "ObjectProperty", "DatatypeProperty", "AnnotationProperty", "NamedIndividual", "inverseOf", "intersectionOf", "unionOf", "datatypeComplementOf", "oneOf", "onDatatype", "withRestrictions", "complementOf", "Restriction", "onProperty", "someValuesFrom", "allValuesFrom", "hasValue", "hasSelf", "minCardinality", "minQualifiedCardinality", "onClass", "maxCardinality", "maxQualifiedCardinality", "cardinality", "qualifiedCardinality", "onProperties", "onDataRange", "equivalentClass", "disjointWith", "AllDisjointClasses", "members", "disjointUnionOf", "propertyChainAxiom", "equivalentProperty", "propertyDisjointWith", "AllDisjointProperties", "FunctionalProperty", "InverseFunctionalProperty", "ReflexiveProperty", "IrreflexiveProperty", "SymmetricProperty", "AsymmetricProperty", "TransitiveProperty", "hasKey", "sameAs", "differentFrom", "AllDifferent", "NegativePropertyAssertion", "sourceIndividual", "assertionProperty", "targetIndividual", "targetValue", "Annotation", "annotatedSource", "annotatedProperty", "annotatedTarget", "Axiom", "OntologyProperty", "DataRange", "Nothing", "Thing", "distinctMembers", "DeprecatedClass", "deprecated", "DeprecatedProperty", "subject", "predicate", "object"};
OWL::OWL(Namespace::NodeStorage &node_storage) : ClosedNamespace(NAMESPACE, SUFFIXES, node_storage) {}

}  // namespace rdf4cpp::rdf::namespaces