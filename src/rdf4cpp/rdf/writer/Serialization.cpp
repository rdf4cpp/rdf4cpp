#include "Serialization.hpp"

namespace rdf4cpp::rdf::writer {

class TypeIRIBufferBuilder {
    static consteval size_t get_size() {
        size_t m = 0;
        for (const auto &[name, id] : datatypes::registry::reserved_datatype_ids) {
            if (id.to_underlying() > m)
                m = id.to_underlying();
        }
        return m + 1;
    }

    static consteval size_t match_prefix(std::string_view name) {
        for (size_t i = 0; i < iri_prefixes.size(); ++i) {
            auto c = iri_prefixes[i];
            if (name.starts_with(c.prefix)) {
                return i;
            }
        }
        return std::numeric_limits<size_t>::max();
    }

    template<size_t TypeID>
    static consteval size_t find_type() {
        for (size_t i = 0; i < datatypes::registry::reserved_datatype_ids.storage.size(); ++i) {
            if (static_cast<size_t>(datatypes::registry::reserved_datatype_ids.storage[i].second.to_underlying()) == TypeID)
                return i;
        }
        return std::numeric_limits<size_t>::max();
    }

    template<size_t TypeID>
    static consteval std::string_view build_single_entry() {
        constexpr size_t type_index = find_type<TypeID>();
        if constexpr (type_index == std::numeric_limits<size_t>::max()) {
            return "";
        } else {
            constexpr auto type_data = datatypes::registry::reserved_datatype_ids.storage[type_index];
            constexpr auto prefix_index = match_prefix(type_data.first);
            if constexpr (prefix_index == std::numeric_limits<size_t>::max()) {
                return "";
            } else {
                using namespace datatypes::registry::util;
                constexpr auto shorthand = iri_prefixes[prefix_index].shorthand;
                constexpr auto rel = type_data.first.substr(iri_prefixes[prefix_index].prefix.size());
                return ConstexprStringHolder<ConstexprString<shorthand.size() + 1>(shorthand) + ConstexprString(":") + ConstexprString<rel.size() + 1>(rel)>::value;
            }
        }
    }

    template<size_t... Idx>
    static consteval auto build_buffer(std::index_sequence<Idx...>) {
        return std::array{build_single_entry<Idx>()...};
    }

public:
    static consteval auto build() {
        return build_buffer(std::make_index_sequence<get_size()>());
    }
};

static constexpr auto type_iri_buffer = TypeIRIBufferBuilder::build();

// check at compile time, if all datatype ids have a valid mapping. if not, at throws, which breaks the static_assert
static consteval bool validate_iri_buffer() {
    for (const auto &d : datatypes::registry::reserved_datatype_ids) {
        if (type_iri_buffer.at(d.second.to_underlying()).size() == 1)  // also check, if something is just :
            return false;
    }
    return true;
}
static_assert(validate_iri_buffer());

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
    if (!write_str(datatypes::registry::DatatypeRegistry::get_entry(t)->datatype_iri, buffer, cursor, flush))
        return false;
    if (!write_str(">", buffer, cursor, flush))
        return false;
    return true;
}

class PrefixDataBuilder {
    template<size_t I>
    static consteval auto build_line() {
        using namespace datatypes::registry::util;
        constexpr auto sh = iri_prefixes[I].shorthand;
        constexpr auto pr = iri_prefixes[I].prefix;
        return ConstexprString("@prefix ") + ConstexprString<sh.size()+1>(sh) + ConstexprString(": <") + ConstexprString<pr.size()+1>(pr) + ConstexprString("> .\n");
    }

    template<size_t... Idx>
    static consteval auto build(std::index_sequence<Idx...>) {
        return (build_line<Idx>() + ...);
    }
public:
    static consteval std::string_view build() {
        return datatypes::registry::util::ConstexprStringHolder<build(std::make_index_sequence<iri_prefixes.size()>())>::value;
    }
};

bool write_prefix_data(void *buffer, Cursor &cursor, FlushFunc flush) {
    static constexpr auto d = PrefixDataBuilder::build();
    return write_str(d, buffer, &cursor, flush);
}
}  // namespace rdf4cpp::rdf::writer
