#include <rdf4cpp/rdf/datatypes/xsd/time/YearMonth.hpp>

#include <rdf4cpp/rdf/datatypes/registry/util/DateTimeUtils.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_gYearMonth>::cpp_type capabilities::Default<xsd_gYearMonth>::from_string(std::string_view s) {
    using namespace registry::util;
    auto year = parse_date_time_fragment<std::chrono::year, int, '-'>(s);
    auto tz = rdf::util::Timezone::parse_optional(s);
    auto month = parse_date_time_fragment<std::chrono::month, unsigned int, '\0'>(s);
    auto date = year / month;
    if (!date.ok())
        throw std::invalid_argument("invalid date");

    return std::make_pair(date, tz);
}

template<>
std::string capabilities::Default<xsd_gYearMonth>::to_canonical_string(const cpp_type &value) noexcept {
    auto str = std::format("{:%Y-%m}", value.first);
    if (value.second.has_value())
        str += value.second->to_canonical_string();
    return str;
}

struct __attribute__((__packed__)) InliningHelperYearMonth {
    int16_t year;
    uint8_t month;
};
static_assert(sizeof(std::chrono::year_month) * 8 <= storage::node::identifier::LiteralID::width);

template<>
std::optional<storage::node::identifier::LiteralID> capabilities::Inlineable<xsd_gYearMonth>::try_into_inlined(cpp_type const &value) noexcept {
    if (value.second.has_value())
        return std::nullopt;
    return util::pack<storage::node::identifier::LiteralID>(InliningHelperYearMonth{static_cast<int16_t>(static_cast<int>(value.first.year())),
                                                                                    static_cast<uint8_t>(static_cast<unsigned int>(value.first.month()))});
}

template<>
capabilities::Inlineable<xsd_gYearMonth>::cpp_type capabilities::Inlineable<xsd_gYearMonth>::from_inlined(storage::node::identifier::LiteralID inlined) noexcept {
    auto i = util::unpack<InliningHelperYearMonth>(inlined);
    return std::make_pair(std::chrono::year{i.year} / std::chrono::month{i.month}, std::nullopt);
}

template<>
std::partial_ordering capabilities::Comparable<xsd_gYearMonth>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    auto ym_to_tp = [](std::chrono::year_month t) noexcept -> rdf::util::TimePoint {
        return rdf::util::construct(t / std::chrono::last, rdf::util::TimePointReplacementTimeOfDay);
    };
    return registry::util::compare_time_points(ym_to_tp(lhs.first), lhs.second, ym_to_tp(rhs.first), rhs.second);
}

template<>
template<>
capabilities::Subtype<xsd_gYearMonth>::super_cpp_type<0> capabilities::Subtype<xsd_gYearMonth>::into_supertype<0>(cpp_type const &value) noexcept {
    return std::make_pair(value.first / std::chrono::last, value.second);
}

template<>
template<>
nonstd::expected<capabilities::Subtype<xsd_gYearMonth>::cpp_type, DynamicError> capabilities::Subtype<xsd_gYearMonth>::from_supertype<0>(super_cpp_type<0> const &value) noexcept {
    return std::make_pair(value.first.year() / value.first.month(), value.second);
}

template struct LiteralDatatypeImpl<xsd_gYearMonth,
                                    capabilities::Comparable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable,
                                    capabilities::Subtype>;
}  // namespace rdf4cpp::rdf::datatypes::registry