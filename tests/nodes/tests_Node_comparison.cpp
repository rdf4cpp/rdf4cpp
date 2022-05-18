#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Ordering") {
    IRI iri1 = IRI{"http://www.example.org/test2"};
    IRI iri2 = IRI{"http://www.example.org/test1"};
    IRI iri3 = IRI{"http://www.example.org/oest"};
    Literal lit1 = Literal{"testlit1"};
    Literal lit2 = Literal{"testlit2"};
    CHECK(iri2 < iri1);
    CHECK(iri3 < iri1);
    CHECK(iri3 < iri2);
    CHECK(iri1 < lit1);
    CHECK(lit1 < lit2);
}