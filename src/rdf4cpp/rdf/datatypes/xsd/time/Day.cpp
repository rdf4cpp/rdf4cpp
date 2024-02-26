#include "Day.hpp"

#include <rdf4cpp/rdf/datatypes/registry/util/DateTimeUtils.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

#ifndef DOXYGEN_PARSER
template<>
capabilities::Default<xsd_gDay>::cpp_type capabilities::Default<xsd_gDay>::from_string(std::string_view s) {
    using namespace registry::util;
    if (!s.starts_with("---")) {
        throw std::runtime_error{"missing gDay prexfix"};
    }

    s.remove_prefix(3);

    auto tz = rdf::util::Timezone::parse_optional(s);
    auto day = parse_date_time_fragment<std::chrono::day, unsigned int, '\0', identifier>(s);
    if (!day.ok()) {
        throw std::runtime_error("invalid day");
    }

    return std::make_pair(day, tz);
}

template<>
bool capabilities::Default<xsd_gDay>::serialize_canonical_string(cpp_type const &value, void *buffer, writer::Cursor *cursor, writer::FlushFunc flush) noexcept {
    auto str = std::format("---{:%d}", value.first);
    if (value.second.has_value())
        str += value.second->to_canonical_string();

    return writer::write_str(str, buffer, cursor, flush);
}

template<>
std::partial_ordering capabilities::Comparable<xsd_gDay>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    auto day_to_tp = [](std::chrono::day d) noexcept -> rdf::util::TimePoint {
        return rdf::util::construct(rdf::util::TimePointReplacementDate.year() / rdf::util::TimePointReplacementDate.month() / d, rdf::util::TimePointReplacementTimeOfDay);
    };
    return registry::util::compare_time_points(day_to_tp(lhs.first), lhs.second, day_to_tp(rhs.first), rhs.second);
}

using IHelp = registry::util::InliningHelper<uint8_t>;
static_assert(registry::util::number_of_bits(31u) == 5);
static_assert(sizeof(std::chrono::day) == 1);
static_assert(sizeof(IHelp) * 8 < storage::node::identifier::LiteralID::width);

template<>
std::optional<storage::node::identifier::LiteralID> capabilities::Inlineable<xsd_gDay>::try_into_inlined(cpp_type const &value) noexcept {
    IHelp i{static_cast<uint8_t>(static_cast<unsigned int>(value.first)), value.second};
    return util::pack<storage::node::identifier::LiteralID>(i);
}

template<>
capabilities::Inlineable<xsd_gDay>::cpp_type capabilities::Inlineable<xsd_gDay>::from_inlined(storage::node::identifier::LiteralID inlined) noexcept {
    auto i = util::unpack<IHelp>(inlined);
    return std::make_pair(std::chrono::day{i.time_value}, i.decode_tz());
}

template<>
template<>
capabilities::Subtype<xsd_gDay>::super_cpp_type<0> capabilities::Subtype<xsd_gDay>::into_supertype<0>(cpp_type const &value) noexcept {
    return std::make_pair(rdf::util::TimePointReplacementDate.year() / rdf::util::TimePointReplacementDate.month() / value.first, value.second);
}

template<>
template<>
nonstd::expected<capabilities::Subtype<xsd_gDay>::cpp_type, DynamicError> capabilities::Subtype<xsd_gDay>::from_supertype<0>(super_cpp_type<0> const &value) noexcept {
    return std::make_pair(value.first.day(), value.second);
}
#endif

template struct LiteralDatatypeImpl<xsd_gDay,
                                    capabilities::Comparable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable,
                                    capabilities::Subtype>;

}  // namespace rdf4cpp::rdf::datatypes::registry