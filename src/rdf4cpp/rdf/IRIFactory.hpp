#ifndef RDF4CPP_IRIFACTORY_HPP
#define RDF4CPP_IRIFACTORY_HPP

#include <nonstd/expected.hpp>
#include <string>
#include <string_view>

#include <dice/sparse-map/sparse_map.hpp>

#include <rdf4cpp/rdf/IRI.hpp>

namespace rdf4cpp::rdf {
enum class IRIFactoryError {
    Ok,
    UnknownPrefix,
    Relative,
    InvalidScheme,
};

/**
 * wrapper around a std::string_view that allows to access it as its IRI components.
 */
class IRIView {
    std::string_view data;

public:
    explicit IRIView(std::string_view iri) noexcept;

private:
    [[nodiscard]] size_t get_scheme_len() const noexcept;
    [[nodiscard]] std::pair<size_t, size_t> get_authority_len() const noexcept;
    [[nodiscard]] std::pair<size_t, size_t> get_path_len() const noexcept;
    [[nodiscard]] std::pair<size_t, size_t> get_query_len() const noexcept;
    [[nodiscard]] size_t get_fragment_offset() const noexcept;

public:
    /**
     * is this IRI a relative reference?
     * equivalent to scheme().empty()
     * @return
     */
    [[nodiscard]] bool is_relative() const noexcept;
    /**
     * the scheme part of the IRI
     * @return
     */
    [[nodiscard]] std::string_view scheme() const noexcept;
    /**
     * the authority part of the IRI
     * @return
     */
    [[nodiscard]] std::string_view authority() const noexcept;
    /**
     * the path of the IRI
     * @return
     */
    [[nodiscard]] std::string_view path() const noexcept;
    /**
     * the query part of the IRI
     * @return
     */
    [[nodiscard]] std::string_view query() const noexcept;
    /**
     * the fragment part of the IRI
     * @return
     */
    [[nodiscard]] std::string_view fragment() const noexcept;

    /**
     * checks if the IRI is valid according to the IRI specification (not the schemes specification).
     * (does not accept relative references)
     * @return
     */
    [[nodiscard]] IRIFactoryError valid() const noexcept;
};

class IRIFactory {
    dice::sparse_map::sparse_map<std::string, std::string, dice::hash::DiceHashwyhash<std::string_view>, std::equal_to<>> prefixes;

public:
    std::string base;

    explicit IRIFactory(std::string_view base = "http://example.org/") noexcept;

    [[nodiscard]] nonstd::expected<IRI, IRIFactoryError> from_relative(std::string_view rel, storage::node::NodeStorage &storage = storage::node::NodeStorage::default_instance()) const noexcept;
    [[nodiscard]] nonstd::expected<IRI, IRIFactoryError> from_prefix(std::string_view prefix, std::string_view local, storage::node::NodeStorage &storage = storage::node::NodeStorage::default_instance()) const;

    void assign_prefix(std::string_view prefix, std::string_view expanded);
    void clear_prefix(std::string_view prefix);

private:
    [[nodiscard]] nonstd::expected<IRI, IRIFactoryError> create_and_validate(std::string_view iri, storage::node::NodeStorage &storage) const noexcept;
};
}  // namespace rdf4cpp::rdf

#endif  //RDF4CPP_IRIFACTORY_HPP
