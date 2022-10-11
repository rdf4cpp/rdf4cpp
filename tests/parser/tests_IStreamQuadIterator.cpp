#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

#include <ranges>
#include <iostream>

using namespace rdf4cpp::rdf;
using namespace rdf4cpp::rdf::parser;

TEST_SUITE("IStreamQuadIterator") {

    TEST_CASE("correct data") {
        constexpr char const *triples = "<http://data.semanticweb.org/workshop/admire/2012/paper/12> <http://purl.org/dc/elements/1.1/subject> \"search\" .\n"
                                        "<http://data.semanticweb.org/workshop/admire/2012/paper/12> <http://purl.org/ontology/bibo/authorList> <http://data.semanticweb.org/workshop/admire/2012/paper/12/authorlist> .\n"
                                        "<http://data.semanticweb.org/workshop/admire/2012/paper/12> <http://purl.org/dc/elements/1.1/subject> \"web applications\" .\n"
                                        "<http://data.semanticweb.org/workshop/admire/2012/paper/12> <http://xmlns.com/foaf/0.1/maker> <http://data.semanticweb.org/person/ichiro-fujinaga> .\n";

        std::istringstream iss{triples};

        size_t n = 0;
        for (auto qit = IStreamQuadIterator{iss}; qit != IStreamQuadIterator{}; ++qit) {
            CHECK(qit->has_value());
            n += 1;
        }

        CHECK(n == 4);
    }

    TEST_CASE("stop after first error") {
        constexpr char const *triples = "<http://data.semanticweb.org/workshop/admire/2012/paper/12> <http://purl.org/dc/elements/1.1/subject> \"search\" .\n"
                                        "<http://data.semanticweb.org/workshop/admire/2012/paper/12> <http://purl.org/ontology/bibo/authorList> <http://data.semanticweb.org/workshop/admire/2012/paper/12/authorlist> .\n"
                                        "<http://data.semanticweb.org/workshop/admire/2012/paper/12> http://purl.org/dc/elements/1.1/subject> \"web applications\" .\n"
                                        "<http://data.semanticweb.org/workshop/admire/2012/paper/12> <http://xmlns.com/foaf/0.1/maker> <http://data.semanticweb.org/person/ichiro-fujinaga> .\n";

        std::istringstream iss{triples};

        IStreamQuadIterator qit{iss, ParsingFlag::Strict | ParsingFlag::StopOnFirstError};
        for (; qit != IStreamQuadIterator{}; ++qit) {
            CHECK(qit->has_value());
            std::cout << **qit << std::endl;
        }

        CHECK(qit == IStreamQuadIterator{});
    }

    TEST_CASE("continue after error") {
        constexpr char const *triples = "<http://data.semanticweb.org/workshop/admire/2012/paper/12> <http://purl.org/dc/elements/1.1/subject> \"search\"^^<http://www.w3.org/2001/XMLSchema#string> .\n"
                                        "<http://data.semanticweb.org/workshop/admire/2012/paper/12> <http://purl.org/ontology/bibo/authorList> <http://data.semanticweb.org/workshop/admire/2012/paper/12/authorlist> .\n"
                                        "<http://data.semanticweb.org/workshop/admire/2012/paper/12> http://purl.org/dc/elements/1.1/subject> \"web applications\" .\n"
                                        "<http://data.semanticweb.org/workshop/admire/2012/paper/12> <http://xmlns.com/foaf/0.1/maker> <http://data.semanticweb.org/person/ichiro-fujinaga> .\n";

        SUBCASE("strict") {
            std::istringstream iss{triples};
            IStreamQuadIterator qit{iss, ParsingFlag::Strict};

            CHECK(qit != IStreamQuadIterator{});
            CHECK(qit->value().subject() == IRI{"http://data.semanticweb.org/workshop/admire/2012/paper/12"});
            CHECK(qit->value().predicate() == IRI{"http://purl.org/dc/elements/1.1/subject"});
            CHECK(qit->value().object() == Literal{"search"});

            ++qit;
            CHECK(qit != IStreamQuadIterator{});
            CHECK(qit->value().subject() == IRI{"http://data.semanticweb.org/workshop/admire/2012/paper/12"});
            CHECK(qit->value().predicate() == IRI{"http://purl.org/ontology/bibo/authorList"});
            CHECK(qit->value().object() == IRI{"http://data.semanticweb.org/workshop/admire/2012/paper/12/authorlist"});

            ++qit;
            CHECK(qit != IStreamQuadIterator{});
            CHECK(!qit->has_value());
            std::cerr << qit->error() << std::endl;

            ++qit;
            CHECK(qit != IStreamQuadIterator{});
            CHECK(qit->value().subject() == IRI{"http://data.semanticweb.org/workshop/admire/2012/paper/12"});
            CHECK(qit->value().predicate() == IRI{"http://xmlns.com/foaf/0.1/maker"});
            CHECK(qit->value().object() == IRI{"http://data.semanticweb.org/person/ichiro-fujinaga"});

            ++qit;
            CHECK(qit == IStreamQuadIterator{});
        }

        SUBCASE("non-strict") {
            std::istringstream iss{triples};
            IStreamQuadIterator qit{iss};

            CHECK(qit != IStreamQuadIterator{});
            CHECK(qit->value().subject() == IRI{"http://data.semanticweb.org/workshop/admire/2012/paper/12"});
            CHECK(qit->value().predicate() == IRI{"http://purl.org/dc/elements/1.1/subject"});
            CHECK(qit->value().object() == Literal{"search"});

            ++qit;
            CHECK(qit != IStreamQuadIterator{});
            CHECK(qit->value().subject() == IRI{"http://data.semanticweb.org/workshop/admire/2012/paper/12"});
            CHECK(qit->value().predicate() == IRI{"http://purl.org/ontology/bibo/authorList"});
            CHECK(qit->value().object() == IRI{"http://data.semanticweb.org/workshop/admire/2012/paper/12/authorlist"});

            ++qit;
            CHECK(qit != IStreamQuadIterator{});
            CHECK(!qit->has_value());
            std::cerr << qit->error() << std::endl;

            ++qit;
            CHECK(qit != IStreamQuadIterator{});
            CHECK(qit->value().subject() == IRI{"http://data.semanticweb.org/workshop/admire/2012/paper/12"});
            CHECK(qit->value().predicate() == IRI{"http://xmlns.com/foaf/0.1/maker"});
            CHECK(qit->value().object() == IRI{"http://data.semanticweb.org/person/ichiro-fujinaga"});

            ++qit;
            CHECK(qit == IStreamQuadIterator{});
        }
    }
}
