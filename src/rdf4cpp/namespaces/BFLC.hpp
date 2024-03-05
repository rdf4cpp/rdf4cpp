#ifndef RDF4CPP_BFLC_HPP
#define RDF4CPP_BFLC_HPP

#include <rdf4cpp/ClosedNamespace.hpp>

namespace rdf4cpp::rdf::namespaces {

/**
 * A ClosedNamespace for http://id.loc.gov/ontologies/bflc/ version http://id.loc.gov/ontologies/bflc-1-2-0/
 */
struct BFLC : ClosedNamespace {
    static constexpr std::string_view prefix = "http://id.loc.gov/ontologies/bflc/";
    static constexpr std::array<std::string_view, 24> suffixes = {"AppliesTo", "CreatorCharacteristic", "DemographicGroup",
                                                                  "Eidr", "EncodingLevel", "GrooveCutting", "ImageBitDepth",
                                                                  "MachineModel", "MetadataLicensor", "OperatingSystem",
                                                                  "PrimaryContribution", "ProgrammingLanguage", "Relation",
                                                                  "Relationship", "Relief", "SeriesAnalysis", "SeriesClassification",
                                                                  "SeriesNumbering", "SeriesNumberingPeculiarities",
                                                                  "SeriesProvider", "SeriesSequentialDesignation", "SeriesTracing",
                                                                  "SeriesTreatment", "TransliteratedTitle"};

    explicit BFLC(storage::DynNodeStorage node_storage = storage::default_node_storage) : ClosedNamespace{prefix, suffixes, node_storage} {
    }
};

}  // namespace rdf4cpp::rdf::namespaces

#endif  //RDF4CPP_BFLC_HPP
