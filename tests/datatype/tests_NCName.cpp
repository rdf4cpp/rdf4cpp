#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Datatype NCName") {

    constexpr auto correct_type_iri_cstr = "http://www.w3.org/2001/XMLSchema#NCName";

    CHECK(std::string(datatypes::xsd::NCName::identifier) == correct_type_iri_cstr);

    auto type_iri = IRI(datatypes::xsd::NCName::identifier);
    CHECK(type_iri.identifier() == correct_type_iri_cstr);

    using type = datatypes::xsd::NCName::cpp_type;

    CHECK(std::is_same_v<type, std::string>);

    std::string value = "Bob";
    auto lit1 = Literal::make<datatypes::xsd::NCName>(value);
    CHECK(lit1.value<datatypes::xsd::NCName>() == value);
    CHECK(lit1.lexical_form() == value);

    value = "Alice";
    auto lit2 = Literal::make<datatypes::xsd::NCName>(value);
    CHECK(lit2.value<datatypes::xsd::NCName>() == value);
    CHECK(lit2.lexical_form() == value);

    value = "Bob";
    auto lit3 = Literal::make<datatypes::xsd::NCName>(value);
    CHECK(lit3.value<datatypes::xsd::NCName>() == value);
    CHECK(lit3.lexical_form() == value);

    value = "12sdfs2df";
    auto lit4 = Literal::make<datatypes::xsd::NCName>(value);
    CHECK(lit4.value<datatypes::xsd::NCName>() == value);
    CHECK(lit4.lexical_form() == value);

    value = "abc xyz";
    auto lit6 = Literal::make<datatypes::xsd::NCName>(value);
    CHECK(lit6.value<datatypes::xsd::NCName>() == value);
    CHECK(lit6.lexical_form() == value);

    value = "abc-xyz";
    auto lit7 = Literal::make<datatypes::xsd::NCName>(value);
    CHECK(lit7.value<datatypes::xsd::NCName>() == value);
    CHECK(lit7.lexical_form() == value);

    value = "abc_xyz";
    auto lit8 = Literal::make<datatypes::xsd::NCName>(value);
    CHECK(lit8.value<datatypes::xsd::NCName>() == value);
    CHECK(lit8.lexical_form() == value);

    CHECK(lit1 != lit2);
    CHECK(lit2 != lit3);
    CHECK(lit1 == lit3);
    CHECK(lit7 != lit8);

    // suppress warnings regarding attribute ‘nodiscard’
    Literal no_discard_dummy;

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("abc  xyz", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("abc  xyz    mno", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal(" xyz", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal(" ", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("xyz ", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("ns:name", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("sdfsdf\nsdfsdf", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("sdfsdf\tsdfsdf", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("sdfsdf\rdsfsdf", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("sdfsdf\n", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("sdfsdf\t", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("sdfsdf\r", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("\n", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("\t", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("\r", type_iri), "XSD Parsing Error", std::runtime_error);
}

