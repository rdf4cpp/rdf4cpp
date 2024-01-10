#include "IRIFactory.hpp"

namespace rdf4cpp::rdf {

/**
* turns the parts of a IRI back into a full IRI.
*/
static std::string_view construct(std::string_view scheme, std::optional<std::string_view> auth, std::string_view path,
                                  std::optional<std::string_view> query, std::optional<std::string_view> frag) noexcept {
    static thread_local std::string str;
    str.clear();
    str.reserve(std::bit_ceil(scheme.size() + 1 + path.size()));
    str.append(scheme);
    str.push_back(':');

    if (auth.has_value()) {
        str.append("//");
        str.append(*auth);
    }

    if (!path.empty() && !path.starts_with('/') && auth.has_value()) {
        str.push_back('/');
    }

    str.append(path);

    if (query.has_value()) {
        str.push_back('?');
        str.append(*query);
    }

    if (frag.has_value()) {
        str.push_back('#');
        str.append(*frag);
    }

    return str;
}

/**
* gets the first segment of path
*/
static std::string_view first_path_segment(std::string_view path) noexcept {
    size_t off = 0;
    if (path.starts_with('/'))
        off = 1;
    auto e = path.find('/', off);
    return path.substr(0, e);
}

/**
 * removes the last segment of path
 */
static void remove_last_path_segment(std::string &path) noexcept {
    auto e = path.find_last_of('/');
    if (e == std::string::npos)
        return;
    path.resize(e);
}

/**
* removes ./ and ../ segments from path.
*/
static std::string_view remove_dot_segments(std::string_view path) noexcept {
    static thread_local std::string r;
    static thread_local std::string in;

    r.clear();
    in.clear();
    in.append(path);

    while (!in.empty()) {
        if (in.starts_with("./")) {
            in.erase(0, 2);
            continue;
        }
        if (in.starts_with("../")) {
            in.erase(0, 3);
            continue;
        }

        if (in.starts_with("/./")) {
            in.erase(0, 2);
            continue;
        }
        auto seg = first_path_segment(in);
        if (seg == "/.") {
            in.erase(1, 1);
            continue;
        }

        if (in.starts_with("/../")) {
            in.erase(0, 3);
            remove_last_path_segment(r);
            continue;
        }
        if (seg == "/..") {
            in.erase(1, 2);
            remove_last_path_segment(r);
            continue;
        }

        if (in == "." || in == "..") {
            break;
        }

        r.append(seg);
        in.erase(0, seg.length());
    }

    return r;
}

/**
 * merges the path of the current base and path, as described in https://datatracker.ietf.org/doc/html/rfc3986#section-5.2.3.
 */
static std::string merge_path_with_base(IRIView::AllParts const &base, std::string_view path) noexcept {
    static thread_local std::string r;

    if (base.scheme.has_value() && base.path.empty()) {
        r.clear();
        r.reserve(path.size() + 1);
        r.push_back('/');
        r.append(path);
        return r;
    }

    r.clear();
    r.reserve(base.path.size() + path.size() + 1);
    r.append(base.path);
    remove_last_path_segment(r);
    r.push_back('/');
    r.append(path);
    return r;
}

/**
 * Converts rel to an absolute IRI by merging it with the given base
 */
static std::string_view to_absolute(IRIView::AllParts const &base, std::string_view rel) noexcept {
    auto [r_scheme, r_auth, r_path, r_query, r_frag] = IRIView{rel}.all_parts();

    if (r_scheme.has_value()) {
        return construct(*r_scheme, r_auth, remove_dot_segments(r_path), r_query, r_frag);
    }

    auto const &[b_scheme, b_auth, b_path, b_query, _b_frag] = base;

    if (r_auth.has_value()) {
        return construct(*b_scheme, r_auth, remove_dot_segments(r_path), r_query, r_frag);
    }
    if (r_path.empty()) {
        return construct(*b_scheme, b_auth, b_path, r_query.has_value() ? r_query : b_query, r_frag);
    }
    if (r_path.starts_with('/')) {
        return construct(*b_scheme, b_auth, remove_dot_segments(r_path), r_query, r_frag);
    }

    auto const merged = merge_path_with_base(base, r_path);
    return construct(*b_scheme, b_auth, remove_dot_segments(merged), r_query, r_frag);
}


IRIFactory::IRIFactory(std::string_view base) {
    if (set_base(base) != IRIFactoryError::Ok) {
        throw std::invalid_argument{"invalid base"};
    }
}

IRIFactory::IRIFactory(prefix_map_type &&prefixes, std::string_view base) : prefixes(std::move(prefixes)) {
    if (set_base(base) != IRIFactoryError::Ok) {
        throw std::invalid_argument{"invalid base"};
    }
}

nonstd::expected<IRI, IRIFactoryError> IRIFactory::from_relative(std::string_view rel, storage::node::NodeStorage &storage) const noexcept {
    return create_and_validate(to_absolute(base_parts_cache, rel), storage);
}

nonstd::expected<IRI, IRIFactoryError> IRIFactory::from_prefix(std::string_view prefix, std::string_view local, storage::node::NodeStorage &storage) const {
    auto i = prefixes.find(prefix);
    if (i == prefixes.end()) {
        return nonstd::make_unexpected(IRIFactoryError::UnknownPrefix);
    }

    static thread_local std::string deref;
    deref.clear();
    deref.reserve(i->second.size() + local.size());
    deref.append(i->second);
    deref.append(local);

    if (IRIView{deref}.is_relative()) {
        return from_relative(deref, storage);
    }

    return create_and_validate(deref, storage);
}

nonstd::expected<IRI, IRIFactoryError> IRIFactory::create_and_validate(std::string_view iri, storage::node::NodeStorage &storage) noexcept {
    if (auto const e = IRIView{iri}.quick_validate(); e != IRIFactoryError::Ok) {
        return nonstd::make_unexpected(e);
    }

    return IRI{iri, storage};
}

void IRIFactory::assign_prefix(std::string_view prefix, std::string_view expanded) {
    std::string pre{prefix};
    prefixes[pre] = expanded;
}
void IRIFactory::clear_prefix(std::string_view prefix) {
    auto it = prefixes.find(prefix);
    if (it == prefixes.end()) [[unlikely]] {
        return;
    }

    prefixes.erase(it);
}

std::string_view IRIFactory::get_base() const noexcept {
    return base;
}

IRIFactoryError IRIFactory::set_base(std::string_view b) noexcept {
    if (auto const e = IRIView{b}.quick_validate(); e != IRIFactoryError::Ok) {
        return e;
    }

    base = b;
    base_parts_cache = IRIView{base}.all_parts();
    return IRIFactoryError::Ok;
}

}  // namespace rdf4cpp::rdf
