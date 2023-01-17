#include <rdf4cpp/rdf/datatypes/xsd/Float.hpp>
#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

#include <cmath>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_float>::cpp_type capabilities::Default<xsd_float>::from_string(std::string_view s) {
    return util::from_chars<cpp_type>(s);
}

template<>
std::string capabilities::Default<xsd_float>::to_string(cpp_type const &value) noexcept {
    return util::to_chars(value);
}

template<>
std::string capabilities::Default<xsd_float>::display(cpp_type const &value) noexcept {
    if (value == 0) {
        return std::signbit(value) ? "-0" : "0";
    }

    if (auto const abs = std::abs(value); abs >= 0.000001 && abs < 1000000) {
        static constexpr size_t buf_sz = 2 + std::numeric_limits<cpp_type>::max_exponent10 + std::numeric_limits<cpp_type>::max_digits10;
        std::array<char, buf_sz> buf;

        auto const res = std::to_chars(buf.data(), buf.data() + buf.size(), value, std::chars_format::fixed);
        assert(res.ec == std::errc{});

        return std::string{buf.data(), static_cast<std::string::size_type>(res.ptr - buf.data())};
    }

    return to_string(value);
}

template<>
bool capabilities::Logical<xsd_float>::effective_boolean_value(cpp_type const &value) noexcept {
    return !std::isnan(value) && value != 0.f;
}

template<>
std::optional<uint64_t> capabilities::Inlineable<xsd_float>::try_into_inlined(cpp_type const &value) noexcept {
    return util::pack<uint64_t>(value);
}

template<>
capabilities::Inlineable<xsd_float>::cpp_type capabilities::Inlineable<xsd_float>::from_inlined(uint64_t const inlined) noexcept {
    return util::unpack<cpp_type>(inlined);
}

template struct LiteralDatatypeImpl<xsd_float,
                                    capabilities::Logical,
                                    capabilities::Numeric,
                                    capabilities::Comparable,
                                    capabilities::Promotable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable>;

}  // namespace rdf4cpp::rdf::datatypes::registry
