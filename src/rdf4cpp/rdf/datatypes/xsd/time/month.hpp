#ifndef RDF4CPP_MONTH_HPP
#define RDF4CPP_MONTH_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>

#include <date/date.h>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
struct DatatypeMapping<xsd_gMonth> {
    using cpp_datatype = date::month;
};


template<>
capabilities::Default<xsd_gMonth>::cpp_type capabilities::Default<xsd_gMonth>::from_string(std::string_view s);

template<>
std::string capabilities::Default<xsd_gMonth>::to_canonical_string(const cpp_type &value) noexcept;



template<>
std::partial_ordering capabilities::Comparable<xsd_gMonth>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept;

extern template struct LiteralDatatypeImpl<xsd_gMonth,
                                           capabilities::Comparable,
                                           capabilities::FixedId>;

}

namespace rdf4cpp::rdf::datatypes::xsd {

struct GMonth : registry::LiteralDatatypeImpl<registry::xsd_gMonth,
                                               registry::capabilities::Comparable,
                                               registry::capabilities::FixedId> {};

}  // namespace rdf4cpp::rdf::datatypes::xsd


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::GMonth const xsd_gMonth_instance;

} // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail

#endif  //RDF4CPP_MONTH_HPP
