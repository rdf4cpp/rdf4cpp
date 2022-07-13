#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("boolean capabilities") {
    static_assert(datatypes::LiteralDatatype<datatypes::xsd::Boolean>);
    static_assert(datatypes::LogicalLiteralDatatype<datatypes::xsd::Boolean>);
    static_assert(!datatypes::NumericLiteralDatatype<datatypes::xsd::Boolean>);
    static_assert(!datatypes::PromotableLiteralDatatype<datatypes::xsd::Boolean>);
    static_assert(!datatypes::SubtypedLiteralDatatype<datatypes::xsd::Boolean>);
}

TEST_CASE("Datatype Boolean") {

    constexpr auto correct_type_iri_cstr = "http://www.w3.org/2001/XMLSchema#boolean";

    CHECK(std::string(datatypes::xsd::Boolean::identifier) == correct_type_iri_cstr);

    auto type_iri = IRI(datatypes::xsd::Boolean::identifier);
    CHECK(type_iri.identifier() == correct_type_iri_cstr);

    using type = datatypes::xsd::Boolean::cpp_type;

    CHECK(std::is_same_v<type, bool>);

    std::string true_val{"true"};
    std::string false_val{"false"};

    bool value = true;
    auto lit1 = Literal::make<datatypes::xsd::Boolean>(value);
    CHECK(lit1.value<datatypes::xsd::Boolean>() == value);
    CHECK(lit1.lexical_form() == true_val);

    value = false;
    auto lit2 = Literal::make<datatypes::xsd::Boolean>(value);
    CHECK(lit2.value<datatypes::xsd::Boolean>() == value);
    CHECK(lit2.lexical_form() == false_val);

    value = 1;
    auto lit3 = Literal::make<datatypes::xsd::Boolean>(value);
    CHECK(lit3.value<datatypes::xsd::Boolean>() == value);
    CHECK(lit3.lexical_form() == true_val);

    value = 0;
    auto lit4 = Literal::make<datatypes::xsd::Boolean>(value);
    CHECK(lit4.value<datatypes::xsd::Boolean>() == value);
    CHECK(lit4.lexical_form() == false_val);

    value = true;
    auto lit5 = Literal{true_val, type_iri};
    CHECK(lit5.value<datatypes::xsd::Boolean>() == value);
    CHECK(std::any_cast<bool>(lit5.value()) == value);

    value = false;
    auto lit6 = Literal{false_val, type_iri};
    CHECK(lit6.value<datatypes::xsd::Boolean>() == value);
    CHECK(std::any_cast<bool>(lit6.value()) == value);

    value = 1;
    auto lit7 = Literal{"1", type_iri};
    CHECK(lit7.value<datatypes::xsd::Boolean>() == value);
    CHECK(lit7.lexical_form() == true_val);

    value = 0;
    auto lit8 = Literal{"0", type_iri};
    CHECK(lit8.value<datatypes::xsd::Boolean>() == value);
    CHECK(lit8.lexical_form() == false_val);

    CHECK(lit1 != lit2);
    CHECK(lit2 != lit3);
    CHECK(lit1 == lit3);
    CHECK(lit2 == lit4);
    CHECK(lit1 == lit5);
    CHECK(lit2 == lit6);
    CHECK(lit3 == lit5);
    CHECK(lit4 == lit6);

    // suppress warnings regarding attribute ‘nodiscard’
    Literal no_discard_dummy;

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("5", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy =  Literal("adsfg", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy =  Literal("5.64566", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy =  Literal("1.7e", type_iri), "XSD Parsing Error", std::runtime_error);
}