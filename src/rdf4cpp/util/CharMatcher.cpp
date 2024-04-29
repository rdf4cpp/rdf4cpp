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
    template<class D, class Func, typename T = hwy::HWY_NAMESPACE::TFromD<D>>
    HWY_INLINE void Foreach(D d, T const *HWY_RESTRICT in, size_t const count, hwy::HWY_NAMESPACE::Vec<D> const no,
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

    template<size_t rn, size_t sn>
    HWY_INLINE std::optional<bool> try_match_simd_impl(std::string_view data, std::array<CharRange, rn> const &ranges, datatypes::registry::util::ConstexprString<sn> const &single) {
        // register usage (3 ranges):
        // 1 - data
        // 6 - ranges
        // 5 - logic
        // ? - singles
        // 16 overall
        using namespace hwy::HWY_NAMESPACE;
        bool found_unicode = false;
        bool r = true;
        using D = ScalableTag<int8_t>;  //NOLINT tag type, selects the used vector type
        using V = Vec<D>;             //NOLINT vector type
        D d;
        bool has_range_0 = rn > 0;
        V const zero = has_range_0 ? Set(d, static_cast<int8_t>(ranges[0].first)) : Set(d, static_cast<int8_t>(single.value.at(0)));
        std::array<std::pair<V, V>, rn> range_vectors;
        for (size_t i = 0; i < rn; ++i) {
            range_vectors[i].first = Set(d, static_cast<int8_t>(ranges[i].first - 1));
            range_vectors[i].second = Set(d, static_cast<int8_t>(ranges[i].last + 1));
        }
        V unicode_bit = Set(d, 7);

        std::array<V, sn-1> single_vectors{};
        auto view = static_cast<std::string_view>(single);
        for (size_t i = 0; i < view.size(); ++i) {
            single_vectors[i] = Set(d, static_cast<int8_t>(view[i]));
        }

        Foreach(d, reinterpret_cast<int8_t const *>(data.data()), data.size(), zero, [&](auto d, auto in_vec) HWY_ATTR {
            if (!AllTrue(d, HighestSetBitIndex(in_vec) < unicode_bit)) {
                found_unicode = true;
                return false;
            }

            // highway doc: on x86 < and > are 1 instruction for signed ints (3 for unsigned)
            // and <= and >= are 2 instructions regardless of signed/unsigned
            auto m = has_range_0 ? And(in_vec > range_vectors[0].first, in_vec < range_vectors[0].second) : FirstN(d, 0);
            for (size_t i = 1; i < rn; ++i) {
                m = Or(m, And(in_vec > range_vectors[i].first, in_vec < range_vectors[i].second));
            }
            for (size_t i = 0; i < sn-1; ++i) {
                m = Or(m, in_vec == single_vectors[i]);
            }
            r = r && AllTrue(d, m);
            return r;
        });

        if (found_unicode)
            return std::nullopt;
        return r;
    }
    std::optional<bool> try_match_simd_impl_3_1(std::string_view data, std::array<CharRange, 3> const &ranges, datatypes::registry::util::ConstexprString<1> const &single) {
        return try_match_simd_impl(data, ranges, single);
    }
    std::optional<bool> try_match_simd_impl_3_4(std::string_view data, std::array<CharRange, 3> const &ranges, datatypes::registry::util::ConstexprString<4> const &single) {
        return try_match_simd_impl(data, ranges, single);
    }
    std::optional<bool> try_match_simd_impl_3_18(std::string_view data, std::array<CharRange, 3> const &ranges, datatypes::registry::util::ConstexprString<18> const &single) {
        return try_match_simd_impl(data, ranges, single);
    }
    std::optional<bool> try_match_simd_impl_3_20(std::string_view data, std::array<CharRange, 3> const &ranges, datatypes::registry::util::ConstexprString<20> const &single) {
        return try_match_simd_impl(data, ranges, single);
    }
    std::optional<bool> try_match_simd_impl_3_21(std::string_view data, std::array<CharRange, 3> const &ranges, datatypes::registry::util::ConstexprString<21> const &single) {
        return try_match_simd_impl(data, ranges, single);
    }
    std::optional<bool> try_match_simd_impl_1_1(std::string_view data, std::array<CharRange, 1> const &ranges, datatypes::registry::util::ConstexprString<1> const &single) {
        return try_match_simd_impl(data, ranges, single);
    }

    bool contains_any_impl(std::string_view data, std::array<char, 4> match) {
        using namespace hwy::HWY_NAMESPACE;
        bool r = false;
        using D = ScalableTag<int8_t>;  //NOLINT  tag type, selects the used vector type
        using V = Vec<D>;             //NOLINT vector type
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
    HWY_EXPORT(try_match_simd_impl_3_1);
    HWY_EXPORT(try_match_simd_impl_3_4);
    HWY_EXPORT(try_match_simd_impl_3_18);
    HWY_EXPORT(try_match_simd_impl_3_20);
    HWY_EXPORT(try_match_simd_impl_3_21);
    HWY_EXPORT(try_match_simd_impl_1_1);
    HWY_EXPORT(contains_any_impl);

    template<>
    std::optional<bool> try_match_simd(std::string_view data, std::array<CharRange, 3> const &ranges, datatypes::registry::util::ConstexprString<1> const &single) {
        return HWY_DYNAMIC_DISPATCH(try_match_simd_impl_3_1)(data, ranges, single);
    }
    template<>
    std::optional<bool> try_match_simd(std::string_view data, std::array<CharRange, 3> const &ranges, datatypes::registry::util::ConstexprString<4> const &single) {
        return HWY_DYNAMIC_DISPATCH(try_match_simd_impl_3_4)(data, ranges, single);
    }
    template<>
    std::optional<bool> try_match_simd(std::string_view data, std::array<CharRange, 3> const &ranges, datatypes::registry::util::ConstexprString<18> const &single) {
        return HWY_DYNAMIC_DISPATCH(try_match_simd_impl_3_18)(data, ranges, single);
    }
    template<>
    std::optional<bool> try_match_simd(std::string_view data, std::array<CharRange, 3> const &ranges, datatypes::registry::util::ConstexprString<20> const &single) {
        return HWY_DYNAMIC_DISPATCH(try_match_simd_impl_3_20)(data, ranges, single);
    }
    template<>
    std::optional<bool> try_match_simd(std::string_view data, std::array<CharRange, 3> const &ranges, datatypes::registry::util::ConstexprString<21> const &single) {
        return HWY_DYNAMIC_DISPATCH(try_match_simd_impl_3_21)(data, ranges, single);
    }
    template<>
    std::optional<bool> try_match_simd(std::string_view data, std::array<CharRange, 1> const &ranges, datatypes::registry::util::ConstexprString<1> const &single) {
        return HWY_DYNAMIC_DISPATCH(try_match_simd_impl_1_1)(data, ranges, single);
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
