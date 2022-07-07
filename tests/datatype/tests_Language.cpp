#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Datatype Language") {

    constexpr auto correct_type_iri_cstr = "http://www.w3.org/2001/XMLSchema#language";

    CHECK(std::string(datatypes::xsd::Language::identifier) == correct_type_iri_cstr);

    auto type_iri = IRI(datatypes::xsd::Language::identifier);
    CHECK(type_iri.identifier() == correct_type_iri_cstr);

    using type = datatypes::xsd::Language::cpp_type;

    CHECK(std::is_same_v<type, std::string>);

    std::string value = "en";
    auto lit1 = Literal::make<datatypes::xsd::Language>(value);
    CHECK(lit1.value<datatypes::xsd::Language>() == value);
    CHECK(lit1.lexical_form() == value);

    value = "de";
    auto lit2 = Literal::make<datatypes::xsd::Language>(value);
    CHECK(lit2.value<datatypes::xsd::Language>() == value);
    CHECK(lit2.lexical_form() == value);

    value = "xx";
    auto lit3 = Literal::make<datatypes::xsd::Language>(value);
    CHECK(lit3.value<datatypes::xsd::Language>() == value);
    CHECK(lit3.lexical_form() == value);

    value = "en";
    auto lit4 = Literal{value, type_iri};
    CHECK(lit4.value<datatypes::xsd::Language>() == value);
    CHECK(lit4.lexical_form() == value);

    value = "de";
    auto lit5 = Literal{value, type_iri};
    CHECK(lit5.value<datatypes::xsd::Language>() == value);
    CHECK(lit5.lexical_form() == value);

    value = "de-12";
    auto lit6 = Literal{value, type_iri};
    CHECK(lit6.value<datatypes::xsd::Language>() == value);
    CHECK(lit6.lexical_form() == value);

    value = "d-12";
    auto lit7 = Literal{value, type_iri};
    CHECK(lit7.value<datatypes::xsd::Language>() == value);
    CHECK(lit7.lexical_form() == value);

    value = "abcdefgh-12ab12";
    auto lit8 = Literal{value, type_iri};
    CHECK(lit8.value<datatypes::xsd::Language>() == value);
    CHECK(lit8.lexical_form() == value);

    value = "abcdefgh-abcdefgh";
    auto lit9 = Literal{value, type_iri};
    CHECK(lit9.value<datatypes::xsd::Language>() == value);
    CHECK(lit9.lexical_form() == value);

    CHECK(lit1 != lit2);
    CHECK(lit2 != lit3);
    CHECK(lit1 == lit4);
    CHECK(lit2 == lit5);

    // suppress warnings regarding attribute ‘nodiscard’
    Literal no_discard_dummy;

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("12en", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("en12", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("abcdefghi", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("abcdefghi:456", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("sdfsdf\nsdfsdf", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("sdfsdf\tsdfsdf", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("sdfsdf\rdsfsdf", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("sdfsdf\n", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("sdfsdf\t", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("sdfsdf\r", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("\n", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("\t", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("\r", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("abc  xyz", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("abc  xyz    mno", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal(" xyz", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal(" ", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("xyz ", type_iri), "XSD Parsing Error", std::runtime_error);
}

