#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>
#include <rdf4cpp/rdf/parser/RDFFileParser.hpp>

TEST_SUITE("RDFFileParser") {
    TEST_CASE("not existing file") {
        rdf4cpp::rdf::parser::RDFFileParser parse{"shouldnotexist.ttl"};
        for (const auto& v : parse) { // more like check that this compiles ;)
            CHECK(v.has_value() == false);
        }
        CHECK((parse.begin() == parse.end()));
    }
    TEST_CASE("existing file") {
        // todo
    }
    // not checking if the parsing is correct, see tests for IStreamQuadIterator
}
