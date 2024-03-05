#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include "rdf4cpp.hpp"
#include <rdf4cpp/parser/RDFFileParser.hpp>

TEST_SUITE("RDFFileParser") {
    TEST_CASE("not existing file") {
        rdf4cpp::parser::RDFFileParser parse{"shouldnotexist.ttl"};
        try {
            for (const auto &v : parse) {  // more like check that this compiles ;)
                FAIL("Unreachable");
                CHECK(v.has_value() == false);
            }
            FAIL("Unreachable");
        } catch (std::system_error const &e) {
            // expecting exception
        }
    }
    TEST_CASE("existing file") {
        int count = 0;
        for (const auto &v : rdf4cpp::parser::RDFFileParser{"./tests_RDFFileParser_simple.ttl"}) {
            if (v.has_value()) {
                switch (count) {
                    case 0:
                        CHECK(v.value().subject() == rdf4cpp::IRI{"http://data.semanticweb.org/workshop/admire/2012/paper/12"});
                        CHECK(v.value().predicate() == rdf4cpp::IRI{"http://purl.org/dc/elements/1.1/subject"});
                        CHECK(v.value().object() == rdf4cpp::Literal::make_simple("search"));
                        break;
                    case 1:
                        CHECK(v.value().subject() == rdf4cpp::IRI{"http://data.semanticweb.org/workshop/admire/2012/paper/12"});
                        CHECK(v.value().predicate() == rdf4cpp::IRI{"http://purl.org/ontology/bibo/authorList"});
                        CHECK(v.value().object() == rdf4cpp::IRI{"http://data.semanticweb.org/workshop/admire/2012/paper/12/authorlist"});
                        break;
                    case 2:
                        CHECK(v.value().subject() == rdf4cpp::IRI{"http://data.semanticweb.org/workshop/admire/2012/paper/12"});
                        CHECK(v.value().predicate() == rdf4cpp::IRI{"http://purl.org/dc/elements/1.1/subject"});
                        CHECK(v.value().object() == rdf4cpp::Literal::make_simple("web applications"));
                        break;
                }
                ++count;
            } else if (count == 3) {
                ++count;
            } else {
                CHECK_MESSAGE(false, v.error());
            }
        }
        CHECK(count == 4);
    }
    TEST_CASE("move iterator") {
        rdf4cpp::parser::RDFFileParser pars{"./tests_RDFFileParser_simple.ttl"};
        auto it = pars.begin();
        ++it;
        CHECK(it != pars.end());
    }
    // only testing basic iterator functionality here, see tests for IStreamQuadIterator for more parsing tests
}
