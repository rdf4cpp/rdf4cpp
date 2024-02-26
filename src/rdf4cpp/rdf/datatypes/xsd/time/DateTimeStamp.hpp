#ifndef RDF4CPP_DATETIMESTAMP_HPP
#define RDF4CPP_DATETIMESTAMP_HPP

#include <chrono>

#include <dice/hash.hpp>
#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/time/DateTime.hpp>
#include <rdf4cpp/rdf/util/Timezone.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

#ifndef DOXYGEN_PARSER
template<>
struct DatatypeMapping<xsd_dateTimeStamp> {
    using cpp_datatype = rdf::util::ZonedTime;
};
template<>
struct DatatypeSupertypeMapping<xsd_dateTimeStamp> {
    using supertype = xsd::DateTime;
};


template<>
capabilities::Default<xsd_dateTimeStamp>::cpp_type capabilities::Default<xsd_dateTimeStamp>::from_string(std::string_view s);

template<>
bool capabilities::Default<xsd_dateTimeStamp>::serialize_canonical_string(cpp_type const &value, void *buffer, writer::Cursor *cursor, writer::FlushFunc flush) noexcept;

template<>
std::optional<storage::node::identifier::LiteralID> capabilities::Inlineable<xsd_dateTimeStamp>::try_into_inlined(cpp_type const &value) noexcept;

template<>
capabilities::Inlineable<xsd_dateTimeStamp>::cpp_type capabilities::Inlineable<xsd_dateTimeStamp>::from_inlined(storage::node::identifier::LiteralID inlined) noexcept;

template<>
std::partial_ordering capabilities::Comparable<xsd_dateTimeStamp>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept;

template<>
template<>
capabilities::Subtype<xsd_dateTimeStamp>::super_cpp_type<0> capabilities::Subtype<xsd_dateTimeStamp>::into_supertype<0>(cpp_type const &value) noexcept;

template<>
template<>
nonstd::expected<capabilities::Subtype<xsd_dateTimeStamp>::cpp_type, DynamicError> capabilities::Subtype<xsd_dateTimeStamp>::from_supertype<0>(super_cpp_type<0> const &value) noexcept;
#endif

extern template struct LiteralDatatypeImpl<xsd_dateTimeStamp,
                                           capabilities::Comparable,
                                           capabilities::FixedId,
                                           capabilities::Subtype,
                                           capabilities::Inlineable>;

}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {

struct DateTimeStamp : registry::LiteralDatatypeImpl<registry::xsd_dateTimeStamp,
                                                     registry::capabilities::Comparable,
                                                     registry::capabilities::FixedId,
                                                     registry::capabilities::Subtype,
                                                     registry::capabilities::Inlineable> {
    /**
     * any DateTimeStamp with this Timezone may get inlined.
     * can be changed at compile time.
     * warning: loading a database that was saved with a different inlining_default_timezone is undefined behavior.
     */
    static constexpr util::Timezone inlining_default_timezone = util::Timezone();
};

}  // namespace rdf4cpp::rdf::datatypes::xsd


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::DateTimeStamp const xsd_DateTimeStamp_instance;

}  // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail

#ifndef DOXYGEN_PARSER
template<typename Policy>
struct dice::hash::dice_hash_overload<Policy, rdf4cpp::rdf::util::ZonedTime> {
    static size_t dice_hash(rdf4cpp::rdf::util::ZonedTime const &x) noexcept {
        auto tp = x.get_sys_time().time_since_epoch().count();
        auto off = x.get_time_zone().offset.count();
        return dice::hash::dice_hash_templates<Policy>::dice_hash(std::tie(tp, off));
    }
};
#endif

#endif  //RDF4CPP_DATETIMESTAMP_HPP
