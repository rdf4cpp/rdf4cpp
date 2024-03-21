#ifndef RDF4CPP_PREMIS3_HPP
#define RDF4CPP_PREMIS3_HPP

#include <rdf4cpp/ClosedNamespace.hpp>

namespace rdf4cpp::namespaces {

/**
 * A ClosedNamespace for http://www.loc.gov/premis/rdf/v3/ version https://id.loc.gov/ontologies/premis-3-0-0
 */
struct PREMIS3 : ClosedNamespace {
    static constexpr std::string_view prefix = "http://www.loc.gov/premis/rdf/v3/";
    static constexpr std::array<std::string_view, 68> suffixes = {"Action", "Agent", "Bitstream", "Copyright",
                                                                  "Dependency", "EnvironmentCharacteristic", "Event",
                                                                  "File", "Fixity", "HardwareAgent", "Identifier",
                                                                  "Inhibitor", "InstitutionalPolicy", "IntellectualEntity",
                                                                  "License", "Object", "Organization", "OutcomeStatus",
                                                                  "Person", "PreservationPolicy", "Representation",
                                                                  "RightsBasis", "RightsStatus", "Rule", "Signature",
                                                                  "SignatureEncoding", "SignificantProperties", "SoftwareAgent",
                                                                  "Statute", "StorageLocation", "StorageMedium", "act",
                                                                  "allows", "basis", "characteristic", "citation",
                                                                  "compositionLevel", "dependency", "determinationDate",
                                                                  "documentation", "encoding", "endDate", "fixity",
                                                                  "governs", "identifier", "inhibitedBy", "inhibits",
                                                                  "jurisdiction", "key", "medium", "note", "originalName",
                                                                  "outcome", "outcomeNote", "policy", "prohibits", "purpose",
                                                                  "rationale", "relationship", "restriction", "rightsStatus",
                                                                  "signature", "size", "startDate", "storedAt", "terms",
                                                                  "validationRules", "version"};

    explicit PREMIS3(storage::DynNodeStoragePtr node_storage = storage::default_node_storage) : ClosedNamespace{prefix, suffixes, node_storage} {
    }
};

}  // namespace rdf4cpp::namespaces

#endif  //RDF4CPP_PREMIS3_HPP
