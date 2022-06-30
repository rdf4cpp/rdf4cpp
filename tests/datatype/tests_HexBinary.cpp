#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Datatype HexBinary") {

    constexpr auto correct_type_iri_cstr = "http://www.w3.org/2001/XMLSchema#hexBinary";

    CHECK(std::string(datatypes::xsd::HexBinary::identifier) == correct_type_iri_cstr);

    auto type_iri = IRI(datatypes::xsd::HexBinary::identifier);
    CHECK(type_iri.identifier() == correct_type_iri_cstr);

    using type = datatypes::xsd::HexBinary::cpp_type;

    CHECK(std::is_same_v<type, std::vector<std::int16_t>>);

    std::vector<std::int16_t> value = {0x0FB7, 0x0AB7};
    auto lit1 = Literal::make<datatypes::xsd::HexBinary>(value);
    CHECK(lit1.value<datatypes::xsd::HexBinary>() == value);

    value = {0x0FB7, 0x0AB7, 0x0FB1};
    auto lit2 = Literal::make<datatypes::xsd::HexBinary>(value);
    CHECK(lit2.value<datatypes::xsd::HexBinary>() == value);

    value = {0x0FB7, 0x0AB7, 0x0DB1};
    auto lit3 = Literal::make<datatypes::xsd::HexBinary>(value);
    CHECK(lit3.value<datatypes::xsd::HexBinary>() == value);

    value = {4023, 2743};
    auto lit4 = Literal::make<datatypes::xsd::HexBinary>(value);
    CHECK(lit4.value<datatypes::xsd::HexBinary>() == value);

    auto lit5 = Literal{"0fb70ab7", type_iri};
    CHECK(lit5.value<datatypes::xsd::HexBinary>() == value);

    value = {0x0fb7, 0x0a};
    auto lit6 = Literal{"0fb70a", type_iri};
    CHECK(lit6.value<datatypes::xsd::HexBinary>() == value);

    CHECK(lit1 != lit2);
    CHECK(lit2 != lit3);
    CHECK(lit1 != lit3);
    CHECK(lit1 == lit4);
    CHECK(lit1 == lit5);
    CHECK(lit4 == lit5);
    CHECK(lit1.value<datatypes::xsd::HexBinary>() == lit5.value<datatypes::xsd::HexBinary>());
    CHECK(lit4.value<datatypes::xsd::HexBinary>() == lit5.value<datatypes::xsd::HexBinary>());

    // suppress warnings regarding attribute ‘nodiscard’
    Literal no_discard_dummy;

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("0fb70", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("0.0", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("f", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("\t", type_iri), "XSD Parsing Error", std::runtime_error);

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("0gb70za7", type_iri), "XSD Parsing Error", std::runtime_error);
}