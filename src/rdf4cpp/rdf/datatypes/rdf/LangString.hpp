#ifndef RDF4CPP_RDF_LANGSTRING_HPP
#define RDF4CPP_RDF_LANGSTRING_HPP

#include <optional>
#include <string_view>

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/storage/util/robin-hood-hashing/robin_hood_hash.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

namespace lang_tags {
enum class InlinedTags : uint8_t {
    en,
    de,
    fr,
    ch,
};
constexpr const size_t inlined_size = 2;
constexpr const uint64_t mask_inlined = ((1l << inlined_size) - 1) << (storage::node::identifier::LiteralID::width - inlined_size);
constexpr const uint64_t mask_id = (1l << (storage::node::identifier::LiteralID::width - inlined_size)) - 1;

constexpr const char *from_inlined(InlinedTags t) noexcept {
    switch (t) {
        case InlinedTags::en:
            return "en";
        case InlinedTags::de:
            return "de";
        case InlinedTags::fr:
            return "fr";
        case InlinedTags::ch:
            return "ch";
        default:
            return "";
    }
}
constexpr std::optional<InlinedTags> try_into_inlined(std::string_view t) noexcept {
    if (t == "en")
        return InlinedTags::en;
    if (t == "de")
        return InlinedTags::de;
    if (t == "fr")
        return InlinedTags::fr;
    if (t == "ch")
        return InlinedTags::ch;
    return std::nullopt;
}

constexpr std::optional<uint64_t> try_into_inlined(uint64_t id, InlinedTags tag) noexcept {
    if ((id & mask_inlined) != 0)
        return std::nullopt;
    uint64_t t = static_cast<uint64_t>(static_cast<uint8_t>(tag));
    return id | (t << (storage::node::identifier::LiteralID::width - inlined_size));
}
constexpr std::pair<InlinedTags, uint64_t> from_inlined(uint64_t id) noexcept {
    uint64_t t = (id & mask_inlined) >> (storage::node::identifier::LiteralID::width - inlined_size);
    uint64_t i = id & mask_id;
    return std::pair{static_cast<InlinedTags>(static_cast<uint8_t>(t)), i};
}
}  // namespace lang_tags

struct LangStringRepr {
    std::string_view lexical_form;
    std::string_view language_tag;

    auto operator<=>(LangStringRepr const &other) const noexcept = default;
};

template<>
struct DatatypeMapping<rdf_lang_string> {
    using cpp_datatype = LangStringRepr;
};

template<>
inline capabilities::Default<rdf_lang_string>::cpp_type capabilities::Default<rdf_lang_string>::from_string([[maybe_unused]] std::string_view) {
    // dummy implementation, actual implementation in Literal
    assert(false);
    __builtin_unreachable();
}

template<>
inline std::string capabilities::Default<rdf_lang_string>::to_canonical_string([[maybe_unused]] cpp_type const &) noexcept {
    // dummy implementation, actual implementation in Literal
    assert(false);
    __builtin_unreachable();
}


template<>
inline std::optional<uint64_t> capabilities::Inlineable<rdf_lang_string>::try_into_inlined([[maybe_unused]] cpp_type const &value) noexcept {
    // dummy implementation, actual implementation in Literal
    assert(false);
    __builtin_unreachable();
}

template<>
inline capabilities::Inlineable<rdf_lang_string>::cpp_type capabilities::Inlineable<rdf_lang_string>::from_inlined([[maybe_unused]] uint64_t inlined) noexcept {
    // dummy implementation, actual implementation in Literal
    assert(false);
    __builtin_unreachable();
}

extern template struct LiteralDatatypeImpl<rdf_lang_string,
                                           capabilities::Comparable,
                                           capabilities::FixedId>;

}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::rdf {

struct LangString : registry::LiteralDatatypeImpl<registry::rdf_lang_string,
                                                  registry::capabilities::Comparable,
                                                  registry::capabilities::FixedId,
                                                  registry::capabilities::Inlineable> {};

}  // namespace rdf4cpp::rdf::datatypes::rdf

template<>
struct std::hash<rdf4cpp::rdf::datatypes::registry::LangStringRepr> {
    size_t operator()(rdf4cpp::rdf::datatypes::registry::LangStringRepr const &x) const noexcept {
        using namespace rdf4cpp::rdf::storage::util;

        return robin_hood::hash<std::array<size_t, 2>>{}(
                std::array<size_t, 2>{
                        robin_hood::hash<std::string_view>{}(x.lexical_form),
                        robin_hood::hash<std::string_view>{}(x.language_tag)});
    }
};


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline rdf::LangString const rdf_lang_string_instance;

} // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail


#endif  //RDF4CPP_RDF_LANGSTRING_HPP
