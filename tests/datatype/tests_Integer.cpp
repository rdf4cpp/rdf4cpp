#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Datatype Integer") {

    constexpr auto correct_type_iri_cstr = "http://www.w3.org/2001/XMLSchema#integer";

    CHECK(std::string(datatypes::xsd::Integer::identifier) == correct_type_iri_cstr);

    auto type_iri = IRI(datatypes::xsd::Integer::identifier);
    CHECK(type_iri.identifier() == correct_type_iri_cstr);

    using type = datatypes::xsd::Integer::cpp_type;

    CHECK(std::is_same_v<type, int64_t>);


    int64_t value = 1.00;
    auto lit1 = Literal::make<datatypes::xsd::Integer>(value);
    CHECK(lit1.value<datatypes::xsd::Integer>() == value);
    CHECK(lit1.lexical_form() == std::to_string(value));

    value = -2147483648;
    auto lit2 = Literal::make<datatypes::xsd::Integer>(value);
    CHECK(lit2.value<datatypes::xsd::Integer>() == value);
    CHECK(lit2.lexical_form() == std::to_string(value));

    value = 2147483647;
    auto lit3 = Literal::make<datatypes::xsd::Integer>(value);
    CHECK(lit3.value<datatypes::xsd::Integer>() == value);
    CHECK(lit3.lexical_form() == std::to_string(value));

    //Testing Literal Constructor
    value = 1;
    auto lit4 = Literal{std::to_string(value), type_iri};
    CHECK(lit4.value<datatypes::xsd::Integer>() == value);

    value = 2147483647;
    auto lit5 = Literal{std::to_string(value), type_iri};
    CHECK(lit5.value<datatypes::xsd::Integer>() == value);

    auto lit6 = Literal{"+1", type_iri};
    CHECK(lit6.value<datatypes::xsd::Integer>() == 1);

    value = std::numeric_limits<int64_t>::max();
    auto lit7 = Literal::make<datatypes::xsd::Integer>(value);
    CHECK(lit7.value<datatypes::xsd::Integer>() == value);
    CHECK(lit7.lexical_form() == std::to_string(value));

    value = std::numeric_limits<int64_t>::min();
    auto lit8 = Literal::make<datatypes::xsd::Integer>(value);
    CHECK(lit8.value<datatypes::xsd::Integer>() == value);
    CHECK(lit8.lexical_form() == std::to_string(value));

    CHECK(lit1 != lit2);
    CHECK(lit2 != lit3);
    CHECK(lit1 == lit4);
    CHECK(lit3 == lit5);
    CHECK(lit4.value<datatypes::xsd::Integer>() == lit6.value<datatypes::xsd::Integer>()); //literal representation will not be matched but the values will match.

    // suppress warnings regarding attribute ‘nodiscard’
    std::any no_discard_dummy = false;

    auto lit9 = Literal{"a23dg", type_iri};
    CHECK_THROWS_WITH_AS(no_discard_dummy = lit9.value(), "XSD Parsing Error", std::runtime_error);

    auto lit10 = Literal{"2.2e-308", type_iri};
    CHECK_THROWS_WITH_AS(no_discard_dummy = lit10.value(), "XSD Parsing Error", std::runtime_error);
}