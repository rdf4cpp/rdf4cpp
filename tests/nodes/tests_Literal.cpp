#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

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

    SUBCASE("string datatype") {
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
    SUBCASE("int datatype") {
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
    SUBCASE("date datatype") {
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
    SUBCASE("decimal datatype") {
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
    SUBCASE("boolean datatype - true") {
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
    SUBCASE("boolean datatype - false") {
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
    SUBCASE("boolean datatype - 0") {
        auto iri = IRI{"http://www.w3.org/2001/XMLSchema#boolean"};
        auto lit1 = Literal{"0", iri};

        CHECK(not lit1.is_blank_node());
        CHECK(lit1.is_literal());
        CHECK(not lit1.is_variable());
        CHECK(not lit1.is_iri());
        CHECK(lit1.lexical_form() == "false");
        CHECK(lit1.datatype() == iri);
        CHECK(lit1.language_tag() == "");
        CHECK(std::string(lit1) == "\"false\"^^<http://www.w3.org/2001/XMLSchema#boolean>");
    }
    SUBCASE("boolean datatype - 1") {
        auto iri = IRI{"http://www.w3.org/2001/XMLSchema#boolean"};
        auto lit1 = Literal{"1", iri};

        CHECK(not lit1.is_blank_node());
        CHECK(lit1.is_literal());
        CHECK(not lit1.is_variable());
        CHECK(not lit1.is_iri());
        CHECK(lit1.lexical_form() == "true");
        CHECK(lit1.datatype() == iri);
        CHECK(lit1.language_tag() == "");
        CHECK(std::string(lit1) == "\"true\"^^<http://www.w3.org/2001/XMLSchema#boolean>");
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

TEST_CASE("Literal - ctor edge-case") {
    IRI const iri{"http://www.w3.org/2001/XMLSchema#int"};
    Literal const lit1{"1", iri};
    Literal const lit2{"2", iri};

    Literal const expected{"3", iri};
    CHECK(lit1 + lit2 == expected);
}

TEST_CASE("Literal - check fixed id") {
    IRI const iri{datatypes::registry::xsd_string};
    Literal const lit{"hello", iri};

    CHECK(lit.backend_handle().node_id().literal_type().is_fixed());
    CHECK(lit.datatype().backend_handle().node_id().value() < datatypes::registry::min_dynamic_datatype_id);
    CHECK(iri.backend_handle().node_id().value() < datatypes::registry::min_dynamic_datatype_id);
}

TEST_CASE("Literal - casting") {
    using namespace datatypes::xsd;

    auto const lit1 = Literal::make<datatypes::xsd::Int>(123);

    SUBCASE("id cast") {
        auto const lit1 = Literal::make<String>("hello");
        auto const lit2 = lit1.template cast<String>();

        CHECK(lit2.datatype() == IRI{String::identifier});
        CHECK(lit2.template value<String>() == "hello");
    }

    SUBCASE("str -> any") {
        auto const lit1 = Literal::make<String>("1.2");
        auto const lit2 = lit1.template cast<Float>();

        CHECK(lit2.datatype() == IRI{Float::identifier});
        CHECK(lit2.value<Float>() == 1.2f);
    }

    SUBCASE("any -> str") {
        auto const lit1 = Literal::make<Decimal>(1.5);
        auto const lit2 = lit1.template cast<String>();

        CHECK(lit2.template value<String>() == "1.5");
    }

    SUBCASE("any -> bool") {
        auto const lit1 = Literal::make<Float>(1.4);
        auto const lit2 = lit1.template cast<Boolean>();

        CHECK(lit2.datatype() == IRI{Boolean::identifier});
        CHECK(lit2.template value<Boolean>() == true);
    }

    SUBCASE("downcast: dbl -> flt") {
        auto const lit1 = Literal::make<Double>(1.4);
        auto const lit2 = lit1.template cast<Float>();

        CHECK(lit2.datatype() == IRI{Float::identifier});
        CHECK(lit2.template value<Float>() == 1.4f);
    }

    SUBCASE("dec -> flt") {
        auto const lit1 = Literal::make<Decimal>(1.0);
        auto const lit2 = lit1.template cast<Float>();

        CHECK(lit2.datatype() == IRI{Float::identifier});
        CHECK(lit2.template value<Float>() == 1.f);
    }

    SUBCASE("dec -> dbl") {
        auto const lit1 = Literal::make<Decimal>(1.0);
        auto const lit2 = lit1.template cast<Double>();

        CHECK(lit2.datatype() == IRI{Double::identifier});
        CHECK(lit2.template value<Double>() == 1.0);
    }

    SUBCASE("dec -> int") {
        auto const lit1 = Literal::make<Decimal>(1.2);
        auto const lit2 = lit1.template cast<Int>();

        CHECK(lit2.datatype() == IRI{Int::identifier});
        CHECK(lit2.template value<Int>() == 1);
    }

    SUBCASE("downcast: int -> dec") {
        auto const lit1 = Literal::make<Integer>(1);
        auto const lit2 = lit1.template cast<Decimal>();

        CHECK(lit2.datatype() == IRI{Decimal::identifier});
        CHECK(lit2.template value<Decimal>() == 1);
    }

    SUBCASE("int -> flt") {
        auto const lit1 = Literal::make<Integer>(1);
        auto const lit2 = lit1.template cast<Float>();

        CHECK(lit2.datatype() == IRI{Float::identifier});
        CHECK(lit2.template value<Float>() == 1.f);
    }

    SUBCASE("int -> dbl") {
        auto const lit1 = Literal::make<Integer>(1);
        auto const lit2 = lit1.template cast<Double>();

        CHECK(lit2.datatype() == IRI{Double::identifier});
        CHECK(lit2.template value<Double>() == 1.0);
    }

    SUBCASE("bool -> numeric") {
        auto const lit1 = Literal::make<Boolean>(true);
        auto const lit2 = lit1.template cast<Integer>();

        CHECK(lit2.datatype() == IRI{Integer::identifier});
        CHECK(lit2.value<Integer>() == 1);

        auto const lit3 = Literal::make<Boolean>(false);
        auto const lit4 = lit3.template cast<Float>();

        CHECK(lit4.datatype() == IRI{Float::identifier});
        CHECK(lit4.value<Float>() == 0.f);
    }

    SUBCASE("cross hierarchy: int -> unsignedInt") {
        auto const lit1 = Literal::make<Int>(1);
        auto const lit2 = lit1.template cast<UnsignedInt>();

        CHECK(lit2.datatype() == IRI{UnsignedInt::identifier});
        CHECK(lit2.value<UnsignedInt>() == 1);
    }

    SUBCASE("IRI -> str") {
        // TODO: possible with IRI::identifier but IRI would need cast function (?)
    }

    SUBCASE("subtypes") {
        CHECK(lit1.template cast<Integer>().datatype() == IRI{Integer::identifier});
        CHECK(lit1.template cast<Float>().datatype() == IRI{Float::identifier});

        auto const lit2 = Literal::make<Integer>(420);
        CHECK(lit2.template cast<Int>() == Literal::make<Int>(420));
    }
}
