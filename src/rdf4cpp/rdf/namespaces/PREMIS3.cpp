#include "PREMIS3.hpp"

namespace rdf4cpp::rdf::namespaces {

const std::string PREMIS3::NAMESPACE = "http://www.loc.gov/premis/rdf/v3/";
const std::vector<std::string> PREMIS3::SUFFIXES = {"Action", "Agent", "Bitstream", "Copyright", "Dependency", "EnvironmentCharacteristic", "Event", "File", "Fixity", "HardwareAgent", "Identifier", "Inhibitor", "InstitutionalPolicy", "IntellectualEntity", "License", "Object", "Organization", "OutcomeStatus", "Person", "PreservationPolicy", "Representation", "RightsBasis", "RightsStatus", "Rule", "Signature", "SignatureEncoding", "SignificantProperties", "SoftwareAgent", "Statute", "StorageLocation", "StorageMedium", "act", "allows", "basis", "characteristic", "citation", "compositionLevel", "dependency", "determinationDate", "documentation", "encoding", "endDate", "fixity", "governs", "identifier", "inhibitedBy", "inhibits", "jurisdiction", "key", "medium", "note", "originalName", "outcome", "outcomeNote", "policy", "prohibits", "purpose", "rationale", "relationship", "restriction", "rightsStatus", "signature", "size", "startDate", "storedAt", "terms", "validationRules", "version"};
PREMIS3::PREMIS3(Namespace::NodeStorage &node_storage) : ClosedNamespace(NAMESPACE, SUFFIXES, node_storage) {}

}  // namespace rdf4cpp::rdf::namespaces