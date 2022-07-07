#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Datatype AnyURI") {

    constexpr auto correct_type_iri_cstr = "http://www.w3.org/2001/XMLSchema#anyURI";

    CHECK(std::string(datatypes::xsd::AnyURI::identifier) == correct_type_iri_cstr);

    auto type_iri = IRI(datatypes::xsd::AnyURI::identifier);
    CHECK(type_iri.identifier() == correct_type_iri_cstr);

    using type = datatypes::xsd::AnyURI::cpp_type;

    CHECK(std::is_same_v<type, std::string>);

    auto lit1 = Literal::make<datatypes::xsd::AnyURI>(correct_type_iri_cstr);
    CHECK(lit1.value<datatypes::xsd::AnyURI>() == correct_type_iri_cstr);
    CHECK(lit1.lexical_form() == correct_type_iri_cstr);

    std::string value = "http://www.w3.org/2001/XMLSchema#date";
    auto lit2 = Literal::make<datatypes::xsd::AnyURI>(value);
    CHECK(lit2.value<datatypes::xsd::AnyURI>() == value);
    CHECK(lit2.lexical_form() == value);

    auto lit3 = Literal{correct_type_iri_cstr, type_iri};
    CHECK(lit3.value<datatypes::xsd::AnyURI>() == correct_type_iri_cstr);

    CHECK(lit1 != lit2);
    CHECK(lit2 != lit3);
    CHECK(lit1 == lit3);
}


