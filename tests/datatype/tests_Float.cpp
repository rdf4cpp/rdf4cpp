#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>
#include <cmath>

using namespace rdf4cpp::rdf;

TEST_CASE("float capabilities") {
    static_assert(datatypes::LiteralDatatype<datatypes::xsd::Float>);
    static_assert(datatypes::NumericLiteralDatatype<datatypes::xsd::Float>);
    static_assert(datatypes::LogicalLiteralDatatype<datatypes::xsd::Float>);
    static_assert(datatypes::PromotableLiteralDatatype<datatypes::xsd::Float>);
    static_assert(!datatypes::SubtypedLiteralDatatype<datatypes::xsd::Float>);
    static_assert(datatypes::ComparableLiteralDatatype<datatypes::xsd::Float>);
    static_assert(datatypes::FixedIdLiteralDatatype<datatypes::xsd::Float>);
}

TEST_CASE("Datatype Float") {

    constexpr auto correct_type_iri_cstr = "http://www.w3.org/2001/XMLSchema#float";

    CHECK(std::string(datatypes::xsd::Float::identifier) == correct_type_iri_cstr);

    auto type_iri = IRI(datatypes::xsd::Float::identifier);
    CHECK(type_iri.identifier() == correct_type_iri_cstr);

    using type = datatypes::xsd::Float::cpp_type;

    CHECK(std::is_same_v<type, float>);

    std::string const rdf_float_1_0{"1.0E0"};

    type value = 1.00f;
    auto lit1 = Literal::make<datatypes::xsd::Float>(value);
    CHECK(lit1.value<datatypes::xsd::Float>() == value);
    CHECK(lit1.lexical_form() == rdf_float_1_0);

    value = 32568.2350f;
    auto lit2 = Literal::make<datatypes::xsd::Float>(value);
    CHECK(lit2.value<datatypes::xsd::Float>() == value);

    value = -14523.2350f;
    auto lit3 = Literal::make<datatypes::xsd::Float>(value);
    CHECK(lit3.value<datatypes::xsd::Float>() == value);

    value = 1;
    auto lit4 = Literal::make<datatypes::xsd::Float>(value);
    CHECK(lit4.value<datatypes::xsd::Float>() == value);
    CHECK(lit4.lexical_form() == rdf_float_1_0);

    value = 1;
    auto lit6 = Literal{std::to_string(value), type_iri};
    CHECK(lit6.value<datatypes::xsd::Float>() == value);
    CHECK(lit6.lexical_form() == rdf_float_1_0);

    value = 32568.2350f;
    auto lit7 = Literal{std::to_string(value), type_iri};
    CHECK(lit7.value<datatypes::xsd::Float>() == value);

    auto lit8 = Literal{"NaN", type_iri};
    CHECK(std::isnan(lit8.value<datatypes::xsd::Float>()));

    auto lit9 = Literal{"INF", type_iri};
    CHECK(std::isinf(lit9.value<datatypes::xsd::Float>()));

    value = INFINITY;
    auto lit10 = Literal::make<datatypes::xsd::Float>(value);
    CHECK(std::isinf(lit10.value<datatypes::xsd::Float>()));

    value = NAN;
    auto lit11 = Literal::make<datatypes::xsd::Float>(value);
    CHECK(std::isnan(lit11.value<datatypes::xsd::Float>()));

    value = 1.17e-38;
    auto lit12 = Literal{"1.17e-38", type_iri};
    CHECK(lit12.value<datatypes::xsd::Float>() == value);

    auto lit13 = Literal{"+INF", type_iri};
    CHECK(std::isinf(lit13.value<datatypes::xsd::Float>()));

    auto lit14 = Literal{"-INF", type_iri};
    CHECK(std::isinf(lit14.value<datatypes::xsd::Float>()));
    CHECK(lit14.value<datatypes::xsd::Float>() == -std::numeric_limits<type>::infinity());

    value = -INFINITY;
    auto lit15 = Literal::make<datatypes::xsd::Float>(value);
    CHECK(std::isinf(lit15.value<datatypes::xsd::Float>()));
    CHECK(lit15.value<datatypes::xsd::Float>() == -std::numeric_limits<type>::infinity());

    CHECK(lit1 != lit2);
    CHECK(lit2 != lit3);
    CHECK(lit1 == lit4);
    CHECK(lit1 == lit6);
    CHECK(lit2 == lit7);
    CHECK(lit9 == lit10);
    CHECK(lit9 == lit13);
    CHECK(lit10 == lit13);
    CHECK(lit14 == lit15);

    // suppress warnings regarding attribute ‘nodiscard’
    Literal no_discard_dummy;

    CHECK_THROWS(no_discard_dummy = Literal("454sdsd", type_iri));
}

TEST_CASE("round-trip") {
    datatypes::xsd::Float::cpp_type const value = -0.123456789f;
    auto const lit = Literal::make<datatypes::xsd::Float>(value);
    std::cout << lit.lexical_form() << std::endl;
    CHECK(lit.value<datatypes::xsd::Float>() == value);
}
