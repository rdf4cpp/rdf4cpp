#ifndef RDF4CPP_BFLC_HPP
#define RDF4CPP_BFLC_HPP

#include <rdf4cpp/rdf/ClosedNamespace.hpp>

namespace rdf4cpp::rdf::namespaces {

/**
 * A ClosedNamespace for http://id.loc.gov/ontologies/bflc/ version http://id.loc.gov/ontologies/bflc-1-2-0/
 */
class BFLC : public ClosedNamespace {
public:
    static constexpr std::string_view NAMESPACE = "http://id.loc.gov/ontologies/bflc/";
    static constexpr std::array<std::string_view, 24> SUFFIXES = {"AppliesTo", "CreatorCharacteristic", "DemographicGroup", "Eidr", "EncodingLevel", "GrooveCutting", "ImageBitDepth", "MachineModel", "MetadataLicensor", "OperatingSystem", "PrimaryContribution", "ProgrammingLanguage", "Relation", "Relationship", "Relief", "SeriesAnalysis", "SeriesClassification", "SeriesNumbering", "SeriesNumberingPeculiarities", "SeriesProvider", "SeriesSequentialDesignation", "SeriesTracing", "SeriesTreatment", "TransliteratedTitle"};

    explicit BFLC(NodeStorage &node_storage = NodeStorage::default_instance());
};

}  // namespace rdf4cpp::rdf::namespaces

#endif  //RDF4CPP_BFLC_HPP
