#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Datatype NonNegativeInteger") {

    constexpr auto correct_type_iri_cstr = "http://www.w3.org/2001/XMLSchema#nonNegativeInteger";

    CHECK(std::string(datatypes::xsd::NonNegativeInteger::identifier) == correct_type_iri_cstr);

    auto type_iri = IRI(datatypes::xsd::NonNegativeInteger::identifier);
    CHECK(type_iri.identifier() == correct_type_iri_cstr);

    int64_t value = 1;
    auto lit1 = Literal::make<datatypes::xsd::NonNegativeInteger>(value);
    CHECK(lit1.value<datatypes::xsd::NonNegativeInteger>() == value);
    CHECK(lit1.lexical_form() == std::to_string(value));

    value = 18446744073709;
    auto lit2 = Literal::make<datatypes::xsd::NonNegativeInteger>(value);
    CHECK(lit2.value<datatypes::xsd::NonNegativeInteger>() == value);
    CHECK(lit2.lexical_form() == std::to_string(value));

    value = 0;
    auto lit3 = Literal::make<datatypes::xsd::NonNegativeInteger>(value);
    CHECK(lit3.value<datatypes::xsd::NonNegativeInteger>() == value);
    CHECK(lit3.lexical_form() == std::to_string(value));

    value = 1;
    auto lit4 = Literal{std::to_string(value), type_iri};
    CHECK(lit4.value<datatypes::xsd::NonNegativeInteger>() == value);

    value = 18446744073709;
    auto lit5 = Literal{std::to_string(value), type_iri};
    CHECK(lit5.value<datatypes::xsd::NonNegativeInteger>() == value);

    CHECK(lit1 != lit2);
    CHECK(lit2 != lit3);
    CHECK(lit1 == lit4);
    CHECK(lit2 == lit5);

    // suppress warnings regarding attribute ‘nodiscard’
    Literal no_discard_dummy;

    CHECK_THROWS(no_discard_dummy = Literal("qwerty", type_iri));

    CHECK_THROWS(no_discard_dummy = Literal("-1", type_iri));

    CHECK_THROWS(no_discard_dummy = Literal("a23dg.59566", type_iri));

    CHECK_THROWS(no_discard_dummy = Literal("-0.01", type_iri));
}

TEST_CASE("xsd:nonNegativeInteger inlining") {
    using datatypes::xsd::NonNegativeInteger;

    auto zero_lit = Literal::make<NonNegativeInteger>(0);
    CHECK(zero_lit.backend_handle().is_inlined());
    CHECK(zero_lit.value<NonNegativeInteger>() == 0);

    auto one_lit = Literal::make<NonNegativeInteger>(1);
    CHECK(one_lit.backend_handle().is_inlined());
    CHECK(one_lit.value<NonNegativeInteger>() == 1);

    auto large_lit = Literal::make<NonNegativeInteger>((1ul << 42) - 1);
    CHECK(large_lit.backend_handle().is_inlined());
    CHECK(large_lit.value<NonNegativeInteger>() == ((1ul << 42) - 1));
}
