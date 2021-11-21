//
// Created by kaimal on 18.11.21.
//
#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Literal - Check for only lexical form") {

    auto iri = IRI{"http://www.w3.org/2001/XMLSchema#string"};
    auto lit1 = Literal{"Bugs Bunny"};

    CHECK(not lit1.is_blank_node());
    CHECK(lit1.is_literal());
    CHECK(not lit1.is_variable());
    CHECK(not lit1.is_iri());
    CHECK(lit1.lexical_form() == "Bugs Bunny");
    CHECK(lit1.datatype() == iri);
    CHECK(lit1.language_tag() == "");
    CHECK(std::string(lit1) == "\"Bugs Bunny\"^^<http://www.w3.org/2001/XMLSchema#string>");
}

TEST_CASE("Literal - Check for lexical form with IRI") {

    SUBCASE("string datatype"){
        auto iri = IRI{"http://www.w3.org/2001/XMLSchema#string"};
        auto lit1 = Literal{"Bugs Bunny", iri};

        CHECK(not lit1.is_blank_node());
        CHECK(lit1.is_literal());
        CHECK(not lit1.is_variable());
        CHECK(not lit1.is_iri());
        CHECK(lit1.lexical_form() == "Bugs Bunny");
        CHECK(lit1.datatype() == iri);
        CHECK(lit1.language_tag() == "");
        CHECK(std::string(lit1) == "\"Bugs Bunny\"^^<http://www.w3.org/2001/XMLSchema#string>");
    }
    SUBCASE("int datatype"){
        auto iri = IRI{"http://www.w3.org/2001/XMLSchema#int"};
        auto lit1 = Literal{"101", iri};

        CHECK(not lit1.is_blank_node());
        CHECK(lit1.is_literal());
        CHECK(not lit1.is_variable());
        CHECK(not lit1.is_iri());
        CHECK(lit1.lexical_form() == "101");
        CHECK(lit1.datatype() == iri);
        CHECK(lit1.language_tag() == "");
        CHECK(std::string(lit1) == "\"101\"^^<http://www.w3.org/2001/XMLSchema#int>");
    }
    SUBCASE("date datatype"){
        auto iri = IRI{"http://www.w3.org/2001/XMLSchema#date"};
        auto lit1 = Literal{"2021-11-21", iri};

        CHECK(not lit1.is_blank_node());
        CHECK(lit1.is_literal());
        CHECK(not lit1.is_variable());
        CHECK(not lit1.is_iri());
        CHECK(lit1.lexical_form() == "2021-11-21");
        CHECK(lit1.datatype() == iri);
        CHECK(lit1.language_tag() == "");
        CHECK(std::string(lit1) == "\"2021-11-21\"^^<http://www.w3.org/2001/XMLSchema#date>");
    }
    SUBCASE("decimal datatype"){
        auto iri = IRI{"http://www.w3.org/2001/XMLSchema#decimal"};
        auto lit1 = Literal{"2.0", iri};

        CHECK(not lit1.is_blank_node());
        CHECK(lit1.is_literal());
        CHECK(not lit1.is_variable());
        CHECK(not lit1.is_iri());
        CHECK(lit1.lexical_form() == "2.0");
        CHECK(lit1.datatype() == iri);
        CHECK(lit1.language_tag() == "");
        CHECK(std::string(lit1) == "\"2.0\"^^<http://www.w3.org/2001/XMLSchema#decimal>");
    }
    SUBCASE("boolean datatype - true"){
        auto iri = IRI{"http://www.w3.org/2001/XMLSchema#boolean"};
        auto lit1 = Literal{"true", iri};

        CHECK(not lit1.is_blank_node());
        CHECK(lit1.is_literal());
        CHECK(not lit1.is_variable());
        CHECK(not lit1.is_iri());
        CHECK(lit1.lexical_form() == "true");
        CHECK(lit1.datatype() == iri);
        CHECK(lit1.language_tag() == "");
        CHECK(std::string(lit1) == "\"true\"^^<http://www.w3.org/2001/XMLSchema#boolean>");
    }
    SUBCASE("boolean datatype - false"){
        auto iri = IRI{"http://www.w3.org/2001/XMLSchema#boolean"};
        auto lit1 = Literal{"false", iri};

        CHECK(not lit1.is_blank_node());
        CHECK(lit1.is_literal());
        CHECK(not lit1.is_variable());
        CHECK(not lit1.is_iri());
        CHECK(lit1.lexical_form() == "false");
        CHECK(lit1.datatype() == iri);
        CHECK(lit1.language_tag() == "");
        CHECK(std::string(lit1) == "\"false\"^^<http://www.w3.org/2001/XMLSchema#boolean>");
    }
    SUBCASE("boolean datatype - 0"){
        auto iri = IRI{"http://www.w3.org/2001/XMLSchema#boolean"};
        auto lit1 = Literal{"0", iri};

        CHECK(not lit1.is_blank_node());
        CHECK(lit1.is_literal());
        CHECK(not lit1.is_variable());
        CHECK(not lit1.is_iri());
        CHECK(lit1.lexical_form() == "0");
        CHECK(lit1.datatype() == iri);
        CHECK(lit1.language_tag() == "");
        CHECK(std::string(lit1) == "\"0\"^^<http://www.w3.org/2001/XMLSchema#boolean>");
    }
    SUBCASE("boolean datatype - 1"){
        auto iri = IRI{"http://www.w3.org/2001/XMLSchema#boolean"};
        auto lit1 = Literal{"1", iri};

        CHECK(not lit1.is_blank_node());
        CHECK(lit1.is_literal());
        CHECK(not lit1.is_variable());
        CHECK(not lit1.is_iri());
        CHECK(lit1.lexical_form() == "1");
        CHECK(lit1.datatype() == iri);
        CHECK(lit1.language_tag() == "");
        CHECK(std::string(lit1) == "\"1\"^^<http://www.w3.org/2001/XMLSchema#boolean>");
    }
}

TEST_CASE("Literal - Check for lexical form with language tag") {

    auto iri = IRI{"http://www.w3.org/1999/02/22-rdf-syntax-ns#langString"};
    auto lit1 = Literal{"Bugs Bunny", "en"};

    CHECK(not lit1.is_blank_node());
    CHECK(lit1.is_literal());
    CHECK(not lit1.is_variable());
    CHECK(not lit1.is_iri());
    CHECK(lit1.lexical_form() == "Bugs Bunny");
    CHECK(lit1.datatype() == iri);
    CHECK(lit1.language_tag() == "en");
    CHECK(std::string(lit1) == "\"Bugs Bunny\"@en");
}
