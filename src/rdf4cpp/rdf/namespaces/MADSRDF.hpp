#ifndef RDF4CPP_MADSRDF_HPP
#define RDF4CPP_MADSRDF_HPP

#include <rdf4cpp/rdf/ClosedNamespace.hpp>

namespace rdf4cpp::rdf::namespaces {

/**
 * A ClosedNamespace for http://www.loc.gov/mads/rdf/v1# version http://id.loc.gov/ontologies/madsrdf/v1-4-0
 */
class MADSRDF : public ClosedNamespace {
public:
    static constexpr std::string_view NAMESPACE = "http://www.loc.gov/mads/rdf/v1#";
    static constexpr std::array<std::string_view, 154> SUFFIXES = {"activityEndDate", "activityStartDate", "associatedLanguage", "associatedLocale", "birthDate", "birthPlace", "deathDate", "deathPlace", "entityDescriptor", "establishDate", "fieldOfActivity", "gender", "hasAffiliation", "honoraryTitle", "isIdentifiedByAuthority", "occupation", "prominentFamilyMember", "terminateDate", "fullerName", "creationDateStart", "creationDateEnd", "workOrigin", "hasChararacteristic", "Address", "Affiliation", "Area", "Geographic", "Authority", "DeprecatedAuthority", "MADSCollection", "MADSScheme", "Variant", "City", "CitySection", "ComplexSubject", "SimpleType", "NameTitle", "HierarchicalGeographic", "ComplexType", "MADSType", "componentList", "ConferenceName", "Name", "Continent", "CorporateName", "Country", "County", "DateNameElement", "NameElement", "Element", "elementValue", "ExtraterrestrialArea", "FamilyName", "FamilyNameElement", "FullNameElement", "GenreForm", "GenreFormElement", "GeographicElement", "GivenNameElement", "State", "Region", "Identifier", "Island", "Language", "LanguageElement", "Collection", "MainTitleElement", "TitleElement", "Title", "NonSortElement", "Occupation", "PartNameElement", "PartNumberElement", "PersonalName", "Province", "RWO", "Source", "SubTitleElement", "Temporal", "TemporalElement", "TermsOfAddressNameElement", "Territory", "Topic", "TopicElement", "adminMetadata", "affiliationEnd", "affiliationStart", "authoritativeLabel", "changeNote", "note", "citationNote", "citationSource", "citationStatus", "city", "classification", "code", "country", "definitionNote", "deletionNote", "deprecatedLabel", "editorialNote", "elementList", "email", "exampleNote", "extendedAddress", "extension", "fax", "hasAbbreviationVariant", "hasVariant", "hasAcronymVariant", "hasAffiliationAddress", "hasBroaderAuthority", "hasRelatedAuthority", "hasNarrowerAuthority", "hasBroaderExternalAuthority", "hasCloseExternalAuthority", "hasCorporateParentAuthority", "hasCorporateSubsidiaryAuthority", "hasDemonym", "isDemonymFor", "hasEarlierEstablishedForm", "see", "hasLaterEstablishedForm", "hasExactExternalAuthority", "hasExpansionVariant", "hasHiddenVariant", "hasIdentifier", "hasMADSCollectionMember", "isMemberOfMADSCollection", "hasMADSSchemeMember", "isMemberOfMADSScheme", "hasNarrowerExternalAuthority", "hasReciprocalAuthority", "hasReciprocalExternalAuthority", "hasSource", "hasTopMemberOfMADSScheme", "hasTranslationVariant", "hiddenLabel", "historyNote", "hours", "identifiesRWO", "idScheme", "idValue", "isTopMemberOfMADSScheme", "natureOfAffiliation", "organization", "phone", "postcode", "scopeNote", "state", "streetAddress", "useFor", "useInstead", "variantLabel"};

    explicit MADSRDF(NodeStorage &node_storage = NodeStorage::default_instance());
};

}  // namespace rdf4cpp::rdf::namespaces

#endif  //RDF4CPP_MADSRDF_HPP
