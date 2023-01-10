
/**
* @file Registers xsd:boolean with DatatypeRegistry
*/

#ifndef RDF4CPP_XSD_BOOLEAN_HPP
#define RDF4CPP_XSD_BOOLEAN_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
struct DatatypeMapping<xsd_boolean> {
    using cpp_datatype = bool;
};

template<>
capabilities::Default<xsd_boolean>::cpp_type capabilities::Default<xsd_boolean>::from_string(std::string_view s);

template<>
std::string capabilities::Default<xsd_boolean>::to_string(const cpp_type &value) noexcept;

template<>
bool capabilities::Logical<xsd_boolean>::effective_boolean_value(cpp_type const &value) noexcept;

template<>
std::optional<uint64_t> capabilities::Inlineable<xsd_boolean>::try_into_inlined(cpp_type const &value) noexcept;

template<>
capabilities::Inlineable<xsd_boolean>::cpp_type capabilities::Inlineable<xsd_boolean>::from_inlined(uint64_t inlined) noexcept;

extern template struct LiteralDatatypeImpl<xsd_boolean,
                                           capabilities::Logical,
                                           capabilities::Comparable,
                                           capabilities::FixedId,
                                           capabilities::Inlineable>;

}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {

struct Boolean : registry::LiteralDatatypeImpl<registry::xsd_boolean,
                                               registry::capabilities::Logical,
                                               registry::capabilities::Comparable,
                                               registry::capabilities::FixedId,
                                               registry::capabilities::Inlineable> {};

}  // namespace rdf4cpp::rdf::datatypes::xsd


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::Boolean const xsd_boolean_instance;

} // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail

#endif  //RDF4CPP_XSD_BOOLEAN_HPP
