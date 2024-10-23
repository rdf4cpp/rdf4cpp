#include "YearMonth.hpp"

#include <rdf4cpp/datatypes/registry/util/DateTimeUtils.hpp>

namespace rdf4cpp::datatypes::registry {

#ifndef DOXYGEN_PARSER
template<>
capabilities::Default<xsd_gYearMonth>::cpp_type capabilities::Default<xsd_gYearMonth>::from_string(std::string_view s) {
    using namespace registry::util;
    auto year = parse_date_time_fragment<RDFYear, boost::multiprecision::checked_int128_t, '-', identifier>(s);
    auto tz = rdf4cpp::Timezone::parse_optional(s, identifier);
    auto month = parse_date_time_fragment<std::chrono::month, unsigned int, '\0', identifier>(s);
    auto date = RDFYearMonth{year, month};
    if (!month.ok()) {
        throw InvalidNode(std::format("{} parsing error: {} is invalid", identifier, date));
    }

    return std::make_pair(date, tz);
}

template<>
bool capabilities::Default<xsd_gYearMonth>::serialize_canonical_string(cpp_type const &value, writer::BufWriterParts writer) noexcept {
    //year,-,month,tz
    std::array<char, registry::util::chrono_max_canonical_string_chars::year + 1 +
                             registry::util::chrono_max_canonical_string_chars::month + Timezone::max_canonical_string_chars>
            buff;
    char *it = std::format_to(buff.data(), "{}", value.first);
    if (value.second.has_value()) {
        it = value.second->to_canonical_string(it);
    }
    size_t const len = it - buff.data();
    assert(len <= buff.size());
    return writer::write_str(std::string_view(buff.data(), len), writer);
}

struct __attribute__((__packed__)) InliningHelperYearMonth {
    int16_t year;
    uint8_t month;
};
static_assert(sizeof(std::chrono::year_month) * 8 <= storage::identifier::LiteralID::width);

template<>
std::optional<storage::identifier::LiteralID> capabilities::Inlineable<xsd_gYearMonth>::try_into_inlined(cpp_type const &value) noexcept {
    if (value.second.has_value()) [[unlikely]] {
        return std::nullopt;
    }
    if (!util::fits_into<int16_t>(value.first.year.year)) [[unlikely]] {
        return std::nullopt;
    }
    return util::pack<storage::identifier::LiteralID>(InliningHelperYearMonth{static_cast<int16_t>(value.first.year.year),
                                                                                    static_cast<uint8_t>(static_cast<unsigned int>(value.first.month))});
}

template<>
capabilities::Inlineable<xsd_gYearMonth>::cpp_type capabilities::Inlineable<xsd_gYearMonth>::from_inlined(storage::identifier::LiteralID inlined) noexcept {
    auto i = util::unpack<InliningHelperYearMonth>(inlined);
    return std::make_pair(RDFYearMonth{RDFYear{i.year}, std::chrono::month{i.month}}, std::nullopt);
}

template<>
std::partial_ordering capabilities::Comparable<xsd_gYearMonth>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    auto ym_to_tp = [](const RDFYearMonth& t) -> rdf4cpp::TimePoint {
        return rdf4cpp::util::construct_timepoint(RDFDate{t.year, t.month, std::chrono::last}, rdf4cpp::util::time_point_replacement_time_of_day);
    };
    return registry::util::compare_time_points(ym_to_tp(lhs.first), lhs.second, ym_to_tp(rhs.first), rhs.second);
}

template<>
template<>
capabilities::Subtype<xsd_gYearMonth>::super_cpp_type<0> capabilities::Subtype<xsd_gYearMonth>::into_supertype<0>(cpp_type const &value) noexcept {
    return std::make_pair(RDFDate{value.first.year, value.first.month, std::chrono::last}, value.second);
}

template<>
template<>
nonstd::expected<capabilities::Subtype<xsd_gYearMonth>::cpp_type, DynamicError> capabilities::Subtype<xsd_gYearMonth>::from_supertype<0>(super_cpp_type<0> const &value) noexcept {
    return std::make_pair(RDFYearMonth{value.first.year, value.first.month}, value.second);
}
#endif

template struct LiteralDatatypeImpl<xsd_gYearMonth,
                                    capabilities::Comparable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable,
                                    capabilities::Subtype>;
}  // namespace rdf4cpp::datatypes::registry