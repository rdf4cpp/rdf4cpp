#ifndef RDF4CPP_RDF_BNODEMANAGEMENT_REFERENCEBACKENDS_SCOPE_REFERENCESKOLEMIRISCOPE_HPP
#define RDF4CPP_RDF_BNODEMANAGEMENT_REFERENCEBACKENDS_SCOPE_REFERENCESKOLEMIRISCOPE_HPP

#include <rdf4cpp/rdf/bnode_management/reference_backends/factory/SkolemIRIFactory.hpp>
#include <rdf4cpp/rdf/bnode_management/reference_backends/scope/ReferenceScopeBase.hpp>

namespace rdf4cpp::rdf::util {

/**
 * Reference implementation for a Skolem IRI scope.
 * The generated Skolem IRIs are not explicitly scoped to this scope
 * in the node storage.
 */
struct ReferenceSkolemIRIScope final : SkolemIRIFactory, ReferenceScopeBase<ReferenceSkolemIRIScope> {
private:
    friend ReferenceScopeBase<ReferenceSkolemIRIScope>;
    ReferenceSkolemIRIScope(ReferenceSkolemIRIScope const &other) = default;
public:
    inline ReferenceSkolemIRIScope(SkolemIRIFactory const &other) noexcept : SkolemIRIFactory{other} {
    }
    inline explicit ReferenceSkolemIRIScope(std::string iri_prefix) noexcept : SkolemIRIFactory{std::move(iri_prefix)} {
    }

    ReferenceSkolemIRIScope(ReferenceSkolemIRIScope &&other) noexcept = default;
};

}  //namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_RDF_BNODEMANAGEMENT_REFERENCEBACKENDS_SCOPE_REFERENCESKOLEMIRISCOPE_HPP
