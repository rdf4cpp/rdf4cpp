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

    TEST_CASE("correct data with prefix") {
        constexpr char const *triples = "@prefix ex: <http://www.example.org/> ."
                                        "ex:s1 ex:p1 ex:o1 .\n"
                                        "ex:s1 ex:p2 <http://www.example.org/o2> .\n"
                                        "ex:s2 ex:p3 \"test\" .\n";

        std::istringstream iss{triples};
        IStreamQuadIterator qit{iss};
        CHECK(qit != IStreamQuadIterator{});
        CHECK(qit->value().subject() == IRI{"http://www.example.org/s1"});
        CHECK(qit->value().predicate() == IRI{"http://www.example.org/p1"});
        CHECK(qit->value().object() == IRI{"http://www.example.org/o1"});

        ++qit;
        CHECK(qit != IStreamQuadIterator{});
        CHECK(qit->value().subject() == IRI{"http://www.example.org/s1"});
        CHECK(qit->value().predicate() == IRI{"http://www.example.org/p2"});
        CHECK(qit->value().object() == IRI{"http://www.example.org/o2"});

        ++qit;
        CHECK(qit != IStreamQuadIterator{});
        CHECK(qit->value().subject() == IRI{"http://www.example.org/s2"});
        CHECK(qit->value().predicate() == IRI{"http://www.example.org/p3"});
        CHECK(qit->value().object() == Literal{"test"});

        ++qit;
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

    TEST_CASE("continue after error turtle") {
        constexpr char const *triples = "@prefix rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#> .\n"
                                        "@prefix dc: <http://purl.org/dc/elements/1.1/> .\n"
                                        "@prefix ex: <http://example.org/stuff/1.0/> .\n"
                                        "\n"
                                        "<http://www.w3.org/TR/rdf-syntax-grammar>\n"
                                        "  dc:title \"RDF/XML Syntax Specification (Revised)\" ;\n"
                                        "  ex:editor [\n"
                                        "    ERRORHERE \"Dave Beckett\";\n"
                                        "    ex:homePage <http://purl.org/net/dajobe/>\n"
                                        "  ] .\n"
                                        "\n"
                                        "<http://www.w3.org/TR/rdf-syntax-grammar>\n"
                                        "  dc:title \"RDF/XML Syntax Specification (Revised)\" ;\n"
                                        "  ex:editor [\n"
                                        "    ex:fullname \"Dave Beckett\";\n"
                                        "  ] .";

        std::istringstream iss{triples};
        IStreamQuadIterator qit{iss, ParsingFlag::KeepBlankNodeIds};

        CHECK(qit != IStreamQuadIterator{});
        CHECK(qit->value() == Quad{IRI{"http://www.w3.org/TR/rdf-syntax-grammar"},
                                   IRI{"http://purl.org/dc/elements/1.1/title"},
                                   Literal::make<datatypes::xsd::String>("RDF/XML Syntax Specification (Revised)")});


        ++qit;
        CHECK(qit != IStreamQuadIterator{});
        CHECK(qit->value() == Quad{IRI{"http://www.w3.org/TR/rdf-syntax-grammar"},
                                   IRI{"http://example.org/stuff/1.0/editor"},
                                   BlankNode{"b1"}});

        // error here, rest of this spec gets dropped because parsing state is now messed up
        ++qit;
        CHECK(!qit->has_value());

        // second error is a result of parsing state being messed up
        ++qit;
        CHECK(!qit->has_value());

        // start of new spec
        ++qit;
        CHECK(qit != IStreamQuadIterator{});
        CHECK(qit->value() == Quad{IRI{"http://www.w3.org/TR/rdf-syntax-grammar"},
                                   IRI{"http://purl.org/dc/elements/1.1/title"},
                                   Literal::make<datatypes::xsd::String>("RDF/XML Syntax Specification (Revised)")});

        ++qit;
        CHECK(qit != IStreamQuadIterator{});
        CHECK(qit->value() == Quad{IRI{"http://www.w3.org/TR/rdf-syntax-grammar"},
                                   IRI{"http://example.org/stuff/1.0/editor"},
                                   BlankNode{"b2"}});

        ++qit;
        CHECK(qit != IStreamQuadIterator{});
        CHECK(qit->value() == Quad{BlankNode{"b2"},
                                   IRI{"http://example.org/stuff/1.0/fullname"},
                                   Literal::make<datatypes::xsd::String>("Dave Beckett")});

        ++qit;
        CHECK(qit == IStreamQuadIterator{});
    }

    TEST_CASE("invalid literal") {
        constexpr char const *triples = "<http://data.semanticweb.org/workshop/admire/2012/paper/12> <http://purl.org/dc/elements/1.1/subject> \"search\"^^<http://www.w3.org/2001/XMLSchema#int> .\n";

        std::istringstream iss{triples};
        IStreamQuadIterator qit{iss};

        CHECK(qit != IStreamQuadIterator{});
        CHECK(!qit->has_value());
        std::cerr << qit->error() << std::endl;

        ++qit;
        CHECK(qit == IStreamQuadIterator{});
    }

    TEST_CASE("unknown prefix") {
        constexpr char const *triples = "<http://data.semanticweb.org/workshop/admire/2012/paper/12> prefix:predicate \"search\"^^<http://www.w3.org/2001/XMLSchema#string> .\n";

        std::istringstream iss{triples};
        IStreamQuadIterator qit{iss};

        CHECK(qit != IStreamQuadIterator{});
        CHECK(!qit->has_value());
        std::cerr << qit->error() << std::endl;

        ++qit;
        CHECK(qit == IStreamQuadIterator{});
    }

    TEST_CASE("curie as literal type") {
        constexpr char const *triples = "@prefix xsd: <http://some-random-url.de#> .\n"
                                        "<http://data.semanticweb.org/workshop/admire/2012/paper/12> <http://purl.org/dc/elements/1.1/subject> \"search\"^^xsd:string .\n";

        std::istringstream iss{triples};
        IStreamQuadIterator qit{iss};

        CHECK(qit != IStreamQuadIterator{});
        CHECK(qit->has_value());
        std::cerr << qit->value() << std::endl;

        ++qit;
        CHECK(qit == IStreamQuadIterator{});
    }

    TEST_CASE("manually added prefix") {
        constexpr char const *triples = "<http://data.semanticweb.org/workshop/admire/2012/paper/12> prefix:predicate \"search\"^^<http://www.w3.org/2001/XMLSchema#string> .\n";

        std::istringstream iss{triples};
        IStreamQuadIterator::state_type state{
                .prefixes = { {"prefix", "https://hello.com#"} }};

        IStreamQuadIterator qit{iss, ParsingFlags::none(), &state};

        CHECK(qit != IStreamQuadIterator{});
        CHECK(qit->has_value());
        CHECK(qit->value() == Quad{IRI{"http://data.semanticweb.org/workshop/admire/2012/paper/12"},
                                   IRI{"https://hello.com#predicate"},
                                   Literal{"search"}});

        ++qit;
        CHECK(qit == IStreamQuadIterator{});
    }

    TEST_CASE("no prefix parsing") {
        constexpr char const *triples = "@base <http://invalid-url.org> .\n"
                                        "@prefix xsd: <http://some-random-url.de#> .\n"
                                        "<http://data.semanticweb.org/workshop/admire/2012/paper/12> <http://purl.org/dc/elements/1.1/subject> \"search\" .\n"
                                        "xsd:subject xsd:predicate \"aaaaa\" .\n";

        std::istringstream iss{triples};
        IStreamQuadIterator qit{iss, ParsingFlag::NoParsePrefix};

        CHECK(qit != IStreamQuadIterator{});
        CHECK(!qit->has_value());
        std::cout << qit->error() << std::endl;

        ++qit;
        CHECK(qit != IStreamQuadIterator{});
        CHECK(!qit->has_value());
        std::cout << qit->error() << std::endl;

        ++qit;
        CHECK(qit != IStreamQuadIterator{});
        CHECK(qit->has_value());
        CHECK(qit->value() == Quad{IRI{"http://data.semanticweb.org/workshop/admire/2012/paper/12"},
                                   IRI{"http://purl.org/dc/elements/1.1/subject"},
                                   Literal{"search"}});

        ++qit;
        CHECK(qit != IStreamQuadIterator{});
        CHECK(!qit->has_value());
        std::cout << qit->error() << std::endl;

        ++qit;
        CHECK(qit == IStreamQuadIterator{});
    }

    TEST_CASE("bnode management") {
        constexpr char const *triples = "<http://data.semanticweb.org/workshop/admire/2012/paper/12> <http://purl.org/dc/elements/1.1/subject> _:b1 .\n"
                                        "<http://data.semanticweb.org/workshop/admire/2012/paper/13> <http://purl.org/dc/elements/1.1/subject> _:b1 .\n"
                                        "_:b2 <http://purl.org/dc/elements/1.1/subject> \"Some Subject\" .\n";

        SUBCASE("bnodes") {
            IStreamQuadIterator::state_type state{};
            std::istringstream iss{triples};
            IStreamQuadIterator qit{iss, ParsingFlags::none(), &state};

            CHECK(qit != IStreamQuadIterator{});
            CHECK(qit->has_value());
            CHECK(qit->value().subject() == IRI{"http://data.semanticweb.org/workshop/admire/2012/paper/12"});
            CHECK(qit->value().predicate() == IRI{"http://purl.org/dc/elements/1.1/subject"});

            auto b1_1 = qit->value().object();
            CHECK(b1_1.is_blank_node());
            std::cout << qit->value().object() << std::endl;

            ++qit;
            CHECK(qit != IStreamQuadIterator{});
            CHECK(qit->value().subject() == IRI{"http://data.semanticweb.org/workshop/admire/2012/paper/13"});
            CHECK(qit->value().predicate() == IRI{"http://purl.org/dc/elements/1.1/subject"});

            auto b1_2 = qit->value().object();
            CHECK(b1_1.is_blank_node());
            std::cout << qit->value().object() << std::endl;
            CHECK(b1_1 == b1_2);

            ++qit;
            CHECK(qit != IStreamQuadIterator{});
            auto b2_1 = qit->value().subject();
            CHECK(b2_1.is_blank_node());
            CHECK(b2_1 != b1_1);
            std::cout << b2_1 << std::endl;
            CHECK(qit->value().predicate() == IRI{"http://purl.org/dc/elements/1.1/subject"});
            CHECK(qit->value().object() == Literal{"Some Subject"});

            ++qit;
            CHECK(qit == IStreamQuadIterator{});
        }

        SUBCASE("skolem iris") {
            IStreamQuadIterator::state_type state{ .skolem_iri_prefix = "http://skolem-iris.org#" };
            std::istringstream iss{triples};
            IStreamQuadIterator qit{iss, ParsingFlags::none(), &state};

            CHECK(qit != IStreamQuadIterator{});
            CHECK(qit->has_value());
            CHECK(qit->value().subject() == IRI{"http://data.semanticweb.org/workshop/admire/2012/paper/12"});
            CHECK(qit->value().predicate() == IRI{"http://purl.org/dc/elements/1.1/subject"});

            auto b1_1 = qit->value().object();
            CHECK(b1_1.is_iri());
            std::cout << qit->value().object() << std::endl;

            ++qit;
            CHECK(qit != IStreamQuadIterator{});
            CHECK(qit->value().subject() == IRI{"http://data.semanticweb.org/workshop/admire/2012/paper/13"});
            CHECK(qit->value().predicate() == IRI{"http://purl.org/dc/elements/1.1/subject"});

            auto b1_2 = qit->value().object();
            CHECK(b1_1.is_iri());
            std::cout << qit->value().object() << std::endl;
            CHECK(b1_1 == b1_2);

            ++qit;
            CHECK(qit != IStreamQuadIterator{});
            auto b2_1 = qit->value().subject();
            CHECK(b2_1.is_iri());
            CHECK(b2_1 != b1_1);
            std::cout << b2_1 << std::endl;
            CHECK(qit->value().predicate() == IRI{"http://purl.org/dc/elements/1.1/subject"});
            CHECK(qit->value().object() == Literal{"Some Subject"});

            ++qit;
            CHECK(qit == IStreamQuadIterator{});
        }
    }
}
