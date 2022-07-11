#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Literal - logical ops") {
    SUBCASE("Literal - logical ops - results") {
        SUBCASE("Literal - logical ops - bool results - and") {
            {
                auto const lhs = Literal::make<datatypes::xsd::Boolean>(true);
                auto const rhs = Literal::make<datatypes::xsd::Boolean>(false);

                auto const res = lhs && rhs;
                auto const res2 = rhs && lhs;
                CHECK(res == Literal::make<datatypes::xsd::Boolean>(false));
                CHECK(res2 == Literal::make<datatypes::xsd::Boolean>(false));
            }
            {
                auto const lhs = Literal::make<datatypes::xsd::Boolean>(true);
                auto const rhs = Literal::make<datatypes::xsd::Boolean>(true);

                auto const res = lhs && rhs;
                auto const res2 = rhs && lhs;
                CHECK(res == Literal::make<datatypes::xsd::Boolean>(true));
                CHECK(res2 == Literal::make<datatypes::xsd::Boolean>(true));
            }
            {
                auto const lhs = Literal::make<datatypes::xsd::Boolean>(false);
                auto const rhs = Literal::make<datatypes::xsd::Boolean>(false);

                auto const res = lhs && rhs;
                auto const res2 = rhs && lhs;
                CHECK(res == Literal::make<datatypes::xsd::Boolean>(false));
                CHECK(res2 == Literal::make<datatypes::xsd::Boolean>(false));
            }
        }

        SUBCASE("Literal - logical ops - bool results - or") {
            {
                auto const lhs = Literal::make<datatypes::xsd::Boolean>(true);
                auto const rhs = Literal::make<datatypes::xsd::Boolean>(false);

                auto const res = lhs || rhs;
                auto const res2 = rhs || lhs;
                CHECK(res == Literal::make<datatypes::xsd::Boolean>(true));
                CHECK(res2 == Literal::make<datatypes::xsd::Boolean>(true));
            }
            {
                auto const lhs = Literal::make<datatypes::xsd::Boolean>(true);
                auto const rhs = Literal::make<datatypes::xsd::Boolean>(true);

                auto const res = lhs || rhs;
                auto const res2 = rhs || lhs;
                CHECK(res == Literal::make<datatypes::xsd::Boolean>(true));
                CHECK(res2 == Literal::make<datatypes::xsd::Boolean>(true));
            }
            {
                auto const lhs = Literal::make<datatypes::xsd::Boolean>(false);
                auto const rhs = Literal::make<datatypes::xsd::Boolean>(false);

                auto const res = lhs || rhs;
                auto const res2 = rhs || lhs;
                CHECK(res == Literal::make<datatypes::xsd::Boolean>(false));
                CHECK(res2 == Literal::make<datatypes::xsd::Boolean>(false));
            }
        }

        SUBCASE("Literal - logical ops - bool results - not") {
            {
                auto const op = Literal::make<datatypes::xsd::Boolean>(true);
                CHECK(!op == Literal::make<datatypes::xsd::Boolean>(false));
            }
            {
                auto const op = Literal::make<datatypes::xsd::Boolean>(false);
                CHECK(!op == Literal::make<datatypes::xsd::Boolean>(true));
            }
        }
    }
}

#define GENERATE_BINOP_TESTCASE(lhs_type, lhs, op, rhs_type, rhs, expected_type, expected) \
    SUBCASE("lhs_type op rhs_type") {                                                      \
        auto const lhs_lit = Literal::make<datatypes::xsd::lhs_type>(lhs);                 \
        auto const rhs_lit = Literal::make<datatypes::xsd::rhs_type>(rhs);                 \
                                                                                           \
        auto const expected_lit = Literal::make<datatypes::xsd::expected_type>(expected);  \
        CHECK(lhs_lit op rhs_lit == expected_lit);                                         \
    }

#define GENERATE_UNOP_TESTCASE(type, value, op, expected)                         \
    SUBCASE("op type") {                                                          \
        auto const value_lit = Literal::make<datatypes::xsd::type>(value);        \
                                                                                  \
        auto const expected_lit = Literal::make<datatypes::xsd::type>(expected);  \
        CHECK(op value_lit == expected_lit);                                      \
    }

TEST_CASE("Literal - numeric ops") {
    // simple promotion test cases
    GENERATE_BINOP_TESTCASE(Float, 42.f, +, Decimal, 120.0, Float, 162.f);
    GENERATE_BINOP_TESTCASE(Decimal, 2.f, *, Float, 120.0, Float, 240.f);
    GENERATE_BINOP_TESTCASE(Integer, 100, -, Float, 1.f, Float, 99.f);
    GENERATE_BINOP_TESTCASE(Float, 100.f, /, Integer, 2, Float, 50.f);

    // simple subtype substitution test cases
    GENERATE_BINOP_TESTCASE(Integer, 6, +, Int, 7, Integer, 13);
    GENERATE_BINOP_TESTCASE(Int, 8, *, Integer, 9, Integer, 72);

    // operator assignment test cases
    GENERATE_BINOP_TESTCASE(Integer, 1, +, Integer , 2, Integer, 3);
    GENERATE_BINOP_TESTCASE(Integer, 12, -, Integer, 1, Integer, 11);
    GENERATE_BINOP_TESTCASE(Integer, 3, *, Integer, 6, Integer, 18);
    GENERATE_BINOP_TESTCASE(Integer, 12, /, Integer, 4, Integer, 3); // TODO: use idiv or div? currently using idiv
    GENERATE_UNOP_TESTCASE(Integer, 1, +, 1);
    GENERATE_UNOP_TESTCASE(Integer, 1, -, -1);

    GENERATE_BINOP_TESTCASE(Int, 1, +, Integer, 3, Integer, 4);
    GENERATE_BINOP_TESTCASE(Int, 1, +, Decimal, 2, Decimal, 3);

    SUBCASE("boolean not add") {
        auto const lhs = Literal::make<datatypes::xsd::Boolean>(true);
        auto const rhs = Literal::make<datatypes::xsd::Boolean>(false);

        CHECK_THROWS_WITH_AS(lhs + rhs, "result datatype not numeric", std::runtime_error);
    }

    SUBCASE("add literal type mismatch") {
        auto const lhs = Literal::make<datatypes::xsd::Float>(1.f);
        auto const rhs = Literal::make<datatypes::xsd::Boolean>(false);

        CHECK_THROWS_WITH_AS(lhs + rhs, "datatype mismatch and not in same promotion hierarchy", std::runtime_error);
    }
}


// create fake hierarchy
namespace rdf4cpp::rdf::datatypes::registry {

constexpr static registry::ConstexprString Z{"Z"};

template<>
struct DatatypeMapping<Z> {
    using cpp_datatype = double;
};

template<>
inline capabilities::Default<Z>::cpp_type capabilities::Default<Z>::from_string(std::string_view s) {
    double value;
    auto const parse_res = std::from_chars(s.data(), s.data() + s.size(), value);

    if (parse_res.ptr != s.data() + s.size()) {
        throw std::runtime_error("XSD Parsing Error");
    } else {
        return value;
    }
}

constexpr static registry::ConstexprString Y{"Y"};

template<>
struct DatatypeMapping<Y> {
    using cpp_datatype = float;
};

template<>
struct DatatypeSupertypeMapping<Y> {
    static constexpr ConstexprString supertype_identifier = Z;
};

template<>
inline capabilities::Default<Y>::cpp_type capabilities::Default<Y>::from_string(std::string_view s) {
    cpp_type value;
    auto const parse_res = std::from_chars(s.data(), s.data() + s.size(), value);

    if (parse_res.ptr != s.data() + s.size()) {
        throw std::runtime_error("XSD Parsing Error");
    } else {
        return value;
    }
}

constexpr static registry::ConstexprString A{"A"};

template<>
struct DatatypeMapping<A> {
    using cpp_datatype = int64_t;
};

template<>
struct DatatypePromotionMapping<A> {
    static constexpr ConstexprString promoted_identifier = Z;
};

template<>
inline capabilities::Default<A>::cpp_type capabilities::Default<A>::from_string(std::string_view s) {
    cpp_type value;
    auto const parse_res = std::from_chars(s.data(), s.data() + s.size(), value);

    if (parse_res.ptr != s.data() + s.size()) {
        throw std::runtime_error("XSD Parsing Error");
    } else {
        return value;
    }
}


constexpr static registry::ConstexprString B{"B"};

template<>
struct DatatypeMapping<B> {
    using cpp_datatype = int32_t;
};

template<>
struct DatatypeSupertypeMapping<B> {
    static constexpr ConstexprString supertype_identifier = A;
};

template<>
inline capabilities::Default<B>::cpp_type capabilities::Default<B>::from_string(std::string_view s) {
    cpp_type value;
    auto const parse_res = std::from_chars(s.data(), s.data() + s.size(), value);

    if (parse_res.ptr != s.data() + s.size()) {
        throw std::runtime_error("XSD Parsing Error");
    } else {
        return value;
    }
}

constexpr static registry::ConstexprString B2{"B2"};

template<>
struct DatatypeMapping<B2> {
    using cpp_datatype = uint32_t;
};

template<>
struct DatatypeSupertypeMapping<B2> {
    static constexpr ConstexprString supertype_identifier = A;
};

template<>
inline capabilities::Default<B2>::cpp_type capabilities::Default<B2>::from_string(std::string_view s) {
    cpp_type value;
    auto const parse_res = std::from_chars(s.data(), s.data() + s.size(), value);

    if (parse_res.ptr != s.data() + s.size()) {
        throw std::runtime_error("XSD Parsing Error");
    } else {
        return value;
    }
}


constexpr static registry::ConstexprString C{"C"};

template<>
struct DatatypeMapping<C> {
    using cpp_datatype = int16_t;
};

template<>
struct DatatypeSupertypeMapping<C> {
    static constexpr ConstexprString supertype_identifier = B;
};

template<>
inline capabilities::Default<C>::cpp_type capabilities::Default<C>::from_string(std::string_view s) {
    cpp_type value;
    auto const parse_res = std::from_chars(s.data(), s.data() + s.size(), value);

    if (parse_res.ptr != s.data() + s.size()) {
        throw std::runtime_error("XSD Parsing Error");
    } else {
        return value;
    }
}
}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {

/**
 *       A   -->   Z
 *     |  |        |
 *     B  B2 -->   Y
 *     |
 *     C
 */
using A = registry::LiteralDatatypeImpl<registry::A, registry::capabilities::Numeric, registry::capabilities::Promotable>;
using B = registry::LiteralDatatypeImpl<registry::B, registry::capabilities::Numeric, registry::capabilities::Subtype>;
using B2 = registry::LiteralDatatypeImpl<registry::B, registry::capabilities::Numeric, registry::capabilities::Subtype>;
using C = registry::LiteralDatatypeImpl<registry::C, registry::capabilities::Numeric, registry::capabilities::Subtype>;

using Z = registry::LiteralDatatypeImpl<registry::Z, registry::capabilities::Numeric>;
using Y = registry::LiteralDatatypeImpl<registry::Y, registry::capabilities::Numeric, registry::capabilities::Subtype>;
}  // namespace rdf4cpp::rdf::datatypes::xsd


#define GENERATE_HIERARCHY_TEST(lhs, rhs, expected) \
    GENERATE_BINOP_TESTCASE(lhs, 1, +, rhs, 1, expected, 2)

TEST_CASE("complex hierarchy") {
    GENERATE_HIERARCHY_TEST(A, B, A);
    GENERATE_HIERARCHY_TEST(A, B2, A);
    GENERATE_HIERARCHY_TEST(A, C, A);
    GENERATE_HIERARCHY_TEST(A, Z, Z);
    GENERATE_HIERARCHY_TEST(A, Y, Z);
    GENERATE_HIERARCHY_TEST(B, B2, A);
    GENERATE_HIERARCHY_TEST(B, C, B);
    GENERATE_HIERARCHY_TEST(B, Z, Z);
    GENERATE_HIERARCHY_TEST(B, Y, Z);
    GENERATE_HIERARCHY_TEST(B2, C, A);
    GENERATE_HIERARCHY_TEST(B2, Z, Z);
    GENERATE_HIERARCHY_TEST(B2, Y, Y);
    GENERATE_HIERARCHY_TEST(C, Z, Z);
    GENERATE_HIERARCHY_TEST(C, Y, Z);
    GENERATE_HIERARCHY_TEST(Z, Y, Z);
}