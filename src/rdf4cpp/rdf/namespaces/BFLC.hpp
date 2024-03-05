#ifndef RDF4CPP_BFLC_HPP
#define RDF4CPP_BFLC_HPP

#include <rdf4cpp/rdf/ClosedNamespace.hpp>
#include <rdf4cpp/rdf/namespaces/util/NamespaceInstance.hpp>

namespace rdf4cpp::rdf::namespaces {

/**
 * A ClosedNamespace for http://id.loc.gov/ontologies/bflc/ version http://id.loc.gov/ontologies/bflc-1-2-0/
 */
class BFLC : public ClosedNamespace, public util::NamespaceInstance<BFLC> {
public:
    static constexpr std::string_view NAMESPACE = "http://id.loc.gov/ontologies/bflc/";
    static constexpr std::array<std::string_view, 24> SUFFIXES = {"AppliesTo", "CreatorCharacteristic", "DemographicGroup", "Eidr", "EncodingLevel", "GrooveCutting", "ImageBitDepth", "MachineModel", "MetadataLicensor", "OperatingSystem", "PrimaryContribution", "ProgrammingLanguage", "Relation", "Relationship", "Relief", "SeriesAnalysis", "SeriesClassification", "SeriesNumbering", "SeriesNumberingPeculiarities", "SeriesProvider", "SeriesSequentialDesignation", "SeriesTracing", "SeriesTreatment", "TransliteratedTitle"};

    explicit BFLC(storage::DynNodeStorage node_storage = storage::default_node_storage);
};

}  // namespace rdf4cpp::rdf::namespaces

#endif  //RDF4CPP_BFLC_HPP
