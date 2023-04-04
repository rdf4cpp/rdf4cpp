#ifndef RDF4CPP_RDF_LANGSTRING_HPP
#define RDF4CPP_RDF_LANGSTRING_HPP

#include <bit>
#include <optional>
#include <string_view>
#include <vector>

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/storage/util/robin-hood-hashing/robin_hood_hash.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

namespace lang_tags {
/**
 * language tags to try to inline.
 * the vector can be modified at startup.
 * modifying the vector after any Literal was created is undefined behavior.
 * this includes Literals already created in a persistent node storage.
 */
extern
#ifndef MODIFYABLE_LANG_TAG_INLINES
        const
#endif
        std::vector<std::string>
                tags_to_inline;

using LangTagID = uint64_t;

constexpr size_t bits_needed_for(uint64_t i) {
    return std::bit_width(i);
}

/**
 * number of bits needed for the current tags to inline
 */
inline size_t inlined_size() noexcept {
    // number of bits needed for the biggest id
    return bits_needed_for(tags_to_inline.size() - 1);
}

/**
 * shift where the inlined tag is located
 */
inline uint64_t shift() noexcept {
    return storage::node::identifier::LiteralID::width - inlined_size();
}

/**
 * mask for the inlined language tag
 */
inline uint64_t mask_inlined() noexcept {
    return ((1l << inlined_size()) - 1) << shift();
}

/**
 * mask for the base literal id
 */
inline uint64_t mask_base_id() noexcept {
    return (1l << shift()) - 1;
}

/**
 * converts a inlined language tag id back to its language tag.
 * @param id
 * @return language tag or the empty string on a invalid id
 */
inline std::string_view inlined_to_tag(LangTagID id) noexcept {
    if (id < tags_to_inline.size())
        return tags_to_inline[id];
    return "";
}

/**
 * tries to convert a language tag to its inlined id.
 * @param tag
 * @return id or std::nullopt
 */
inline std::optional<LangTagID> try_tag_to_inlined(std::string_view tag) noexcept {
    for (uint64_t i = 0; i < tags_to_inline.size(); ++i) {
        if (tags_to_inline[i] == tag) {
            return i;
        }
    }
    return std::nullopt;
}

/**
 * tries to inline a language tag into a LiteralID
 * @param id
 * @param tag
 * @return inlined LiteralID or std::nullopt
 */
inline std::optional<storage::node::identifier::LiteralID> try_into_inlined(storage::node::identifier::LiteralID id, LangTagID tag) noexcept {
    if ((id.value & mask_inlined()) != 0)
        return std::nullopt;
    return storage::node::identifier::LiteralID{id.value | (tag << shift())};
}

/**
 * extracts the base LiteralID and the language tag id
 * @param id
 * @return [language_tag_id, base_literal_id]
 */
inline std::pair<LangTagID, storage::node::identifier::LiteralID> from_inlined(storage::node::identifier::LiteralID id) noexcept {
    uint64_t t = (id.value & mask_inlined()) >> shift();
    uint64_t i = id.value & mask_base_id();
    return std::pair{t, storage::node::identifier::LiteralID{i}};
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
inline capabilities::Default<rdf_lang_string>::cpp_type capabilities::Default<rdf_lang_string>::from_string(std::string_view) {
    // dummy implementation, actual implementation in Literal
    assert(false);
    __builtin_unreachable();
}

template<>
inline std::string capabilities::Default<rdf_lang_string>::to_canonical_string(cpp_type const &) noexcept {
    // dummy implementation, actual implementation in Literal
    assert(false);
    __builtin_unreachable();
}


template<>
inline std::optional<storage::node::identifier::LiteralID> capabilities::Inlineable<rdf_lang_string>::try_into_inlined(cpp_type const &) noexcept {
    // dummy implementation, actual implementation in Literal
    assert(false);
    __builtin_unreachable();
}

template<>
inline capabilities::Inlineable<rdf_lang_string>::cpp_type capabilities::Inlineable<rdf_lang_string>::from_inlined(storage::node::identifier::LiteralID) noexcept {
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
