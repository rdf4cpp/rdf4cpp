#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include "rdf4cpp.hpp"

using namespace rdf4cpp::rdf;
using namespace datatypes;

TEST_CASE("Datatype Long") {

    constexpr auto correct_type_iri_cstr = "http://www.w3.org/2001/XMLSchema#long";

    CHECK(std::string(datatypes::xsd::Long::identifier) == correct_type_iri_cstr);

    auto type_iri = IRI(datatypes::xsd::Long::identifier);
    CHECK(type_iri.identifier() == correct_type_iri_cstr);

    using type = datatypes::xsd::Long::cpp_type;

    CHECK(std::is_same_v<type, int64_t>);

    int64_t value = 1;
    auto lit1 = Literal::make_typed_from_value<datatypes::xsd::Long>(value);
    CHECK(lit1.value<datatypes::xsd::Long>() == value);
    CHECK(lit1.lexical_form() == std::to_string(value));

    value = std::numeric_limits<int64_t>::min();
    auto lit2 = Literal::make_typed_from_value<datatypes::xsd::Long>(value);
    CHECK(lit2.value<datatypes::xsd::Long>() == value);
    CHECK(lit2.lexical_form() == std::to_string(value));

    value = std::numeric_limits<int64_t>::max();
    auto lit3 = Literal::make_typed_from_value<datatypes::xsd::Long>(value);
    CHECK(lit3.value<datatypes::xsd::Long>() == value);
    CHECK(lit3.lexical_form() == std::to_string(value));

    value = 1;
    auto lit4 = Literal::make_typed(std::to_string(value), type_iri);
    CHECK(lit4.value<datatypes::xsd::Long>() == value);

    value = std::numeric_limits<int64_t>::min();
    auto lit5 = Literal::make_typed(std::to_string(value), type_iri);
    CHECK(lit5.value<datatypes::xsd::Long>() == value);

    CHECK(lit1 != lit2);
    CHECK(lit2 != lit3);
    CHECK(lit1 == lit4);
    CHECK(lit2 == lit5);

    // suppress warnings regarding attribute ‘nodiscard’
    Literal no_discard_dummy;

    CHECK_THROWS(no_discard_dummy = Literal::make_typed("a23dg.59566", type_iri));

    CHECK_THROWS(no_discard_dummy = Literal::make_typed("\n", type_iri));

    CHECK_THROWS(no_discard_dummy = Literal::make_typed("qwerty", type_iri));

    CHECK_THROWS(no_discard_dummy = Literal::make_typed("1.00001", type_iri));
}

TEST_CASE("small 64bit positive int inlining") {
    auto const i = (1l << 41) - 1;
    auto const lit1 = Literal::make_typed_from_value<xsd::Long>(i);
    auto const lit2 = Literal::make_typed(std::to_string(i), IRI{xsd::Long::identifier});
    CHECK(lit1.backend_handle().is_inlined());
    CHECK(lit2.backend_handle().is_inlined());
    CHECK(lit1 == lit2);

    auto const extracted1 = lit1.template value<xsd::Long>();
    auto const extracted2 = lit2.value();
    CHECK(extracted1 == i);
    CHECK(std::any_cast<xsd::Long::cpp_type>(extracted2) == i);
}

TEST_CASE("negative 64bit int inlining") {
    auto const i = -256;
    auto const lit1 = Literal::make_typed_from_value<xsd::Long>(i);
    auto const lit2 = Literal::make_typed(std::to_string(i), IRI{xsd::Long::identifier});
    CHECK(lit1.backend_handle().is_inlined());
    CHECK(lit2.backend_handle().is_inlined());
    CHECK(lit1 == lit2);
}
