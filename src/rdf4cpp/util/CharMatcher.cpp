#include "CharMatcher.hpp"

#undef HWY_TARGET_INCLUDE
#define HWY_TARGET_INCLUDE "rdf4cpp/util/CharMatcher.cpp"
#include <hwy/foreach_target.h>

#include <cassert>
#include <hwy/highway.h>
#include <stdexcept>


HWY_BEFORE_NAMESPACE();  // at file scope
namespace rdf4cpp::util::char_matcher_detail::HWY_NAMESPACE {
    // Calls `func(d, v)` for each input vector; out of bound lanes with index i >=
    // `count` are instead taken from `no[i % Lanes(d)]`.
    // if func returns false, exits early
    // from hwy/contrib/algo/transform-inl.h
    // TODO move to some header, probably will get used somewhere else
    template<class D, class Func, typename T = hwy::HWY_NAMESPACE::TFromD<D>>
    void Foreach(D d, T const *HWY_RESTRICT in, size_t const count, hwy::HWY_NAMESPACE::Vec<D> const no,
                 Func const &func) {
        size_t const N = Lanes(d);

        size_t idx = 0;
        if (count >= N) {
            for (; idx <= count - N; idx += N) {
                hwy::HWY_NAMESPACE::Vec<D> const v = LoadU(d, in + idx);
                if (!func(d, v)) {
                    return;
                }
            }
        }

        // `count` was a multiple of the vector length `N`: already done.
        if (HWY_UNLIKELY(idx == count))
            return;

        size_t const remaining = count - idx;
        HWY_DASSERT(0 != remaining && remaining < N);
        hwy::HWY_NAMESPACE::Vec<D> const v = LoadNOr(no, d, in + idx, remaining);
        func(d, v);
    }

    std::optional<bool> try_match_simd_impl(std::string_view data, std::array<CharRange, 3> const &ranges, std::string_view single) {
        // register usage:
        // 1 - data
        // 6 - ranges
        // 5 - logic
        // ? - singles
        // 16 overall
        using namespace hwy::HWY_NAMESPACE;
        bool found_unicode = false;
        bool r = true;
        using D = ScalableTag<int8_t>;  //NOLINT
        using V = VFromD<D>;             //NOLINT
        D d;
        bool r0a = ranges[0].first < ranges[0].last;
        V zero = r0a ? Set(d, static_cast<int8_t>(ranges[0].first)) : Set(d, static_cast<int8_t>(single.at(0)));
        V r0b = Set(d, static_cast<int8_t>(ranges[0].first - 1));
        V r0e = Set(d, static_cast<int8_t>(ranges[0].last + 1));
        bool r1a = ranges[1].first < ranges[1].last;
        V r1b = Set(d, static_cast<int8_t>(ranges[1].first - 1));
        V r1e = Set(d, static_cast<int8_t>(ranges[1].last + 1));
        bool r2a = ranges[2].first < ranges[2].last;
        V r2b = Set(d, static_cast<int8_t>(ranges[2].first - 1));
        V r2e = Set(d, static_cast<int8_t>(ranges[2].last + 1));
        V unicode_bit = Set(d, 7);

        // if this gets ported to variable size vectors, this needs to be changed (like above)
        // this array prevents keeping in registers, but there are not enough registers anyway
        std::array<V, simd_max_single_chars> si{};
        size_t si_num = single.size();
        if (si_num > si.size()) {
            assert(false);
            return std::nullopt;
        }
        for (size_t i = 0; i < si_num; ++i) {
            si[i] = Set(d, static_cast<int8_t>(single[i]));
        }

        Foreach(d, reinterpret_cast<int8_t const *>(data.data()), data.size(), zero, [&](auto d, auto in_vec) HWY_ATTR {
            if (!AllTrue(d, HighestSetBitIndex(in_vec) < unicode_bit)) {
                found_unicode = true;
                return false;
            }

            // highway doc: on x86 < and > are 1 instruction for signed ints (3 for unsigned)
            // and <= and >= are 2 instructions regardless of signed/unsigned
            auto m = r0a ? And(in_vec > r0b, in_vec < r0e) : FirstN(d, 0);
            if (r1a) {
                m = Or(m, And(in_vec > r1b, in_vec < r1e));
            }
            if (r2a) {
                m = Or(m, And(in_vec > r2b, in_vec < r2e));
            }
            for (size_t i = 0; i < si_num; ++i) {
                m = Or(m, in_vec == si[i]);
            }
            r = r && AllTrue(d, m);
            return r;
        });

        if (found_unicode)
            return std::nullopt;
        return r;
    }

    bool contains_any_impl(std::string_view data, std::array<char, 4> match) {
        using namespace hwy::HWY_NAMESPACE;
        bool r = false;
        using D = ScalableTag<int8_t>;  //NOLINT
        using V = VFromD<D>;             //NOLINT
        D d;
        V zero = Set(d, static_cast<int8_t>(0));
        V m0 = Set(d, static_cast<int8_t>(match[0]));
        V m1 = Set(d, static_cast<int8_t>(match[1]));
        V m2 = Set(d, static_cast<int8_t>(match[2]));
        V m3 = Set(d, static_cast<int8_t>(match[3]));

        Foreach(d, reinterpret_cast<int8_t const *>(data.data()), data.size(), zero, [&](auto d, auto in_vec) HWY_ATTR {
            auto m = in_vec == m0;
            m = Or(m, in_vec == m1);
            m = Or(m, in_vec == m2);
            m = Or(m, in_vec == m3);
            r = r || !AllFalse(d, m);
            return !r;
        });

        return r;
    }
    // NOLINTNEXTLINE(google-readability-namespace-comments)
}  // namespace rdf4cpp::util::char_matcher_detail::HWY_NAMESPACE
HWY_AFTER_NAMESPACE();

#if HWY_ONCE
namespace rdf4cpp::util::char_matcher_detail {
    HWY_EXPORT(try_match_simd_impl);
    HWY_EXPORT(contains_any_impl);

    std::optional<bool> try_match_simd(std::string_view data, std::array<CharRange, 3> const &ranges, std::string_view single) {
        return HWY_DYNAMIC_DISPATCH(try_match_simd_impl)(data, ranges, single);
    }

    bool contains_any(std::string_view data, std::array<char, 4> match) {
        return HWY_DYNAMIC_DISPATCH(contains_any_impl)(data, match);
    }

    void test_simd_foreach_supported(void(*func)(std::string_view target_name)) {
        auto targets = hwy::SupportedAndGeneratedTargets();
        for (const auto t : targets) {
            hwy::SetSupportedTargetsForTest(t);
            func(hwy::TargetName(t));
        }
        hwy::SetSupportedTargetsForTest(0);
    }
}  // namespace rdf4cpp::util::char_matcher_detail
#endif
