#ifndef RDF4CPP_XSD_DOUBLE_HPP
#define RDF4CPP_XSD_DOUBLE_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
struct DatatypeMapping<xsd_double> {
    using cpp_datatype = double;
};

template<>
capabilities::Default<xsd_double>::cpp_type capabilities::Default<xsd_double>::from_string(std::string_view s);

template<>
std::string capabilities::Default<xsd_double>::to_string(cpp_type const &value) noexcept;

template<>
bool capabilities::Logical<xsd_double>::effective_boolean_value(cpp_type const &value) noexcept;

template<>
std::optional<uint64_t> capabilities::Inlineable<xsd_double>::try_into_inlined(cpp_type const &value) noexcept;

template<>
capabilities::Inlineable<xsd_double>::cpp_type capabilities::Inlineable<xsd_double>::from_inlined(uint64_t inlined) noexcept;

extern template struct LiteralDatatypeImpl<xsd_double,
                                           capabilities::Logical,
                                           capabilities::Numeric,
                                           capabilities::Comparable,
                                           capabilities::FixedId,
                                           capabilities::Inlineable>;

}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {

struct Double : registry::LiteralDatatypeImpl<registry::xsd_double,
                                              registry::capabilities::Logical,
                                              registry::capabilities::Numeric,
                                              registry::capabilities::Comparable,
                                              registry::capabilities::FixedId,
                                              registry::capabilities::Inlineable> {};

}  // namespace rdf4cpp::rdf::datatypes::xsd


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::Double const xsd_double_instance;

} // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail

#endif  //RDF4CPP_XSD_DOUBLE_HPP
