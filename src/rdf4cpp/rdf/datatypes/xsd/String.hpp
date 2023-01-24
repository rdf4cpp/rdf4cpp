#ifndef RDF4CPP_XSD_STRING_HPP
#define RDF4CPP_XSD_STRING_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
struct DatatypeMapping<xsd_string> {
    using cpp_datatype = std::string;
};

template<>
capabilities::Default<xsd_string>::cpp_type capabilities::Default<xsd_string>::from_string(std::string_view s);

template<>
std::string capabilities::Default<xsd_string>::to_canonical_string(cpp_type const &value) noexcept;

template<>
bool capabilities::Logical<xsd_string>::effective_boolean_value(cpp_type const &value) noexcept;

extern template struct LiteralDatatypeImpl<xsd_string,
                                           capabilities::Logical,
                                           capabilities::Comparable,
                                           capabilities::FixedId>;

} // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {

struct String : registry::LiteralDatatypeImpl<registry::xsd_string,
                                              registry::capabilities::Logical,
                                              registry::capabilities::Comparable,
                                              registry::capabilities::FixedId> {};

} // namespace rdf4cpp::rdf::datatypes::xsd


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::String const xsd_string_instance;

} // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail

#endif  //RDF4CPP_XSD_STRING_HPP
