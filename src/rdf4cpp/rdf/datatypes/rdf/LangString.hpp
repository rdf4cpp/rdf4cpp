#ifndef RDF4CPP_RDF_LANGSTRING_HPP
#define RDF4CPP_RDF_LANGSTRING_HPP

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
    return static_cast<size_t>(std::floor(std::log2(static_cast<double>(i)))) + 1;
}

/**
 * number of bits needed for the current tags to inline
 */
size_t inlined_size() noexcept;

/**
 * shift where the inlined tag is located
 */
uint64_t shift() noexcept;

/**
 * mask for the inlined language tag
 */
uint64_t mask_inlined() noexcept;

/**
 * mask for the base literal id
 */
uint64_t mask_base_id() noexcept;

/**
 * converts a inlined language tag id back to its language tag.
 * @param id
 * @return language tag or the empty string on a invalid id
 */
std::string_view inlined_to_tag(LangTagID id) noexcept;

/**
 * tries to convert a language tag to its inlined id.
 * @param tag
 * @return id or std::nullopt
 */
std::optional<LangTagID> try_tag_to_inlined(std::string_view tag) noexcept;

/**
 * tries to inline a language tag into a LiteralID
 * @param id
 * @param tag
 * @return inlined LiteralID or std::nullopt
 */
std::optional<storage::node::identifier::LiteralID> try_into_inlined(storage::node::identifier::LiteralID id, LangTagID tag) noexcept;

/**
 * extracts the base LiteralID and the language tag id
 * @param id
 * @return [language_tag_id, base_literal_id]
 */
std::pair<LangTagID, storage::node::identifier::LiteralID> from_inlined(storage::node::identifier::LiteralID id) noexcept;
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
