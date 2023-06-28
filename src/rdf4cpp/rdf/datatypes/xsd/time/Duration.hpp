#ifndef RDF4CPP_DURATION_HPP
#define RDF4CPP_DURATION_HPP

#include <chrono>

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/time/Timezone.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
struct DatatypeMapping<xsd_duration> {
    using cpp_datatype = std::pair<std::chrono::months, std::chrono::milliseconds>;
};


template<>
capabilities::Default<xsd_duration>::cpp_type capabilities::Default<xsd_duration>::from_string(std::string_view s);

template<>
std::string capabilities::Default<xsd_duration>::to_canonical_string(const cpp_type &value) noexcept;


template<>
std::partial_ordering capabilities::Comparable<xsd_duration>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept;

extern template struct LiteralDatatypeImpl<xsd_duration,
                                           capabilities::Comparable,
                                           capabilities::FixedId>;

}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {

struct Duration : registry::LiteralDatatypeImpl<registry::xsd_duration,
                                                registry::capabilities::Comparable,
                                                registry::capabilities::FixedId> {};

}  // namespace rdf4cpp::rdf::datatypes::xsd


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::Duration const xsd_Duration_instance;

}  // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail

#endif  //RDF4CPP_DURATION_HPP
