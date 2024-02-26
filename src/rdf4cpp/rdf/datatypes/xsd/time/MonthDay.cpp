#include "MonthDay.hpp"

#include <rdf4cpp/rdf/datatypes/registry/util/DateTimeUtils.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

#ifndef DOXYGEN_PARSER
template<>
capabilities::Default<xsd_gMonthDay>::cpp_type capabilities::Default<xsd_gMonthDay>::from_string(std::string_view s) {
    using namespace registry::util;
    if (!s.starts_with("--")) {
        throw std::runtime_error{"gMonth prefix missing"};
    }

    s.remove_prefix(2);

    auto month = parse_date_time_fragment<std::chrono::month, unsigned int, '-', identifier>(s);
    auto tz = rdf::util::Timezone::parse_optional(s);
    auto day = parse_date_time_fragment<std::chrono::day, unsigned int, '\0', identifier>(s);
    auto date = month / day;
    if (!date.ok()) {
        throw std::runtime_error("invalid date");
    }

    return std::make_pair(date, tz);
}

template<>
bool capabilities::Default<xsd_gMonthDay>::serialize_canonical_string(cpp_type const &value, void *buffer, writer::Cursor *cursor, writer::FlushFunc flush) noexcept {
    auto str = std::format("--{:%m-%d}", value.first);
    if (value.second.has_value())
        str += value.second->to_canonical_string();
    //return str;
    return false; //TODO
}

struct __attribute__((__packed__)) InliningHelperMonthDay {
    uint8_t month, day;
};
using IHelp = registry::util::InliningHelper<InliningHelperMonthDay>;
static_assert(sizeof(std::chrono::month_day) == 2);
static_assert(sizeof(IHelp) * 8 < storage::node::identifier::LiteralID::width);

template<>
std::optional<storage::node::identifier::LiteralID> capabilities::Inlineable<xsd_gMonthDay>::try_into_inlined(cpp_type const &value) noexcept {
    IHelp i{InliningHelperMonthDay{static_cast<uint8_t>(static_cast<unsigned int>(value.first.month())), static_cast<uint8_t>(static_cast<unsigned int>(value.first.day()))}, value.second};
    return util::pack<storage::node::identifier::LiteralID>(i);
}

template<>
capabilities::Inlineable<xsd_gMonthDay>::cpp_type capabilities::Inlineable<xsd_gMonthDay>::from_inlined(storage::node::identifier::LiteralID inlined) noexcept {
    auto i = util::unpack<IHelp>(inlined);
    return std::make_pair(std::chrono::month{i.time_value.month} / std::chrono::day{i.time_value.day}, i.decode_tz());
}

template<>
std::partial_ordering capabilities::Comparable<xsd_gMonthDay>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    auto md_to_tp = [](std::chrono::month_day md) noexcept -> rdf::util::TimePoint {
        return rdf::util::construct(rdf::util::TimePointReplacementDate.year() / md, rdf::util::TimePointReplacementTimeOfDay);
    };
    return registry::util::compare_time_points(md_to_tp(lhs.first), lhs.second, md_to_tp(rhs.first), rhs.second);
}

template<>
template<>
capabilities::Subtype<xsd_gMonthDay>::super_cpp_type<0> capabilities::Subtype<xsd_gMonthDay>::into_supertype<0>(cpp_type const &value) noexcept {
    return std::make_pair(rdf::util::TimePointReplacementDate.year() / value.first, value.second);
}

template<>
template<>
nonstd::expected<capabilities::Subtype<xsd_gMonthDay>::cpp_type, DynamicError> capabilities::Subtype<xsd_gMonthDay>::from_supertype<0>(super_cpp_type<0> const &value) noexcept {
    return std::make_pair(value.first.month() / value.first.day(), value.second);
}
#endif

template struct LiteralDatatypeImpl<xsd_gMonthDay,
                                    capabilities::Comparable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable,
                                    capabilities::Subtype>;

}  // namespace rdf4cpp::rdf::datatypes::registry