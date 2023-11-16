#include "IRIFactory.hpp"

#include <rdf4cpp/rdf/regex/Regex.hpp>

namespace rdf4cpp::rdf {

IRIView::IRIView(std::string_view iri) noexcept : data(iri) {
}

std::string_view IRIView::apply(const IRIView::IRIPart &r) const noexcept {
    return data.substr(r.start, r.len);
}
std::optional<std::string_view> IRIView::apply_opt(const IRIView::IRIPart &r) const noexcept {
    if (r.defined)
        return apply(r);
    return std::nullopt;
}

IRIView::IRIPart IRIView::get_scheme_part() const noexcept {
    auto c = data.find(':');
    if (c == std::string_view::npos)
        return {0, 0, false};
    auto s = data.find('/');
    if (c > s)
        return {0, 0, false};
    return {0, c, true};
}
std::optional<std::string_view> IRIView::scheme() const noexcept {
    return apply_opt(get_scheme_part());
}
bool IRIView::is_relative() const noexcept {
    return !get_scheme_part().defined;
}

IRIView::IRIPart IRIView::get_authority_part(const IRIPart &scheme) const noexcept {
    auto d = data;
    auto s = scheme.start + scheme.len;
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
    return apply_opt(get_authority_part(get_scheme_part()));
}

IRIView::IRIPart IRIView::get_path_part(const IRIPart &auth) const noexcept {
    auto b = auth.start + auth.len;
    auto d = data.substr(b);
    if (auth.len > 0 && !d.starts_with('/'))
        return {b, 0, true};
    auto e = d.find_first_of("#?");
    if (e == std::string_view::npos)
        e = d.length();
    return {b, e, true};
}
std::string_view IRIView::path() const noexcept {
    return apply(get_path_part(get_authority_part(get_scheme_part())));
}

IRIView::IRIPart IRIView::get_query_part(const IRIPart &path) const noexcept {
    auto b = path.start + path.len;
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
    return apply_opt(get_query_part(get_path_part(get_authority_part(get_scheme_part()))));
}

IRIView::IRIPart IRIView::get_fragment_part(const IRIPart &query) const noexcept {
    auto b = query.start + query.len;
    auto d = data.substr(b);
    if (!d.starts_with('#'))
        return {b, d.length(), false};
    return {b + 1, d.length() - 1, true};
}
std::optional<std::string_view> IRIView::fragment() const noexcept {
    return apply_opt(get_fragment_part(get_query_part(get_path_part(get_authority_part(get_scheme_part())))));
}

std::tuple<std::optional<std::string_view>,
           std::optional<std::string_view>,
           std::string_view,
           std::optional<std::string_view>,
           std::optional<std::string_view>>
IRIView::all_parts() const noexcept {
    auto s = get_scheme_part();
    auto a = get_authority_part(s);
    auto p = get_path_part(a);
    auto q = get_query_part(p);
    auto f = get_fragment_part(q);
    return {apply_opt(s), apply_opt(a), apply(p), apply_opt(q), apply_opt(f)};
}

std::string_view IRIView::to_absolute() const noexcept {
    auto f = get_fragment_part(get_query_part(get_path_part(get_authority_part(get_scheme_part()))));
    if (!f.defined)
        return data;
    return data.substr(0, f.start - 1);
}

IRIView::IRIPart IRIView::get_userinfo_part(const IRIView::IRIPart &auth) const noexcept {
    if (!auth.defined)
        return {0, 0, false};
    auto d = apply(auth);
    auto e = d.find('@');
    if (e == std::string_view::npos)
        return {auth.start, 0, false};
    return {auth.start, e, true};
}
std::optional<std::string_view> IRIView::userinfo() const noexcept {
    return apply_opt(get_userinfo_part(get_authority_part(get_scheme_part())));
}

IRIView::IRIPart IRIView::get_host_part(const IRIView::IRIPart &auth, const IRIView::IRIPart &uinfo) const noexcept {
    if (!auth.defined)
        return {0, 0, false};
    auto d = apply(auth);
    if (uinfo.defined)
        d = d.substr(uinfo.len + 1);
    auto b = uinfo.start + uinfo.len + 1;
    auto e = d.find(':');
    if (e == std::string_view::npos)
        return {b, auth.len - uinfo.len, true};
    return {b, e, true};
}
std::string_view IRIView::host() const noexcept {
    auto a = get_authority_part(get_scheme_part());
    return apply(get_host_part(a, get_userinfo_part(a)));
}

IRIView::IRIPart IRIView::get_port_part(const IRIView::IRIPart &auth, const IRIView::IRIPart &host) const noexcept {
    if (!auth.defined)
        return {0, 0, false};
    auto d = apply(auth);
    d = d.substr(host.start - auth.start + host.len);
    auto b = host.start + host.len;
    if (d.starts_with(':'))
        return {b + 1, d.length() - 1, true};
    return {b, 0, false};
}
std::optional<std::string_view> IRIView::port() const noexcept {
    auto a = get_authority_part(get_scheme_part());
    return apply(get_port_part(a, get_host_part(a, get_userinfo_part(a))));
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
    std::string iri{};
    auto [r_scheme, r_auth, r_path, r_query, r_frag] = IRIView{rel}.all_parts();
    auto [b_scheme, b_auth, b_path, b_query, b_frag] = IRIView{base}.all_parts();
    if (r_scheme.has_value()) {
        iri = construct(*r_scheme, r_auth, remove_dot_segments(r_path), r_query, r_frag);
    } else if (r_auth.has_value()) {
        iri = construct(*b_scheme, r_auth, remove_dot_segments(r_path), r_query, r_frag);
    } else if (r_path.empty()) {
        iri = construct(*b_scheme, b_auth, b_path, r_query.has_value() ? r_query : b_query, r_frag);
    } else if (r_path.starts_with('/')) {
        iri = construct(*b_scheme, b_auth, remove_dot_segments(r_path), r_query, r_frag);
    } else {
        auto m = merge_paths(IRIView{base}, r_path);
        iri = construct(*b_scheme, b_auth, remove_dot_segments(m), r_query, r_frag);
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
