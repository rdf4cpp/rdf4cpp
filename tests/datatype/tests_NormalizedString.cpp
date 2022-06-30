#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Datatype NormalizedString") {

    constexpr auto correct_type_iri_cstr = "http://www.w3.org/2001/XMLSchema#normalizedString";

    CHECK(std::string(datatypes::xsd::NormalizedString::identifier) == correct_type_iri_cstr);

    auto type_iri = IRI(datatypes::xsd::NormalizedString::identifier);
    CHECK(type_iri.identifier() == correct_type_iri_cstr);

    using type = datatypes::xsd::NormalizedString::cpp_type;

    CHECK(std::is_same_v<type, std::string>);

    std::string value = "Bob";
    auto lit1 = Literal::make<datatypes::xsd::NormalizedString>(value);
    CHECK(lit1.value<datatypes::xsd::NormalizedString>() == value);
    CHECK(lit1.lexical_form() == value);

    value = "123";
    auto lit2 = Literal::make<datatypes::xsd::NormalizedString>(value);
    CHECK(lit2.value<datatypes::xsd::NormalizedString>() == value);
    CHECK(lit2.lexical_form() == value);

    value = "a";
    auto lit3 = Literal::make<datatypes::xsd::NormalizedString>(value);
    CHECK(lit3.value<datatypes::xsd::NormalizedString>() == value);
    CHECK(lit3.lexical_form() == value);

    value = "b";
    auto lit4 = Literal::make<datatypes::xsd::NormalizedString>(value);
    CHECK(lit4.value<datatypes::xsd::NormalizedString>() == value);
    CHECK(lit4.lexical_form() == value);

    value = "123";
    auto lit5 = Literal::make<datatypes::xsd::NormalizedString>(value);
    CHECK(lit5.value<datatypes::xsd::NormalizedString>() == value);
    CHECK(lit5.lexical_form() == value);

    value = "12sdfs2df";
    auto lit6 = Literal::make<datatypes::xsd::NormalizedString>(value);
    CHECK(lit6.value<datatypes::xsd::NormalizedString>() == value);
    CHECK(lit6.lexical_form() == value);

    value = "ab:ab-123";
    auto lit7 = Literal::make<datatypes::xsd::NormalizedString>(value);
    CHECK(lit7.value<datatypes::xsd::NormalizedString>() == value);
    CHECK(lit7.lexical_form() == value);

    value = "abc xyz";
    auto lit8 = Literal::make<datatypes::xsd::NormalizedString>(value);
    CHECK(lit8.value<datatypes::xsd::NormalizedString>() == value);
    CHECK(lit8.lexical_form() == value);

    value = " abc";
    auto lit9 = Literal::make<datatypes::xsd::NormalizedString>(value);
    CHECK(lit9.value<datatypes::xsd::NormalizedString>() == value);
    CHECK(lit9.lexical_form() == value);

    value = "abc ";
    auto lit10 = Literal::make<datatypes::xsd::NormalizedString>(value);
    CHECK(lit10.value<datatypes::xsd::NormalizedString>() == value);
    CHECK(lit10.lexical_form() == value);

    value = " ";
    auto lit11 = Literal::make<datatypes::xsd::NormalizedString>(value);
    CHECK(lit11.value<datatypes::xsd::NormalizedString>() == value);
    CHECK(lit11.lexical_form() == value);

    value = "abc   xyz";
    auto lit12 = Literal::make<datatypes::xsd::NormalizedString>(value);
    CHECK(lit12.value<datatypes::xsd::NormalizedString>() == value);
    CHECK(lit12.lexical_form() == value);

    CHECK(lit1 != lit2);
    CHECK(lit2 != lit3);
    CHECK(lit2 == lit5);
    CHECK(lit9 != lit10);
    CHECK(lit8 != lit12);

    // suppress warnings regarding attribute ‘nodiscard’
    Literal no_discard_dummy;

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

