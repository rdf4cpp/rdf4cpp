#include "Date.hpp"

#include <rdf4cpp/datatypes/registry/util/DateTimeUtils.hpp>

namespace rdf4cpp::datatypes::registry {

#ifndef DOXYGEN_PARSER
template<>
capabilities::Default<xsd_date>::cpp_type capabilities::Default<xsd_date>::from_string(std::string_view s) {
    using namespace datatypes::registry::util;
    auto year = parse_date_time_fragment<RDFYear, boost::multiprecision::checked_int128_t, '-', identifier>(s);
    auto month = parse_date_time_fragment<std::chrono::month, unsigned int, '-', identifier>(s);
    auto tz = rdf4cpp::Timezone::parse_optional(s, identifier);
    auto day = parse_date_time_fragment<std::chrono::day, unsigned int, '\0', identifier>(s);
    auto date = RDFDate{year, month, day};
    if (registry::relaxed_parsing_mode && !date.ok()) {
        date = normalize(date);
    }
    if (!date.ok()) {
        throw InvalidNode(std::format("{} parsing error: {} is invalid", identifier, date));
    }

    return std::make_pair(date, tz);
}

template<>
bool capabilities::Default<xsd_date>::serialize_canonical_string(cpp_type const &value, writer::BufWriterParts writer) noexcept {
    //year,-,month,-,day,tz
    std::array<char, registry::util::chrono_max_canonical_string_chars::year + 1 +
                             registry::util::chrono_max_canonical_string_chars::month + 1 +
                             registry::util::chrono_max_canonical_string_chars::day + Timezone::max_canonical_string_chars>
            buff;
    char *it = std::format_to(buff.data(), "{}", value.first);
    if (value.second.has_value()) {
        it = value.second->to_canonical_string(it);
    }
    size_t const len = it - buff.data();
    assert(len <= buff.size());
    return writer::write_str(std::string_view(buff.data(), len), writer);
}

struct __attribute__((__packed__)) InliningHelperYearMonthDay {
    int16_t year;
    uint8_t month, day;
};
static_assert(sizeof(std::chrono::year_month_day) * 8 <= storage::identifier::LiteralID::width);

template<>
std::optional<storage::identifier::LiteralID> capabilities::Inlineable<xsd_date>::try_into_inlined(cpp_type const &value) noexcept {
    if (value.second.has_value()) {
        return std::nullopt;
    }
    auto i = value.first.to_time_point().time_since_epoch().count();
    if (!util::fits_into<int64_t>(i)) [[unlikely]] {
        return std::nullopt;
    }
    return util::try_pack_integral<storage::identifier::LiteralID>(static_cast<int64_t>(i));
}

template<>
capabilities::Inlineable<xsd_date>::cpp_type capabilities::Inlineable<xsd_date>::from_inlined(storage::identifier::LiteralID inlined) noexcept {
    boost::multiprecision::checked_int128_t const i {util::unpack_integral<int64_t>(inlined)};
    return capabilities::Inlineable<xsd_date>::cpp_type{RDFDate::time_point{RDFDate::time_point::duration{i}}, std::nullopt};
}

rdf4cpp::TimePoint date_to_tp(const RDFDate& d) noexcept {
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
    return std::make_pair(RDFDate {std::chrono::floor<std::chrono::days>(value.first)}, value.second);
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
