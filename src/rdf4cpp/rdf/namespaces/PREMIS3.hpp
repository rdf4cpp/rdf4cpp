#ifndef RDF4CPP_PREMIS3_HPP
#define RDF4CPP_PREMIS3_HPP

#include <rdf4cpp/rdf/ClosedNamespace.hpp>
#include <rdf4cpp/rdf/namespaces/util/NamespaceInstance.hpp>

namespace rdf4cpp::rdf::namespaces {

/**
 * A ClosedNamespace for http://www.loc.gov/premis/rdf/v3/ version https://id.loc.gov/ontologies/premis-3-0-0
 */
class PREMIS3 : public ClosedNamespace, public util::NamespaceInstance<PREMIS3> {
public:
    static constexpr std::string_view NAMESPACE = "http://www.loc.gov/premis/rdf/v3/";
    static constexpr std::array<std::string_view, 68> SUFFIXES = {"Action", "Agent", "Bitstream", "Copyright", "Dependency", "EnvironmentCharacteristic", "Event", "File", "Fixity", "HardwareAgent", "Identifier", "Inhibitor", "InstitutionalPolicy", "IntellectualEntity", "License", "Object", "Organization", "OutcomeStatus", "Person", "PreservationPolicy", "Representation", "RightsBasis", "RightsStatus", "Rule", "Signature", "SignatureEncoding", "SignificantProperties", "SoftwareAgent", "Statute", "StorageLocation", "StorageMedium", "act", "allows", "basis", "characteristic", "citation", "compositionLevel", "dependency", "determinationDate", "documentation", "encoding", "endDate", "fixity", "governs", "identifier", "inhibitedBy", "inhibits", "jurisdiction", "key", "medium", "note", "originalName", "outcome", "outcomeNote", "policy", "prohibits", "purpose", "rationale", "relationship", "restriction", "rightsStatus", "signature", "size", "startDate", "storedAt", "terms", "validationRules", "version"};

    explicit PREMIS3(storage::node::DynNodeStorage node_storage = storage::node::default_node_storage);
};

}  // namespace rdf4cpp::rdf::namespaces

#endif  //RDF4CPP_PREMIS3_HPP
