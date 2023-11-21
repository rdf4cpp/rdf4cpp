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

private:
    [[nodiscard]] nonstd::expected<IRI, IRIFactoryError> create_and_validate(std::string_view iri, storage::node::NodeStorage &storage) const noexcept;

    [[nodiscard]] static std::string construct(std::string_view scheme, std::optional<std::string_view> auth, std::string_view path,
                                               std::optional<std::string_view> query, std::optional<std::string_view> frag) noexcept;
    [[nodiscard]] static std::string remove_dot_segments(std::string_view path) noexcept;
    [[nodiscard]] static std::string_view first_path_segment(std::string_view path) noexcept;
    static void remove_last_path_segment(std::string &path) noexcept;
    [[nodiscard]] static std::string merge_paths(IRIView base, std::string_view path) noexcept;
};

namespace detail {
template<class T>
concept CharMatcher = requires(const T a, int c) {{a.match(c)} -> std::same_as<bool>; };

struct ASCIIPatternMatcher {
    std::string_view pattern;

    [[nodiscard]] constexpr bool match(int c) const noexcept {
        auto ch = static_cast<char>(c);
        if (c != static_cast<int>(ch))  // not asciii
            return false;
        return pattern.find(ch) != std::string_view::npos;
    }
};

struct ASCIINumMatcher {
    [[nodiscard]] static constexpr bool match(int c) noexcept {
        auto ch = static_cast<char>(c);
        if (c != static_cast<int>(ch))  // not asciii
            return false;
        return c >= '0' && c <= '9';
    }
};
struct ASCIIAlphaMatcher {
    [[nodiscard]] static constexpr bool match(int c) noexcept {
        auto ch = static_cast<char>(c);
        if (c != static_cast<int>(ch))  // not asciii
            return false;
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }
};

template<CharMatcher A, CharMatcher B>
struct OrMatcher {
    A a;
    B b;

    constexpr OrMatcher(A a, B b) : a(a), b(b) {}
    constexpr OrMatcher() = default;

    [[nodiscard]] constexpr bool match(int c) const noexcept {
        return a.match(c) || b.match(c);
    }
};

template<CharMatcher A, CharMatcher B>
constexpr OrMatcher<A, B> operator|(A a, B b) {
    return OrMatcher{a, b};
}

constexpr auto ascii_alphanum_matcher = ASCIIAlphaMatcher{} | ASCIINumMatcher{};

struct UCSCharMatcher {
    [[nodiscard]] static constexpr bool match(int c) noexcept {
        return (c >= 0xA0 && c <= 0xD7FF) ||
               (c >= 0xF900 && c <= 0xFDCF) ||
               (c >= 0xFDF0 && c <= 0xFFEF) ||
               (c >= 0x10000 && c <= 0x1FFFD) ||
               (c >= 0x20000 && c <= 0x2FFFD) ||
               (c >= 0x30000 && c <= 0x3FFFD) ||
               (c >= 0x40000 && c <= 0x4FFFD) ||
               (c >= 0x50000 && c <= 0x5FFFD) ||
               (c >= 0x60000 && c <= 0x6FFFD) ||
               (c >= 0x70000 && c <= 0x7FFFD) ||
               (c >= 0x80000 && c <= 0x8FFFD) ||
               (c >= 0x90000 && c <= 0x9FFFD) ||
               (c >= 0xA0000 && c <= 0xAFFFD) ||
               (c >= 0xB0000 && c <= 0xBFFFD) ||
               (c >= 0xC0000 && c <= 0xCFFFD) ||
               (c >= 0xD0000 && c <= 0xDFFFD) ||
               (c >= 0xE0000 && c <= 0xEFFFD);
    }
};

constexpr auto i_unreserved_matcher = ascii_alphanum_matcher | ASCIIPatternMatcher{"-._~"} | UCSCharMatcher{};
constexpr auto sub_delims_matcher = ASCIIPatternMatcher{"!$&'()*+,;="};

struct IPrivateMatcher {
    [[nodiscard]] static constexpr bool match(int c) noexcept {
        return (c >= 0xE000 && c <= 0xF8FF) ||
               (c >= 0xF0000 && c <= 0xFFFFD) ||
               (c >= 0x100000 && c <= 0x10FFFD);
    }
};

template<CharMatcher M, class S>
bool match(const M &m, S s) noexcept {
    for (int c : s) {
        if (!m.match(c))
            return false;
    }
    return true;
}
}  // namespace detail
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
