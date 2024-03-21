#include "Date.hpp"

#include <rdf4cpp/datatypes/registry/util/DateTimeUtils.hpp>

namespace rdf4cpp::datatypes::registry {

#ifndef DOXYGEN_PARSER
template<>
capabilities::Default<xsd_date>::cpp_type capabilities::Default<xsd_date>::from_string(std::string_view s) {
    using namespace datatypes::registry::util;
    auto year = parse_date_time_fragment<std::chrono::year, int, '-', identifier>(s);
    auto month = parse_date_time_fragment<std::chrono::month, unsigned int, '-', identifier>(s);
    auto tz = rdf4cpp::Timezone::parse_optional(s);
    auto day = parse_date_time_fragment<std::chrono::day, unsigned int, '\0', identifier>(s);
    auto date = year / month / day;
    if (registry::relaxed_parsing_mode && !date.ok())
        date = normalize(date);
    if (!date.ok()) {
        throw std::runtime_error("invalid date");
    }

    return std::make_pair(date, tz);
}

template<>
bool capabilities::Default<xsd_date>::serialize_canonical_string(cpp_type const &value, writer::BufWriterParts writer) noexcept {
    auto str = std::format("{:%Y-%m-%d}", value.first);
    if (value.second.has_value())
        str += value.second->to_canonical_string();

    return writer::write_str(str, writer);
}

struct __attribute__((__packed__)) InliningHelperYearMonthDay {
    int16_t year;
    uint8_t month, day;
};
static_assert(sizeof(std::chrono::year_month_day) * 8 <= storage::identifier::LiteralID::width);

template<>
std::optional<storage::identifier::LiteralID> capabilities::Inlineable<xsd_date>::try_into_inlined(cpp_type const &value) noexcept {
    if (value.second.has_value())
        return std::nullopt;
    return util::pack<storage::identifier::LiteralID>(InliningHelperYearMonthDay{static_cast<int16_t>(static_cast<int>(value.first.year())),
                                                                                       static_cast<uint8_t>(static_cast<unsigned int>(value.first.month())),
                                                                                       static_cast<uint8_t>(static_cast<unsigned int>(value.first.day()))});
}

template<>
capabilities::Inlineable<xsd_date>::cpp_type capabilities::Inlineable<xsd_date>::from_inlined(storage::identifier::LiteralID inlined) noexcept {
    auto i = util::unpack<InliningHelperYearMonthDay>(inlined);
    return std::make_pair(std::chrono::year{i.year} / std::chrono::month{i.month} / std::chrono::day{i.day}, std::nullopt);
}

    rdf4cpp::TimePoint date_to_tp(std::chrono::year_month_day d) noexcept {
    return rdf4cpp::util::construct_timepoint(d, rdf4cpp::util::time_point_replacement_time_of_day);
}

template<>
template<>
capabilities::Subtype<xsd_date>::super_cpp_type<0> capabilities::Subtype<xsd_date>::into_supertype<0>(cpp_type const &value) noexcept {
    return std::make_pair(date_to_tp(value.first), value.second);
}

template<>
template<>
nonstd::expected<capabilities::Subtype<xsd_date>::cpp_type, DynamicError> capabilities::Subtype<xsd_date>::from_supertype<0>(super_cpp_type<0> const &value) noexcept {
    return std::make_pair(std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(value.first)}, value.second);
}

template<>
std::partial_ordering capabilities::Comparable<xsd_date>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    return rdf4cpp::datatypes::registry::util::compare_time_points(date_to_tp(lhs.first), lhs.second, date_to_tp(rhs.first), rhs.second);
}
#endif

template struct LiteralDatatypeImpl<xsd_date,
                                    capabilities::Comparable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable,
                                    capabilities::Subtype>;
}  // namespace rdf4cpp::datatypes::registry
