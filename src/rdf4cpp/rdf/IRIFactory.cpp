#include "IRIFactory.hpp"

#include <rdf4cpp/rdf/regex/Regex.hpp>

namespace rdf4cpp::rdf {

IRIView::IRIView(std::string_view iri) noexcept : data(iri) {
}

std::pair<size_t, bool> IRIView::get_scheme_len() const noexcept {
    auto c = data.find(':');
    if (c == std::string_view::npos)
        return {0, false};
    auto s = data.find('/');
    if (c > s)
        return {0, false};
    return {c, true};
}
std::optional<std::string_view> IRIView::scheme() const noexcept {
    auto s = get_scheme_len();
    if (!s.second)
        return std::nullopt;
    return data.substr(0, s.first);
}
bool IRIView::is_relative() const noexcept {
    return !get_scheme_len().second;
}

std::tuple<size_t, size_t, bool> IRIView::get_authority_len() const noexcept {
    auto d = data;
    auto s = get_scheme_len().first;
    if (s > 0) {
        ++s;
        d = d.substr(s);
    }
    if (!d.starts_with("//"))
        return {s, 0, false};
    d = d.substr(2);
    s += 2;
    auto e = d.find_first_of("/#?");
    if (e == std::string_view::npos)
        e = d.length();
    return {s, e, true};
}
std::optional<std::string_view> IRIView::authority() const noexcept {
    auto a = get_authority_len();
    if (!std::get<2>(a))
        return std::nullopt;
    return data.substr(std::get<0>(a), std::get<1>(a));
}

std::pair<size_t, size_t> IRIView::get_path_len() const noexcept {
    auto a = get_authority_len();
    auto b = std::get<0>(a) + std::get<1>(a);
    auto d = data.substr(b);
    if (std::get<1>(a) > 0 && !d.starts_with('/'))
        return {b, 0};
    auto e = d.find_first_of("#?");
    if (e == std::string_view::npos)
        e = d.length();
    return {b, e};
}
std::string_view IRIView::path() const noexcept {
    auto a = get_path_len();
    return data.substr(a.first, a.second);
}

std::tuple<size_t, size_t, bool> IRIView::get_query_len() const noexcept {
    auto a = get_path_len();
    auto b = a.first + a.second;
    auto d = data.substr(b);
    if (!d.starts_with('?'))
        return {b, 0, false};
    d = d.substr(1);
    ++b;
    auto e = d.find_first_of('#');
    if (e == std::string_view::npos)
        e = d.length();
    return {b, e, true};
}
std::optional<std::string_view> IRIView::query() const noexcept {
    auto a = get_query_len();
    if (!std::get<2>(a))
        return std::nullopt;
    return data.substr(std::get<0>(a), std::get<1>(a));
}

std::pair<size_t, bool> IRIView::get_fragment_offset() const noexcept {
    auto a = get_query_len();
    auto b = std::get<0>(a) + std::get<1>(a);
    auto d = data.substr(b);
    if (!d.starts_with('#'))
        return {b, false};
    return {b + 1, true};
}
std::optional<std::string_view> IRIView::fragment() const noexcept {
    auto f = get_fragment_offset();
    if (!f.second)
        return std::nullopt;
    return data.substr(f.first);
}

std::string_view IRIView::to_absolute() const noexcept {
    auto f = get_fragment_offset();
    if (!f.second)
        return data;
    return data.substr(0, f.first - 1);
}

IRIFactoryError IRIView::valid() const noexcept {
    if (is_relative())
        return IRIFactoryError::Relative;
    auto s = scheme();
    static regex::Regex scheme_reg{"[[:alpha:]][[:alnum:]+-.]*"};
    if (!scheme_reg.regex_match(*s))
        return IRIFactoryError::InvalidScheme;
    // TODO
    return IRIFactoryError::Ok;
}

IRIFactory::IRIFactory(std::string_view base) {
    if (set_base(base) != IRIFactoryError::Ok)
        throw std::invalid_argument{"invalid base"};
}

nonstd::expected<IRI, IRIFactoryError> IRIFactory::from_relative(std::string_view rel, storage::node::NodeStorage &storage) const noexcept {
    IRIView r{rel};
    IRIView b{base};
    std::string iri{};
    auto r_scheme = r.scheme();
    auto r_auth = r.authority();
    auto r_path = r.path();
    auto r_query = r.query();
    auto r_frag = r.fragment();
    if (r_scheme.has_value()) {
        iri = construct(*r_scheme, r_auth, remove_dot_segments(r_path), r_query, r_frag);
    } else if (r_auth.has_value()) {
        iri = construct(*b.scheme(), r_auth, remove_dot_segments(r_path), r_query, r_frag);
    } else if (r_path.empty()) {
        iri = construct(*b.scheme(), b.authority(), b.path(), r_query.has_value() ? r_query : b.query(), r_frag);
    } else if (r_path.starts_with('/')) {
        iri = construct(*b.scheme(), b.authority(), remove_dot_segments(r_path), r_query, r_frag);
    } else {
        auto m = merge_paths(IRIView{base}, r_path);
        iri = construct(*b.scheme(), b.authority(), remove_dot_segments(m), r_query, r_frag);
    }

    return create_and_validate(iri, storage);
}

nonstd::expected<IRI, IRIFactoryError> IRIFactory::from_prefix(std::string_view prefix, std::string_view local, storage::node::NodeStorage &storage) const {
    auto i = prefixes.find(prefix);
    if (i == prefixes.end())
        return nonstd::make_unexpected(IRIFactoryError::UnknownPrefix);
    std::string deref {i->second};
    deref.append(local);
    if (IRIView{deref}.is_relative())
        return from_relative(deref, storage);
    return create_and_validate(deref, storage);
}

nonstd::expected<IRI, IRIFactoryError> IRIFactory::create_and_validate(std::string_view iri, storage::node::NodeStorage &storage) const noexcept {
    auto e = IRIView{iri}.valid();
    if (e != IRIFactoryError::Ok)
        return nonstd::make_unexpected(e);
    return IRI{iri, storage};
}

void IRIFactory::assign_prefix(std::string_view prefix, std::string_view expanded) {
    std::string pre{prefix};
    prefixes[pre] = expanded;
}
void IRIFactory::clear_prefix(std::string_view prefix) {
    prefixes.erase(prefix);
}

std::string IRIFactory::remove_dot_segments(std::string_view path) noexcept {
    std::string r{};
    std::string in{path};
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

std::string_view IRIFactory::first_path_segment(std::string_view path) noexcept {
    size_t off = 0;
    if (path.starts_with('/'))
        off = 1;
    auto e = path.find('/', off);
    return path.substr(0, e);
}

void IRIFactory::remove_last_path_segment(std::string &path) noexcept {
    auto e = path.find_last_of('/');
    if (e == std::string::npos)
        return;
    path.resize(e);
}

std::string IRIFactory::merge_paths(IRIView base, std::string_view path) noexcept {
    if (!base.is_relative() && base.path().empty()) {
        std::string r = "/";
        r.append(path);
        return r;
    }
    std::string r {base.path()};
    remove_last_path_segment(r);
    r.append(1, '/');
    r.append(path);
    return r;
}

std::string IRIFactory::construct(std::string_view scheme, std::optional<std::string_view> auth, std::string_view path,
                                  std::optional<std::string_view> query, std::optional<std::string_view> frag) noexcept {
    std::stringstream str{};
    str << scheme << ':';
    if (auth.has_value())
        str << "//" << *auth;
    if (!path.empty() && !path.starts_with('/') && auth.has_value())
        str << '/';
    str << path;
    if (query.has_value())
        str << '?' << *query;
    if (frag.has_value())
        str << '#' << *frag;
    return str.str();
}

std::string_view IRIFactory::get_base() const noexcept {
    return base;
}

IRIFactoryError IRIFactory::set_base(std::string_view b) noexcept {
    auto e = IRIView{b}.valid();
    if (e != IRIFactoryError::Ok)
        return e;
    base = b;
    return IRIFactoryError::Ok;
}

}  // namespace rdf4cpp::rdf
