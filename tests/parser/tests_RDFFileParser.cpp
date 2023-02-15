#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>
#include <rdf4cpp/rdf/parser/RDFFileParser.hpp>

TEST_SUITE("RDFFileParser") {
    TEST_CASE("not existing file") {
        rdf4cpp::rdf::parser::RDFFileParser parse{"shouldnotexist.ttl"};
        for (const auto &v : parse) {  // more like check that this compiles ;)
            CHECK(v.has_value() == false);
        }
        CHECK((parse.begin() == parse.end()));
    }
    TEST_CASE("existing file") {
        int count = 0;
        for (const auto &v : rdf4cpp::rdf::parser::RDFFileParser{"./tests_RDFFileParser_simple.ttl"}) {
            if (v.has_value()) {
                ++count;
                CHECK(v.value().subject() == rdf4cpp::rdf::IRI{"http://data.semanticweb.org/workshop/admire/2012/paper/12"});
                CHECK(v.value().predicate() == rdf4cpp::rdf::IRI{"http://purl.org/dc/elements/1.1/subject"});
                CHECK(v.value().object() == rdf4cpp::rdf::Literal::make_simple("search"));
            } else {
                CHECK_MESSAGE(false, v.error());
            }
        }
        CHECK(count == 1);
    }
    // only testing basic iterator functionality here, see tests for IStreamQuadIterator for more parsing tests
}
