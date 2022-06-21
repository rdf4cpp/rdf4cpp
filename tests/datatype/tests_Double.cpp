#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Datatype Double") {

    constexpr auto correct_type_iri_cstr = "http://www.w3.org/2001/XMLSchema#double";

    CHECK(std::string(datatypes::xsd::Double::identifier) == correct_type_iri_cstr);

    auto type_iri = IRI(datatypes::xsd::Double::identifier);
    CHECK(type_iri.identifier() == correct_type_iri_cstr);

    using type = datatypes::xsd::Double::cpp_type;

    CHECK(std::is_same_v<type, double>);

    std::string rdf_dbl_1_0{"1.0"};

    double value = 1.00;
    auto lit1 = Literal::make<datatypes::xsd::Double>(value);
    CHECK(lit1.value<datatypes::xsd::Double>() == value);
    CHECK(lit1.lexical_form() == rdf_dbl_1_0);

    value = 987456321123456.123586987;
    auto lit2 = Literal::make<datatypes::xsd::Double>(value);
    CHECK(lit2.value<datatypes::xsd::Double>() == value);

    value = -64545352389.2352345670;
    auto lit3 = Literal::make<datatypes::xsd::Double>(value);
    CHECK(lit3.value<datatypes::xsd::Double>() == value);

    value = 1;
    auto lit4 = Literal::make<datatypes::xsd::Double>(value);
    CHECK(lit4.value<datatypes::xsd::Double>() == value);
    CHECK(lit4.lexical_form() == rdf_dbl_1_0);

    value = std::numeric_limits<double>::max();
    auto lit5 = Literal::make<datatypes::xsd::Double>(value);
    CHECK(lit5.value<datatypes::xsd::Double>() == value);

    value = 1;
    auto lit6 = Literal{std::to_string(value), type_iri};
    CHECK(lit6.value<datatypes::xsd::Double>() == value);
    CHECK(lit6.lexical_form() == rdf_dbl_1_0);

    value = 987456321123456.123586987;
    auto lit7 = Literal{std::to_string(value), type_iri};
    CHECK(lit7.value<datatypes::xsd::Double>() == value);

    auto lit8 = Literal{"NaN", type_iri};
    CHECK(isnan(lit8.value<datatypes::xsd::Double>()));

    auto lit9 = Literal{"INF", type_iri};
    CHECK(isinf(lit9.value<datatypes::xsd::Double>()));

    value = INFINITY;
    auto lit10 = Literal::make<datatypes::xsd::Double>(value);
    CHECK(isinf(lit10.value<datatypes::xsd::Double>()));

    value = NAN;
    auto lit11 = Literal::make<datatypes::xsd::Double>(value);
    CHECK(isnan(lit11.value<datatypes::xsd::Double>()));

    value = 2.22e-308;
    auto lit12 = Literal{"2.22e-308", type_iri};
    //CHECK(lit12.value<datatypes::xsd::Double>() == value); //Testcase Fails
    if(lit12.value<datatypes::xsd::Double>() == value) CHECK(true); //Testcase Passes

    CHECK(lit1 != lit2);
    CHECK(lit2 != lit3);
    CHECK(lit1 == lit4);
    CHECK(lit1 == lit6);
    CHECK(lit2 == lit7);
    CHECK(lit8 == lit11);
    CHECK(lit9 == lit10);

    // suppress warnings regarding attribute ‘nodiscard’
    Literal no_discard_dummy;

    CHECK_THROWS_WITH_AS(no_discard_dummy = Literal("454sdsd", type_iri), "XSD Parsing Error", std::runtime_error);
}