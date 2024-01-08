#include "IRIFactory.hpp"

#include <uni_algo/all.h>

#include <rdf4cpp/rdf/util/CharMatcher.hpp>
#include <rdf4cpp/rdf/datatypes/registry/DatatypeRegistry.hpp>

namespace rdf4cpp::rdf {
IRIFactory::IRIFactory(std::string_view base) {
    if (set_base(base) != IRIFactoryError::Ok)
        throw std::invalid_argument{"invalid base"};
}

IRIFactory::IRIFactory(IRIFactory::PrefixMap &&prefixes, std::string_view base) : prefixes(std::move(prefixes)) {
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

nonstd::expected<IRI, IRIFactoryError> IRIFactory::create_and_validate(std::string_view iri, storage::node::NodeStorage &storage) noexcept {
    if (!rdf4cpp::rdf::datatypes::registry::relaxed_parsing_mode) {
        auto e = IRIView{iri}.quick_validate();
        if (e != IRIFactoryError::Ok)
            return nonstd::make_unexpected(e);
    }
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
    size_t l = scheme.length() + 1 + path.length();
    if (auth.has_value()) {
        l += auth->length() + 2;
    }
    if (query.has_value()) {
        l += query->length() + 1;
    }
    if (frag.has_value()) {
        l += frag->length() + 1;
    }
    std::string str{};
    str.reserve(l);
    str.append(scheme);
    str.append(1, ':');
    if (auth.has_value()) {
        str.append(2, '/');
        str.append(*auth);
    }
    if (!path.empty() && !path.starts_with('/') && auth.has_value()) {
        str.append(1, '/');
    }
    str.append(path);
    if (query.has_value()) {
        str.append(1, '?');
        str.append(*query);
    }
    if (frag.has_value()) {
        str.append(1, '#');
        str.append(*frag);
    }
    return str;
}

std::string_view IRIFactory::get_base() const noexcept {
    return base;
}

IRIFactoryError IRIFactory::set_base(std::string_view b) noexcept {
    if (!rdf4cpp::rdf::datatypes::registry::relaxed_parsing_mode) {
        auto e = IRIView{b}.quick_validate();
        if (e != IRIFactoryError::Ok)
            return e;
    }
    base = b;
    return IRIFactoryError::Ok;
}

}  // namespace rdf4cpp::rdf
