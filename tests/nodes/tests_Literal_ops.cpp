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

TEST_CASE("Literal - binary add") {
    SUBCASE("float add") {
        auto const lhs = Literal::make<datatypes::xsd::Float>(1.f);
        auto const rhs = Literal::make<datatypes::xsd::Float>(2.f);

        auto const expected = Literal::make<datatypes::xsd::Float>(3.f);
        CHECK(lhs + rhs == expected);
    }

    SUBCASE("boolean not add") {
        auto const lhs = Literal::make<datatypes::xsd::Boolean>(true);
        auto const rhs = Literal::make<datatypes::xsd::Boolean>(false);

        CHECK_THROWS_WITH_AS(lhs + rhs, "datatype not numeric", std::runtime_error);
    }

    SUBCASE("add literal type mismatch") {
        auto const lhs = Literal::make<datatypes::xsd::Float>(1.f);
        auto const rhs = Literal::make<datatypes::xsd::Boolean>(false);

        CHECK_THROWS_WITH_AS(lhs + rhs, "datatype mismatch", std::runtime_error);
    }
}
