#ifndef RDF4CPP_IRIFACTORY_HPP
#define RDF4CPP_IRIFACTORY_HPP

#include <optional>
#include <string>
#include <string_view>

#include <dice/sparse-map/sparse_map.hpp>

#include <rdf4cpp/rdf/IRI.hpp>
#include <rdf4cpp/rdf/util/Expected.hpp>

namespace rdf4cpp::rdf {
/**
 * std::format gets you a printable string.
 */
enum class IRIFactoryError {
    Ok,
    UnknownPrefix,
    Relative,
    InvalidScheme,
    InvalidUserinfo,
    InvalidHost,
    InvalidPort,
    InvalidPath,
    InvalidQuery,
    InvalidFragment,
};

/**
 * wrapper around a std::string_view that allows to access it as its IRI components.
 */
class IRIView {
    std::string_view data;

public:
    explicit IRIView(std::string_view iri) noexcept;

private:
    /**
     * note that even if defined = false, start and len are valid.
     * they refer to a 0 length location where the relevant part could have been and where the next part should be searched.
     */
    struct IRIPart {
        size_t start, len;
        bool defined;
    };

    /**
     * ->scheme
     * @return
     */
    [[nodiscard]] IRIPart get_scheme_part() const noexcept;
    /**
     * scheme->authority
     * @param scheme
     * @return
     */
    [[nodiscard]] IRIPart get_authority_part(const IRIPart &scheme) const noexcept;
    /**
     * authority->path
     * @param auth
     * @return
     */
    [[nodiscard]] IRIPart get_path_part(const IRIPart &auth) const noexcept;
    /**
     * path->query
     * @param path
     * @return
     */
    [[nodiscard]] IRIPart get_query_part(const IRIPart &path) const noexcept;
    /**
     * query->fragment
     * @param query
     * @return
     */
    [[nodiscard]] IRIPart get_fragment_part(const IRIPart &query) const noexcept;

    /**
     * authority->userinfo
     * @param auth
     * @return
     */
    [[nodiscard]] IRIPart get_userinfo_part(const IRIPart& auth) const noexcept;
    /**
     * authority,userinfo->host
     * @param auth
     * @param uinfo
     * @return
     */
    [[nodiscard]] IRIPart get_host_part(const IRIPart& auth, const IRIPart& uinfo) const noexcept;
    /**
     * authority,host->port
     * @param auth
     * @param host
     * @return
     */
    [[nodiscard]] IRIPart get_port_part(const IRIPart& auth, const IRIPart& host) const noexcept;

    /**
     * gets the string_view representing the part (empty on undefined)
     * @param r
     * @return
     */
    [[nodiscard]] std::string_view apply(const IRIPart &r) const noexcept;
    /**
     * gets the string_view representing the part or std::nullopt if undefined.
     * @param r
     * @return
     */
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

    /**
     * the userinfo part of the authority.
     * @return
     */
    [[nodiscard]] std::optional<std::string_view> userinfo() const noexcept;
    /**
     * the host part of the authority.
     * @return
     */
    [[nodiscard]] std::optional<std::string_view> host() const noexcept;
    /**
     * the port part of the authority.
     * @return
     */
    [[nodiscard]] std::optional<std::string_view> port() const noexcept;

    /**
     * all authority parts of the IRI. intended to be used with structured bindings:
     * auto [userinfo, host, port] = iri_view.all_authority_parts();
     * more efficient than querying each part individually via its method.
     * @return
     */
    [[nodiscard]] std::tuple<std::optional<std::string_view>, std::optional<std::string_view>, std::optional<std::string_view>> all_authority_parts() const noexcept;

    /**
     * quickly checks if the IRI is valid according to the IRI specification (not the schemes specification).
     * (does not accept relative references)
     * (may miss invalid IRIs)
     * @return
     */
    [[nodiscard]] IRIFactoryError quick_validate() const noexcept;
};

/**
 * Stores a base IRI and a prefix map and allows to create IRIs by possibly applying both.
 */
class IRIFactory {
public:
    using PrefixMap = dice::sparse_map::sparse_map<std::string, std::string, dice::hash::DiceHashwyhash<std::string_view>, std::equal_to<>>;
private:
    PrefixMap prefixes;

    std::string base;

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
    explicit IRIFactory(PrefixMap&& prefixes, std::string_view base = default_base);

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
    [[nodiscard]] static std::string remove_dot_segments(std::string_view path) noexcept;
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
    [[nodiscard]] static std::string merge_paths(IRIView base, std::string_view path) noexcept;
};
}  // namespace rdf4cpp::rdf

template<>
struct std::formatter<rdf4cpp::rdf::IRIFactoryError> : std::formatter<string_view> {
    auto format(rdf4cpp::rdf::IRIFactoryError p, format_context& ctx) const {
        std::string_view s{};
        switch (p) {
            case rdf4cpp::rdf::IRIFactoryError::Ok:
                s = "Ok";
                break;
            case rdf4cpp::rdf::IRIFactoryError::UnknownPrefix:
                s = "UnknownPrefix";
                break;
            case rdf4cpp::rdf::IRIFactoryError::Relative:
                s = "Relative";
                break;
            case rdf4cpp::rdf::IRIFactoryError::InvalidScheme:
                s = "InvalidScheme";
                break;
            case rdf4cpp::rdf::IRIFactoryError::InvalidUserinfo:
                s = "InvalidUserinfo";
                break;
            case rdf4cpp::rdf::IRIFactoryError::InvalidHost:
                s = "InvalidHost";
                break;
            case rdf4cpp::rdf::IRIFactoryError::InvalidPort:
                s = "InvalidPort";
                break;
            case rdf4cpp::rdf::IRIFactoryError::InvalidPath:
                s = "InvalidPath";
                break;
            case rdf4cpp::rdf::IRIFactoryError::InvalidQuery:
                s = "InvalidQuery";
                break;
            case rdf4cpp::rdf::IRIFactoryError::InvalidFragment:
                s = "InvalidFragment";
                break;
            default:
                s = "Unknown";
                break;
        }
        return std::formatter<std::string_view>::format(s, ctx);
    }
};

#endif  //RDF4CPP_IRIFACTORY_HPP
