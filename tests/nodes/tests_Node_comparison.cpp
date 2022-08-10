#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_SUITE("comparisions") {
    using namespace datatypes::xsd;

    TEST_CASE("Ordering") {
        IRI iri1 = IRI{"http://www.example.org/test2"};
        IRI iri2 = IRI{"http://www.example.org/test1"};
        IRI iri3 = IRI{"http://www.example.org/oest"};
        Literal lit1 = Literal::make<String>("testlit1");
        Literal lit2 = Literal::make<String>("testlit2");
        CHECK(iri2 < iri1);
        CHECK(iri3 < iri1);
        CHECK(iri3 < iri2);
        CHECK(iri1 < lit1);
        CHECK(lit1 < lit2);
    }

    TEST_CASE("filter compare tests") {
        CHECK(Literal::make<String>("hello") <=> Literal::make<Int>(5) == std::partial_ordering::unordered);
        CHECK(Literal::make<Int>(1) < Literal::make<Integer>(10));
        CHECK(Literal::make<Float>(1.2f) > Literal::make<Integer>(0));
        CHECK(Literal::make<Float>(1.f) == Literal::make<Decimal>(1.0));
    }

    TEST_CASE("order by compare tests") {
        CHECK(Literal::make<String>("hello").compare_with_extensions(Literal::make<Int>(1)) == std::partial_ordering::unordered);

        SUBCASE("test ordering extensions") {
            // decimal < float < int < integer

            CHECK(Literal::make<Decimal>(1.0).compare_with_extensions(Literal::make<Float>(1)) == std::partial_ordering::less);
            CHECK(Literal::make<Decimal>(1.0).compare_with_extensions(Literal::make<Int>(1)) == std::partial_ordering::less);
            CHECK(Literal::make<Decimal>(1.0).compare_with_extensions(Literal::make<Integer>(1)) == std::partial_ordering::less);

            CHECK(Literal::make<Float>(1.f).compare_with_extensions(Literal::make<Int>(1)) == std::partial_ordering::less);
            CHECK(Literal::make<Float>(1.f).compare_with_extensions(Literal::make<Integer>(1)) == std::partial_ordering::less);

            CHECK(Literal::make<Int>(1).compare_with_extensions(Literal::make<Integer>(1)) == std::partial_ordering::less);
        }

        SUBCASE("test ordering extensions ignored when not equal") {
            CHECK(Literal::make<Float>(2.f).compare_with_extensions(Literal::make<Integer>(1)) == std::partial_ordering::greater);
        }
    }
}
