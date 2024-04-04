#include "CharMatcher.hpp"


#include <hwy/contrib/algo/transform-inl.h>
#include <hwy/highway.h>


HWY_BEFORE_NAMESPACE();  // at file scope
namespace rdf4cpp::util::char_matcher_detail::HWY_NAMESPACE {

    std::optional<bool> try_match_simd_impl(std::string_view data, std::array<CharRange, 3> const &ranges, std::string_view single) {
        bool found_unicode = false;
        bool r = true;
        hwy::HWY_NAMESPACE::ScalableTag<uint8_t> d;
        auto r0b = hwy::HWY_NAMESPACE::Set(d, static_cast<uint8_t>(ranges[0].first));
        auto r0e = hwy::HWY_NAMESPACE::Set(d, static_cast<uint8_t>(ranges[0].last));
        bool r1a = ranges[1].first < ranges[1].last;
        auto r1b = hwy::HWY_NAMESPACE::Set(d, static_cast<uint8_t>(ranges[1].first));
        auto r1e = hwy::HWY_NAMESPACE::Set(d, static_cast<uint8_t>(ranges[1].last));
        bool r2a = ranges[2].first < ranges[2].last;
        auto r2b = hwy::HWY_NAMESPACE::Set(d, static_cast<uint8_t>(ranges[2].first));
        auto r2e = hwy::HWY_NAMESPACE::Set(d, static_cast<uint8_t>(ranges[2].last));
        auto unicode_bit = hwy::HWY_NAMESPACE::Set(d, 7);
        hwy::HWY_NAMESPACE::Foreach(d, reinterpret_cast<uint8_t const *>(data.data()), data.size(), r0b, [&](auto d, auto in_vec) HWY_ATTR {
            if (!hwy::HWY_NAMESPACE::AllTrue(d, hwy::HWY_NAMESPACE::HighestSetBitIndex(in_vec) < unicode_bit)) {
                found_unicode = true;
                return;
            }

            auto m = hwy::HWY_NAMESPACE::And(in_vec >= r0b, in_vec <= r0e);
            if (r1a) {
                m = hwy::HWY_NAMESPACE::Or(m, hwy::HWY_NAMESPACE::And(in_vec >= r1b, in_vec <= r1e));
            }
            if (r2a) {
                m = hwy::HWY_NAMESPACE::Or(m, hwy::HWY_NAMESPACE::And(in_vec >= r2b, in_vec <= r2e));
            }
            r &= hwy::HWY_NAMESPACE::AllTrue(d, m);
        });
        if (found_unicode)
            return std::nullopt;
        return r;
    }

    // NOLINTNEXTLINE(google-readability-namespace-comments)
}  // namespace rdf4cpp::util::char_matcher_detail::HWY_NAMESPACE
HWY_AFTER_NAMESPACE();

namespace rdf4cpp::util::char_matcher_detail {
    std::optional<bool> try_match_simd(std::string_view data, std::array<CharRange, 3> const &ranges, std::string_view single) {
        return HWY_STATIC_DISPATCH(try_match_simd_impl)(data, ranges, single);
    }
}  // namespace rdf4cpp::util::char_matcher_detail
