#include "Serialization.hpp"
#include <rdf4cpp/rdf/writer/TryWrite.hpp>

namespace rdf4cpp::rdf::writer {

using TypeIRIPrefix = std::pair<std::string_view, std::string_view>;

static constexpr std::array iri_prefixes = {
    TypeIRIPrefix{"http://www.w3.org/2001/XMLSchema#", "xsd"},
    TypeIRIPrefix{"http://www.w3.org/1999/02/22-rdf-syntax-ns#", "rdf"},
};

static consteval typename decltype(iri_prefixes)::const_iterator find_prefix(std::string_view name) {
    return std::ranges::find_if(iri_prefixes, [&](auto const &pre) {
        return name.starts_with(pre.first);
    });
}

template<size_t N, size_t Ix, size_t Len>
static consteval std::array<std::string_view, N> make_type_iri_buf(std::array<std::string_view, N> ret = {}) {
    using namespace datatypes::registry;
    using namespace datatypes::registry::util;

    if constexpr (Ix >= Len) {
        return ret;
    } else {
        if constexpr (constexpr auto it = find_prefix(reserved_datatype_ids.storage[Ix].first); it != iri_prefixes.end()) {
            constexpr auto without_prefix = reserved_datatype_ids.storage[Ix].first.substr(it->first.size());

            using concat = ConstexprStringHolder<ConstexprString<it->second.size() + 1>{it->second}
                                                    + ConstexprString{":"}
                                                    + ConstexprString<without_prefix.size() + 1>{without_prefix}>;

            ret[static_cast<size_t>(reserved_datatype_ids.storage[Ix].second.to_underlying())] = concat::value;
        }

        return make_type_iri_buf<N, Ix + 1, Len>(ret);
    }
}

static constexpr auto type_iri_buffer = make_type_iri_buf<1 << storage::node::identifier::LiteralType::width, 0, datatypes::registry::reserved_datatype_ids.size()>();

bool write_type_iri(datatypes::registry::LiteralType t, void *const buffer, writer::Cursor *cursor, writer::FlushFunc const flush, bool short_form) {
    if (short_form) {
        auto &p = type_iri_buffer[t.to_underlying()];
        if (!p.empty()) {
            if (!write_str(p, buffer, cursor, flush))
                return false;
            return true;
        }
    }
    if (!write_str("<", buffer, cursor, flush))
        return false;

    // TODO this is wrong, it doesn't work for non-fixed-id datatypes
    if (!write_str(datatypes::registry::DatatypeRegistry::get_entry(t)->datatype_iri, buffer, cursor, flush))
        return false;
    if (!write_str(">", buffer, cursor, flush))
        return false;
    return true;
}

bool write_prefix_data(void *buffer, Cursor *cursor, FlushFunc flush) {
    for (auto const &[prefix, shorthand] : iri_prefixes) {
        RDF4CPP_DETAIL_TRY_WRITE_STR("@prefix ");
        RDF4CPP_DETAIL_TRY_WRITE_STR(shorthand);
        RDF4CPP_DETAIL_TRY_WRITE_STR(": <");
        RDF4CPP_DETAIL_TRY_WRITE_STR(prefix);
        RDF4CPP_DETAIL_TRY_WRITE_STR("> .\n");
    }

    return true;
}
}  // namespace rdf4cpp::rdf::writer
