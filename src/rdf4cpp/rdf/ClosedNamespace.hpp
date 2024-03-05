#ifndef RDF4CPP_CLOSEDNAMESPACE_HPP
#define RDF4CPP_CLOSEDNAMESPACE_HPP

#include <rdf4cpp/rdf/Namespace.hpp>

namespace rdf4cpp::rdf {

/**
 * A ClosedNamespace can not be extended to more IRIs after construction.
 */
class ClosedNamespace : public Namespace {
public:
    /**
     * Constructs the ClosedNamespace with the fixed set of possible suffixes.
     * @tparam Suffixes Some range that has values convertible to std::string.
     * @param namespace_iri namespace IRI string. This will be used as prefix. IRI must not be encapsulated in <..>.
     * @param all_suffixes A range with all possible suffixes for this ClosedNamespace.
     * @param node_storage where the IRIs will live
     */
    template<typename Suffixes>
        requires std::convertible_to<std::ranges::range_value_t<Suffixes>, std::string_view>
    ClosedNamespace(std::string_view namespace_iri, Suffixes all_suffixes, storage::DynNodeStorage node_storage)
        : Namespace(namespace_iri, node_storage) {
        for (auto const &suffix : all_suffixes)
            this->cache_.emplace(suffix, IRI{namespace_iri_ + std::string{suffix}, node_storage}.backend_handle());
    }

    /**
     * Create an IRI with the suffix added to the ClosedNamespace.
     * @param suffix suffix that is appended
     * @return the constructed IRI
     * @throws std::runtime_exception if a suffix is available within the namespace
     */
    IRI operator+(std::string_view suffix) const override;

    /**
     * Clear has no effect on a ClosedNamespace.
     */
    void clear() const override;
};

}  // namespace rdf4cpp::rdf

#endif  //RDF4CPP_CLOSEDNAMESPACE_HPP
