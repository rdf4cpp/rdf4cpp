#include "BFLC.hpp"

namespace rdf4cpp::rdf::namespaces {

const std::string BFLC::NAMESPACE = "http://id.loc.gov/ontologies/bflc/";
const std::vector<std::string> BFLC::SUFFIXES = {"AppliesTo", "CreatorCharacteristic", "DemographicGroup", "Eidr", "EncodingLevel", "GrooveCutting", "ImageBitDepth", "MachineModel", "MetadataLicensor", "OperatingSystem", "PrimaryContribution", "ProgrammingLanguage", "Relation", "Relationship", "Relief", "SeriesAnalysis", "SeriesClassification", "SeriesNumbering", "SeriesNumberingPeculiarities", "SeriesProvider", "SeriesSequentialDesignation", "SeriesTracing", "SeriesTreatment", "TransliteratedTitle"};
BFLC::BFLC(Namespace::NodeStorage &node_storage) : ClosedNamespace(NAMESPACE, SUFFIXES, node_storage) {}

}  // namespace rdf4cpp::rdf::namespaces