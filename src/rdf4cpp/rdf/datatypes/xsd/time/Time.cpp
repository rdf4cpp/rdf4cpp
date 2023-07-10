#include <rdf4cpp/rdf/datatypes/xsd/time/Time.hpp>

#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_time>::cpp_type capabilities::Default<xsd_time>::from_string(std::string_view s) {
    auto hours = parse_date_time_fragment<std::chrono::hours, unsigned int, ':'>(s);
    auto minutes = parse_date_time_fragment<std::chrono::minutes, unsigned int, ':'>(s);
    auto tz = Timezone::try_parse(s);
    std::chrono::milliseconds ms = parse_milliseconds(tz.second);
    if (minutes < std::chrono::minutes(0) || minutes > std::chrono::hours(1))
        throw std::invalid_argument{"minutes out of range"};
    if (hours < std::chrono::hours(0) || hours > std::chrono::days(1))
        throw std::invalid_argument{"hours out of range"};
    if (ms < std::chrono::seconds(0) || ms > std::chrono::minutes(1))
        throw std::invalid_argument{"seconds out of range"};
    auto time = hours + minutes + ms;
    if (time > std::chrono::hours{24})
        throw std::invalid_argument{"invalid time of day"};

    return std::make_pair(time, tz.first);
}

template<>
std::string capabilities::Default<xsd_time>::to_canonical_string(const cpp_type &value) noexcept {
    auto str = std::format("{:%H:%M:%S}", std::chrono::hh_mm_ss(value.first));
    if (value.second.has_value())
        str += value.second->to_canonical_string();
    return str;
}

using IHelp = InliningHelperPacked;
static_assert(numberOfBits(static_cast<unsigned int>(std::chrono::seconds{std::chrono::hours{24}}.count())) == 17);

template<>
std::optional<storage::node::identifier::LiteralID> capabilities::Inlineable<xsd_time>::try_into_inlined(cpp_type const &value) noexcept {
    if ((value.first - std::chrono::floor<std::chrono::seconds>(value.first)) != std::chrono::milliseconds(0))
        return std::nullopt;
    auto sec = std::chrono::floor<std::chrono::seconds>(value.first);
    if (sec < std::chrono::seconds(0) || sec > std::chrono::hours(24))
        return std::nullopt; // should never happen
    IHelp i{static_cast<uint32_t>(sec.count()), value.second};
    return util::pack<storage::node::identifier::LiteralID>(i);
}

template<>
capabilities::Inlineable<xsd_time>::cpp_type capabilities::Inlineable<xsd_time>::from_inlined(storage::node::identifier::LiteralID inlined) noexcept {
    auto i = util::unpack<IHelp>(inlined);
    return std::make_pair(std::chrono::seconds(i.time_value), i.decode_tz());
}

template<>
std::partial_ordering capabilities::Comparable<xsd_time>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    return TimeComparer<std::chrono::milliseconds>::compare(lhs.first, lhs.second, rhs.first, rhs.second);
}

template<>
TimePoint to_point_on_timeline<std::chrono::milliseconds>(std::chrono::milliseconds t) {
    return construct(TimePointReplacementDate, t);
}

template struct LiteralDatatypeImpl<xsd_time,
                                    capabilities::Comparable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable>;
}  // namespace rdf4cpp::rdf::datatypes::registry