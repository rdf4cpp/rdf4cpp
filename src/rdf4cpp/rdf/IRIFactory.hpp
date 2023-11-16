#ifndef RDF4CPP_IRIFACTORY_HPP
#define RDF4CPP_IRIFACTORY_HPP

#include <nonstd/expected.hpp>
#include <optional>
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
    struct IRIPart {
        size_t start, len;
        bool defined;
    };

    [[nodiscard]] IRIPart get_scheme_part() const noexcept;
    [[nodiscard]] IRIPart get_authority_part(const IRIPart &scheme) const noexcept;
    [[nodiscard]] IRIPart get_path_part(const IRIPart &auth) const noexcept;
    [[nodiscard]] IRIPart get_query_part(const IRIPart &path) const noexcept;
    [[nodiscard]] IRIPart get_fragment_part(const IRIPart &query) const noexcept;

    [[nodiscard]] IRIPart get_userinfo_part(const IRIPart& auth) const noexcept;
    [[nodiscard]] IRIPart get_host_part(const IRIPart& auth, const IRIPart& uinfo) const noexcept;
    [[nodiscard]] IRIPart get_port_part(const IRIPart& auth, const IRIPart& host) const noexcept;

    [[nodiscard]] std::string_view apply(const IRIPart &r) const noexcept;
    [[nodiscard]] std::optional<std::string_view> apply_opt(const IRIPart &r) const noexcept;

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
    [[nodiscard]] std::optional<std::string_view> scheme() const noexcept;
    /**
     * the authority part of the IRI
     * @return
     */
    [[nodiscard]] std::optional<std::string_view> authority() const noexcept;
    /**
     * the path of the IRI
     * @return
     */
    [[nodiscard]] std::string_view path() const noexcept;
    /**
     * the query part of the IRI
     * @return
     */
    [[nodiscard]] std::optional<std::string_view> query() const noexcept;
    /**
     * the fragment part of the IRI
     * @return
     */
    [[nodiscard]] std::optional<std::string_view> fragment() const noexcept;

    /**
     * all parts of the IRI. intended to be used with structured bindings:
     * auto [scheme, auth, path, query, frag] = iri_view.all_parts();
     * more efficient than querying each part individually via its method.
     * @return
     */
    [[nodiscard]] std::tuple<std::optional<std::string_view>,
                             std::optional<std::string_view>,
                             std::string_view,
                             std::optional<std::string_view>,
                             std::optional<std::string_view>>
    all_parts() const noexcept;

    /**
     * everything except the fragment part of the IRI.
     * @return
     */
    [[nodiscard]] std::string_view to_absolute() const noexcept;

    [[nodiscard]] std::optional<std::string_view> userinfo() const noexcept;
    [[nodiscard]] std::string_view host() const noexcept;
    [[nodiscard]] std::optional<std::string_view> port() const noexcept;

    /**
     * checks if the IRI is valid according to the IRI specification (not the schemes specification).
     * (does not accept relative references)
     * @return
     */
    [[nodiscard]] IRIFactoryError valid() const noexcept;
};

class IRIFactory {
    dice::sparse_map::sparse_map<std::string, std::string, dice::hash::DiceHashwyhash<std::string_view>, std::equal_to<>> prefixes;

    std::string base;

public:
    explicit IRIFactory(std::string_view base = "http://example.org/");

    [[nodiscard]] nonstd::expected<IRI, IRIFactoryError> from_relative(std::string_view rel, storage::node::NodeStorage &storage = storage::node::NodeStorage::default_instance()) const noexcept;
    [[nodiscard]] nonstd::expected<IRI, IRIFactoryError> from_prefix(std::string_view prefix, std::string_view local, storage::node::NodeStorage &storage = storage::node::NodeStorage::default_instance()) const;

    void assign_prefix(std::string_view prefix, std::string_view expanded);
    void clear_prefix(std::string_view prefix);

    [[nodiscard]] std::string_view get_base() const noexcept;
    [[nodiscard]] IRIFactoryError set_base(std::string_view b) noexcept;

private:
    [[nodiscard]] nonstd::expected<IRI, IRIFactoryError> create_and_validate(std::string_view iri, storage::node::NodeStorage &storage) const noexcept;

    [[nodiscard]] static std::string construct(std::string_view scheme, std::optional<std::string_view> auth, std::string_view path,
                                               std::optional<std::string_view> query, std::optional<std::string_view> frag) noexcept;
    [[nodiscard]] static std::string remove_dot_segments(std::string_view path) noexcept;
    [[nodiscard]] static std::string_view first_path_segment(std::string_view path) noexcept;
    static void remove_last_path_segment(std::string &path) noexcept;
    [[nodiscard]] static std::string merge_paths(IRIView base, std::string_view path) noexcept;
};
}  // namespace rdf4cpp::rdf

#endif  //RDF4CPP_IRIFACTORY_HPP
