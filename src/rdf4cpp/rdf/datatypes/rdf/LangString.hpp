#ifndef RDF4CPP_RDF_LANGSTRING_HPP
#define RDF4CPP_RDF_LANGSTRING_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

struct LangStringRepr {
    std::string_view lexical_form;
    std::string_view language_tag;

    std::weak_ordering operator<=>(LangStringRepr const &other) const noexcept;

    bool operator==(LangStringRepr const &) const noexcept = default;
    bool operator!=(LangStringRepr const &) const noexcept = default;
    bool operator<(LangStringRepr const &) const noexcept = default;
    bool operator<=(LangStringRepr const &) const noexcept = default;
    bool operator>(LangStringRepr const &) const noexcept = default;
    bool operator>=(LangStringRepr const &) const noexcept = default;
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

extern template struct LiteralDatatypeImpl<rdf_lang_string,
                                           capabilities::Comparable,
                                           capabilities::FixedId>;

}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::rdf {

struct LangString : registry::LiteralDatatypeImpl<registry::rdf_lang_string,
                                                  registry::capabilities::Comparable,
                                                  registry::capabilities::FixedId> {};

}  // namespace rdf4cpp::rdf::datatypes::rdf


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline rdf::LangString const rdf_lang_string_instance;

} // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail

#endif  //RDF4CPP_RDF_LANGSTRING_HPP
