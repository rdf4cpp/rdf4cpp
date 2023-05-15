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
        return rdf4cpp::rdf::storage::util::robin_hood::hash<std::array<size_t, 2>>{}(
                std::array<size_t, 2>{
                        rdf4cpp::rdf::storage::util::robin_hood::hash<std::string_view>{}(x.lexical_form),
                        rdf4cpp::rdf::storage::util::robin_hood::hash<std::string_view>{}(x.language_tag)});
    }
};


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline rdf::LangString const rdf_lang_string_instance;

} // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail


#endif  //RDF4CPP_RDF_LANGSTRING_HPP
