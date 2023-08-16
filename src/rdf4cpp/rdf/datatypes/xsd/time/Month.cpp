#include <rdf4cpp/rdf/datatypes/xsd/time/Month.hpp>

#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_gMonth>::cpp_type capabilities::Default<xsd_gMonth>::from_string(std::string_view s) {
    auto tz = Timezone::try_parse(s);
    auto month = parse_date_time_fragment<std::chrono::month, unsigned int, '\0'>(tz.second);
    if (!month.ok())
        throw std::invalid_argument("invalid month");

    return std::make_pair(month, tz.first);
}

template<>
std::string capabilities::Default<xsd_gMonth>::to_canonical_string(const cpp_type &value) noexcept {
    auto str = std::format("{:%m}", value.first);
    if (value.second.has_value())
        str += value.second->to_canonical_string();
    return str;
}

template<>
std::partial_ordering capabilities::Comparable<xsd_gMonth>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    return TimeComparer<std::chrono::month>::compare(lhs.first, lhs.second, rhs.first, rhs.second);
}

using IHelp = InliningHelper<uint8_t>;
static_assert(numberOfBits(12u) == 4);
static_assert(sizeof(std::chrono::month) == 1);
static_assert(sizeof(IHelp) * 8 < storage::node::identifier::LiteralID::width);

template<>
std::optional<storage::node::identifier::LiteralID> capabilities::Inlineable<xsd_gMonth>::try_into_inlined(cpp_type const &value) noexcept {
    IHelp i{static_cast<uint8_t>(static_cast<unsigned int>(value.first)), value.second};
    return util::pack<storage::node::identifier::LiteralID>(i);
}

template<>
capabilities::Inlineable<xsd_gMonth>::cpp_type capabilities::Inlineable<xsd_gMonth>::from_inlined(storage::node::identifier::LiteralID inlined) noexcept {
    auto i = util::unpack<IHelp>(inlined);
    return std::make_pair(std::chrono::month{i.time_value}, i.decode_tz());
}

template<>
template<>
capabilities::Subtype<xsd_gMonth>::super_cpp_type<0> capabilities::Subtype<xsd_gMonth>::into_supertype<0>(cpp_type const &value) noexcept {
    return std::make_pair(TimePointReplacementDate.year() / value.first / std::chrono::last, value.second);
}

template<>
template<>
nonstd::expected<capabilities::Subtype<xsd_gMonth>::cpp_type, DynamicError> capabilities::Subtype<xsd_gMonth>::from_supertype<0>(super_cpp_type<0> const &value) noexcept {
    return std::make_pair(value.first.month(), value.second);
}

template struct LiteralDatatypeImpl<xsd_gMonth,
                                    capabilities::Comparable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable,
                                    capabilities::Subtype>;

template<>
TimePoint to_point_on_timeline<std::chrono::month>(std::chrono::month t) {
    return construct(TimePointReplacementDate.year() / t / std::chrono::last, TimePointReplacementTimeOfDay);
}
}  // namespace rdf4cpp::rdf::datatypes::registry
