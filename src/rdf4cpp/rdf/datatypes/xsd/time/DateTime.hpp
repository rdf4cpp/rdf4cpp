#ifndef RDF4CPP_DATETIME_HPP
#define RDF4CPP_DATETIME_HPP

#include <chrono>

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/util/Timezone.hpp>
#include <dice/hash.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

#ifndef DOXYGEN_PARSER
template<>
struct DatatypeMapping<xsd_dateTime> {
    using cpp_datatype = std::pair<rdf::util::TimePoint, rdf::util::OptionalTimezone>;
};


template<>
capabilities::Default<xsd_dateTime>::cpp_type capabilities::Default<xsd_dateTime>::from_string(std::string_view s);

template<>
bool capabilities::Default<xsd_dateTime>::serialize_canonical_string(cpp_type const &value, void *buffer, writer::Cursor *cursor, writer::FlushFunc flush) noexcept;

template<>
std::optional<storage::node::identifier::LiteralID> capabilities::Inlineable<xsd_dateTime>::try_into_inlined(cpp_type const &value) noexcept;

template<>
capabilities::Inlineable<xsd_dateTime>::cpp_type capabilities::Inlineable<xsd_dateTime>::from_inlined(storage::node::identifier::LiteralID inlined) noexcept;

template<>
std::partial_ordering capabilities::Comparable<xsd_dateTime>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept;
#endif

extern template struct LiteralDatatypeImpl<xsd_dateTime,
                                           capabilities::Comparable,
                                           capabilities::FixedId,
                                           capabilities::Inlineable>;

}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {

struct DateTime : registry::LiteralDatatypeImpl<registry::xsd_dateTime,
                                                registry::capabilities::Comparable,
                                                registry::capabilities::FixedId,
                                                registry::capabilities::Inlineable> {};

}  // namespace rdf4cpp::rdf::datatypes::xsd


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::DateTime const xsd_DateTime_instance;

}  // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail

#ifndef DOXYGEN_PARSER
template<typename Policy>
struct dice::hash::dice_hash_overload<Policy, rdf4cpp::rdf::util::TimePoint> {
    static size_t dice_hash(rdf4cpp::rdf::util::TimePoint const &x) noexcept {
        auto tp = x.time_since_epoch().count();
        return dice::hash::dice_hash_templates<Policy>::dice_hash(tp);
    }
};
#endif

#endif  //RDF4CPP_DATETIME_HPP
