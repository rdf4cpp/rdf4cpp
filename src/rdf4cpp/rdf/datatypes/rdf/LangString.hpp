#ifndef RDF4CPP_LANGSTRING_HPP
#define RDF4CPP_LANGSTRING_HPP

#include "rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp"
#include "rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp"

namespace rdf4cpp::rdf::datatypes::registry {

struct LangStringRepr {
    std::string lexical_form;
    std::string language_tag;

    auto operator<=>(LangStringRepr const &) const = default;
};

/*
 * Name of the datatype. This is kept so that we won't need to type it over and over again.
 */
constexpr static registry::ConstexprString rdf_lang_string{"http://www.w3.org/1999/02/22-rdf-syntax-ns#langString"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<rdf_lang_string> {
    using cpp_datatype = LangStringRepr;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline capabilities::Default<rdf_lang_string>::cpp_type capabilities::Default<rdf_lang_string>::from_string(std::string_view) {
    // dummy implementation, actual implementation in Literal
    assert(false);
    __builtin_unreachable();
}

template<>
inline std::string capabilities::Default<rdf_lang_string>::to_string(cpp_type const &) {
    // dummy implementation, actual implementation in Literal
    assert(false);
    __builtin_unreachable();
}

}  // namespace rdf4cpp::rdf::datatypes::registry
namespace rdf4cpp::rdf::datatypes::rdf {
/**
 * Implementation of rdf::langString
 */
struct LangString : registry::LiteralDatatypeImpl<registry::rdf_lang_string,
                                                  registry::capabilities::Comparable> {
};

}  // namespace rdf4cpp::rdf::datatypes::rdf

#endif  //RDF4CPP_LANGSTRING_HPP
