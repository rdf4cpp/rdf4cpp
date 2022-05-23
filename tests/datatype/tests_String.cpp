#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Datatype String") {

    constexpr auto correct_type_iri_cstr = "http://www.w3.org/2001/XMLSchema#string";

    CHECK(std::string(datatypes::xsd::String::identifier) == correct_type_iri_cstr);

    auto type_iri = IRI(datatypes::xsd::String::identifier);
    CHECK(type_iri.identifier() == correct_type_iri_cstr);

    using type = datatypes::xsd::String::cpp_type;

    CHECK(std::is_same_v<type, std::string>);


    std::string value = "123";
    auto lit1 = Literal::make<datatypes::xsd::String>(value);
    CHECK(lit1.value<datatypes::xsd::String>() == value);
    CHECK(lit1.lexical_form() == value);

    value = "a";
    auto lit2 = Literal::make<datatypes::xsd::String>(value);
    CHECK(lit2.value<datatypes::xsd::String>() == value);
    CHECK(lit2.lexical_form() == value);

    value = "b";
    auto lit3 = Literal::make<datatypes::xsd::String>(value);
    CHECK(lit3.value<datatypes::xsd::String>() == value);
    CHECK(lit3.lexical_form() == value);

    value = "123";
    auto lit4 = Literal{value, type_iri};
    CHECK(lit4.value<datatypes::xsd::String>() == value);

    value = "a";
    auto lit5 = Literal{value, type_iri};
    CHECK(lit5.value<datatypes::xsd::String>() == value);

    value = "\n";
    auto lit6 = Literal{value, type_iri};
    CHECK(lit6.value<datatypes::xsd::String>() == value);

    value = "\t";
    auto lit7 = Literal{value, type_iri};
    CHECK(lit7.value<datatypes::xsd::String>() == value);

    value = "\r";
    auto lit8 = Literal{value, type_iri};
    CHECK(lit8.value<datatypes::xsd::String>() == value);

    CHECK(lit1 != lit2);
    CHECK(lit2 != lit3);
    CHECK(lit1 == lit4);
    CHECK(lit2 == lit5);
}
