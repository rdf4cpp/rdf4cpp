#ifndef RDF4CPP_IRIFACTORY_HPP
#define RDF4CPP_IRIFACTORY_HPP

#include <optional>
#include <string>
#include <string_view>
#include <boost/container/flat_map.hpp>

#include <rdf4cpp/rdf/IRI.hpp>
#include <rdf4cpp/rdf/util/Expected.hpp>
#include <rdf4cpp/rdf/IRIView.hpp>

namespace rdf4cpp::rdf {
/**
 * Stores a base IRI and a prefix map and allows to create IRIs by possibly applying both.
 */
struct IRIFactory {
    using prefix_map_type = boost::container::flat_map<std::string, std::string, std::less<>>;

private:
    prefix_map_type prefixes;

    std::string base;
    IRIView::AllParts base_parts_cache;

public:
    constexpr static std::string_view default_base = "http://example.org/";
    /**
     * Creates a IRIFactory with empty prefix map and a given base IRI. Throws if base is invalid.
     * @param base
     */
    explicit IRIFactory(std::string_view base = default_base);
    /**
     * Creates a IRIFactory with a given prefix map and a given base IRI. Throws if base is invalid.
     * @param prefixes
     * @param base
     */
    explicit IRIFactory(prefix_map_type &&prefixes, std::string_view base = default_base);

    IRIFactory(IRIFactory &&) noexcept = default;
    IRIFactory &operator=(IRIFactory &&) noexcept = default;

    /**
     * Creates a IRI from a possibly relative IRI.
     * Implements https://datatracker.ietf.org/doc/html/rfc3986#section-5.2.
     * @param rel
     * @param storage
     * @return
     */
    [[nodiscard]] nonstd::expected<IRI, IRIFactoryError> from_relative(std::string_view rel, storage::node::NodeStorage &storage = storage::node::NodeStorage::default_instance()) const noexcept;
    /**
     * Creates a IRI by looking up a prefix in the prefix map and possibly resolving a relative IRI.
     * @param prefix
     * @param local
     * @param storage
     * @return
     */
    [[nodiscard]] nonstd::expected<IRI, IRIFactoryError> from_prefix(std::string_view prefix, std::string_view local, storage::node::NodeStorage &storage = storage::node::NodeStorage::default_instance()) const;

    /**
     * Creates or changes a prefix.
     * @param prefix
     * @param expanded
     */
    void assign_prefix(std::string_view prefix, std::string_view expanded);
    /**
     * Removes a prefix.
     * @param prefix
     */
    void clear_prefix(std::string_view prefix);

    /**
     * The base IRI.
     * @return
     */
    [[nodiscard]] std::string_view get_base() const noexcept;
    /**
     * Changes the base IRI. Validates the new base IRI before setting.
     * @param b
     * @return
     */
    [[nodiscard]] IRIFactoryError set_base(std::string_view b) noexcept;

    /**
     * validates the given IRI and creates it in the given node storage, if valid.
     * @param iri
     * @param storage
     * @return
     */
    [[nodiscard]] static nonstd::expected<IRI, IRIFactoryError> create_and_validate(std::string_view iri, storage::node::NodeStorage &storage = storage::node::NodeStorage::default_instance()) noexcept;

private:
    [[nodiscard]] std::string to_absolute(std::string_view rel) const noexcept;

    /**
     * turns the parts of a IRI back into a full IRI.
     * @param scheme
     * @param auth
     * @param path
     * @param query
     * @param frag
     * @return
     */
    [[nodiscard]] static std::string construct(std::string_view scheme, std::optional<std::string_view> auth, std::string_view path,
                                               std::optional<std::string_view> query, std::optional<std::string_view> frag) noexcept;
    /**
     * removes ./ and ../ segments from path.
     * @param path
     * @return
     */
    [[nodiscard]] static std::string_view remove_dot_segments(std::string_view path) noexcept;
    /**
     * gets the first segment of path.
     * @param path
     * @return
     */
    [[nodiscard]] static std::string_view first_path_segment(std::string_view path) noexcept;
    /**
     * removes the last segment of path.
     * @param path
     */
    static void remove_last_path_segment(std::string &path) noexcept;
    /**
     * merges the path of base and path, as described in https://datatracker.ietf.org/doc/html/rfc3986#section-5.2.3.
     * @param base
     * @param path
     * @return
     */
    [[nodiscard]] static std::string merge_paths(IRIView::AllParts const &base, std::string_view path) noexcept;
};
}  // namespace rdf4cpp::rdf
#endif  //RDF4CPP_IRIFACTORY_HPP
