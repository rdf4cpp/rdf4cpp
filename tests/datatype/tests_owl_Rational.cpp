#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_SUITE("owl:rational") {
    TEST_CASE("capabilities") {
        static_assert(datatypes::LiteralDatatype<datatypes::owl::Rational>);
        static_assert(datatypes::NumericLiteralDatatype<datatypes::owl::Rational>);
        static_assert(datatypes::LogicalLiteralDatatype<datatypes::owl::Rational>);
        static_assert(!datatypes::PromotableLiteralDatatype<datatypes::owl::Rational>);
        static_assert(!datatypes::SubtypedLiteralDatatype<datatypes::owl::Rational>);
    }

    TEST_CASE("use") {
        using datatypes::owl::Rational;
        using cpp_type = Rational::cpp_type;
        using datatypes::registry::owl_rational;

        SUBCASE("to string") {
            cpp_type const value1{1, 2};
            auto const lit1 = Literal::make<Rational>(value1);
            CHECK(lit1.value<Rational>() == value1);
            CHECK(lit1.lexical_form() == "1/2");

            cpp_type const value2{-1, 10};
            auto const lit2 = Literal::make<Rational>(value2);
            CHECK(lit2.value<Rational>() == value2);
            CHECK(lit2.lexical_form() == "-1/10");

            cpp_type const non_canonical{-10, -20};
            auto const lit3 = Literal::make<Rational>(non_canonical);
            CHECK(lit3.lexical_form() == "1/2");
        }

        SUBCASE("from string") {
            Literal const lit1{"1/10", IRI{owl_rational}};
            Literal const lit2 = Literal::make<Rational>(cpp_type{1, 10});
            Literal const lit3{"-1/10", IRI{owl_rational}};
            Literal const lit4 = Literal::make<Rational>(cpp_type{-1, 10});

            CHECK(lit1 == lit2);
            CHECK(lit3 == lit4);
            CHECK(lit1 == -lit3);

            CHECK_THROWS(Literal{" 2/ 5", IRI{owl_rational}});
            CHECK_THROWS(Literal{"+10/5", IRI{owl_rational}});
            CHECK_THROWS(Literal{"asd", IRI{owl_rational}});
            CHECK_THROWS(Literal{"5/-20", IRI{owl_rational}});
            CHECK_THROWS(Literal{"0x123/99", IRI{owl_rational}});
        }
    }

    TEST_CASE("abs") {
        using datatypes::owl::Rational;
        using cpp_type = Rational::cpp_type;

        CHECK(Literal::make<Rational>(cpp_type{-10, 100}).abs() == Literal::make<Rational>(cpp_type{10, 100}));
        CHECK(Literal::make<Rational>(cpp_type{99, -42}).abs() == Literal::make<Rational>(cpp_type{99, 42}));
        CHECK(Literal::make<Rational>(cpp_type{2, 3}).abs() == Literal::make<Rational>(cpp_type{2, 3}));
    }

    TEST_CASE("round") {
        using datatypes::owl::Rational;
        using cpp_type = Rational::cpp_type;

        CHECK(Literal::make<Rational>(cpp_type{2, 5}).round() == Literal::make<Rational>(cpp_type{0, 1}));
        CHECK(Literal::make<Rational>(cpp_type{1, 2}).round() == Literal::make<Rational>(cpp_type{1, 1}));
        CHECK(Literal::make<Rational>(cpp_type{9, 10}).round() == Literal::make<Rational>(cpp_type{1, 1}));
        CHECK(Literal::make<Rational>(cpp_type{11, 10}).round() == Literal::make<Rational>(cpp_type{1, 1}));
        CHECK(Literal::make<Rational>(cpp_type{42, 10}).round() == Literal::make<Rational>(cpp_type{4, 1}));
    }

    TEST_CASE("floor") {
        using datatypes::owl::Rational;
        using cpp_type = Rational::cpp_type;

        CHECK(Literal::make<Rational>(cpp_type{2, 5}).floor() == Literal::make<Rational>(cpp_type{0, 1}));
        CHECK(Literal::make<Rational>(cpp_type{1, 2}).floor() == Literal::make<Rational>(cpp_type{0, 1}));
        CHECK(Literal::make<Rational>(cpp_type{9, 10}).floor() == Literal::make<Rational>(cpp_type{0, 1}));
        CHECK(Literal::make<Rational>(cpp_type{11, 10}).floor() == Literal::make<Rational>(cpp_type{1, 1}));
        CHECK(Literal::make<Rational>(cpp_type{42, 10}).floor() == Literal::make<Rational>(cpp_type{4, 1}));
    };

    TEST_CASE("ceil") {
        using datatypes::owl::Rational;
        using cpp_type = Rational::cpp_type;

        CHECK(Literal::make<Rational>(cpp_type{2, 5}).ceil() == Literal::make<Rational>(cpp_type{1, 1}));
        CHECK(Literal::make<Rational>(cpp_type{1, 2}).ceil() == Literal::make<Rational>(cpp_type{1, 1}));
        CHECK(Literal::make<Rational>(cpp_type{9, 10}).ceil() == Literal::make<Rational>(cpp_type{1, 1}));
        CHECK(Literal::make<Rational>(cpp_type{11, 10}).ceil() == Literal::make<Rational>(cpp_type{2, 1}));
        CHECK(Literal::make<Rational>(cpp_type{42, 10}).ceil() == Literal::make<Rational>(cpp_type{5, 1}));
    }
}
