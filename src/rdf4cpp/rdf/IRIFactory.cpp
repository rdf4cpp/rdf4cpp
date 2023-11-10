#include "IRIFactory.hpp"

#include <rdf4cpp/rdf/regex/Regex.hpp>

namespace rdf4cpp::rdf {

IRIView::IRIView(std::string_view iri) noexcept : data(iri) {
}

size_t IRIView::get_scheme_len() const noexcept {
    auto c = data.find(':');
    if (c == std::string_view::npos)
        return 0;
    auto s = data.find('/');
    if (c > s)
        return 0;
    return c;
}
std::string_view IRIView::scheme() const noexcept {
    return data.substr(0, get_scheme_len());
}
bool IRIView::is_relative() const noexcept {
    return get_scheme_len() == 0;
}

std::pair<size_t, size_t> IRIView::get_authority_len() const noexcept {
    auto d = data;
    auto s = get_scheme_len();
    if (s > 0) {
        ++s;
        d = d.substr(s);
    }
    if (!d.starts_with("//"))
        return std::make_pair(s, 0);
    d = d.substr(2);
    s += 2;
    auto e = d.find_first_of("/#?");
    if (e == std::string_view::npos)
        e = d.length();
    return std::make_pair(s, e);
}
std::string_view IRIView::authority() const noexcept {
    auto a = get_authority_len();
    return data.substr(a.first, a.second);
}

std::pair<size_t, size_t> IRIView::get_path_len() const noexcept {
    auto a = get_authority_len();
    auto b = a.first + a.second;
    auto d = data.substr(b);
    if (a.second > 0 && !d.starts_with('/'))
        return std::make_pair(b, 0);
    auto e = d.find_first_of("#?");
    if (e == std::string_view::npos)
        e = d.length();
    return std::make_pair(b, e);
}
std::string_view IRIView::path() const noexcept {
    auto a = get_path_len();
    return data.substr(a.first, a.second);
}

std::pair<size_t, size_t> IRIView::get_query_len() const noexcept {
    auto a = get_path_len();
    auto b = a.first + a.second;
    auto d = data.substr(b);
    if (!d.starts_with('?'))
        return std::make_pair(b, 0);
    auto e = d.find_first_of('#');
    if (e == std::string_view::npos)
        e = d.length();
    return std::make_pair(b, e);
}
std::string_view IRIView::query() const noexcept {
    auto a = get_query_len();
    return data.substr(a.first, a.second);
}

size_t IRIView::get_fragment_offset() const noexcept {
    auto a = get_query_len();
    auto b = a.first + a.second;
    auto d = data.substr(b);
    if (!d.starts_with('#'))
        return std::string_view::npos;
    return b + 1;
}
std::string_view IRIView::fragment() const noexcept {
    auto f = get_fragment_offset();
    if (f == std::string_view::npos)
        return data.substr(0, 0);
    return data.substr(f);
}

std::string_view IRIView::to_absolute() const noexcept {
    auto f = get_fragment_offset();
    if (f != std::string_view::npos)
        --f;
    return data.substr(0, f);
}

IRIFactoryError IRIView::valid() const noexcept {
    if (is_relative())
        return IRIFactoryError::Relative;
    auto s = scheme();
    static regex::Regex scheme_reg{"[[:alpha:]][[:alnum:]+-.]*"};
    if (!scheme_reg.regex_match(s))
        return IRIFactoryError::InvalidScheme;
    // TODO
    return IRIFactoryError::Ok;
}

IRIFactory::IRIFactory(std::string_view base) noexcept  : base(base) {
}

nonstd::expected<IRI, IRIFactoryError> IRIFactory::from_relative(std::string_view rel, storage::node::NodeStorage &storage) const noexcept {
    std::string iri = base;
    iri.append(rel);
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

std::string IRIFactory::remove_dot_segments(std::string_view path) const {
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
std::string_view IRIFactory::first_path_segment(std::string_view path) const {
    size_t off = 0;
    if (path.starts_with('/'))
        off = 1;
    auto e = path.find('/', off);
    return path.substr(0, e);
}

void IRIFactory::remove_last_path_segment(std::string &path) const {
    auto e = path.find_last_of('/');
    if (e == std::string::npos)
        return;
    path.resize(e);
}

std::string IRIFactory::merge_paths(IRIView base, std::string_view path) const {
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

}  // namespace rdf4cpp::rdf
