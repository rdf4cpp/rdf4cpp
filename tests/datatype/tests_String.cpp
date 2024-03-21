#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp.hpp>

using namespace rdf4cpp;

TEST_CASE("string capabilities") {
    static_assert(datatypes::LiteralDatatype<datatypes::xsd::String>);
    static_assert(datatypes::LogicalLiteralDatatype<datatypes::xsd::String>);
    static_assert(!datatypes::NumericLiteralDatatype<datatypes::xsd::String>);
    static_assert(!datatypes::PromotableLiteralDatatype<datatypes::xsd::String>);
    static_assert(!datatypes::SubtypedLiteralDatatype<datatypes::xsd::String>);
    static_assert(datatypes::ComparableLiteralDatatype<datatypes::xsd::String>);
    static_assert(datatypes::FixedIdLiteralDatatype<datatypes::xsd::String>);
}

TEST_CASE("Datatype String") {

    constexpr auto correct_type_iri_cstr = "http://www.w3.org/2001/XMLSchema#string";

    CHECK(std::string(datatypes::xsd::String::identifier) == correct_type_iri_cstr);

    auto type_iri = IRI(datatypes::xsd::String::identifier);
    CHECK(type_iri.identifier() == correct_type_iri_cstr);

    std::string value = "123";
    auto lit1 = Literal::make_typed_from_value<datatypes::xsd::String>(value);
    CHECK(lit1.value<datatypes::xsd::String>() == value);
    CHECK(lit1.lexical_form() == value);

    value = "a";
    auto lit2 = Literal::make_typed_from_value<datatypes::xsd::String>(value);
    CHECK(lit2.value<datatypes::xsd::String>() == value);
    CHECK(lit2.lexical_form() == value);

    value = "b";
    auto lit3 = Literal::make_typed_from_value<datatypes::xsd::String>(value);
    CHECK(lit3.value<datatypes::xsd::String>() == value);
    CHECK(lit3.lexical_form() == value);

    value = "123";
    auto lit4 = Literal::make_typed(value, type_iri);
    CHECK(lit4.value<datatypes::xsd::String>() == value);

    value = "a";
    auto lit5 = Literal::make_typed(value, type_iri);
    CHECK(lit5.value<datatypes::xsd::String>() == value);

    value = "\n";
    auto lit6 = Literal::make_typed(value, type_iri);
    CHECK(lit6.value<datatypes::xsd::String>() == value);
    CHECK_EQ(std::string{lit6}, R"("\n")");

    value = "\t";
    auto lit7 = Literal::make_typed(value, type_iri);
    CHECK(lit7.value<datatypes::xsd::String>() == value);

    value = "\r";
    auto lit8 = Literal::make_typed(value, type_iri);
    CHECK(lit8.value<datatypes::xsd::String>() == value);
    CHECK_EQ(std::string{lit8}, R"("\r")");

    value = "\\";
    auto lit9 = Literal::make_typed(value, type_iri);
    CHECK(lit9.value<datatypes::xsd::String>() == value);
    CHECK_EQ(std::string{lit9}, R"("\\")");

    value = "\"";
    auto lit10 = Literal::make_typed(value, type_iri);
    CHECK(lit10.value<datatypes::xsd::String>() == value);
    CHECK_EQ(std::string{lit10}, R"("\"")");

    value = "Lorem Ipsum is simply dummy text of the printing and typesetting industry.\n Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book.";
    auto lit11 = Literal::make_typed(value, type_iri);
    CHECK(lit11.value<datatypes::xsd::String>() == value);

    value = "Lorem Ipsum is simply dummy text of the printing and typesetting industry.\t Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book.";
    auto lit12 = Literal::make_typed(value, type_iri);
    CHECK(lit12.value<datatypes::xsd::String>() == value);

    value = "Lorem Ipsum is simply dummy text of the printing and typesetting industry.\" Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book.";
    auto lit13 = Literal::make_typed(value, type_iri);
    CHECK(lit13.value<datatypes::xsd::String>() == value);

    value = "Lorem Ipsum is simply dummy text of the printing and typesetting industry.\\ Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book.";
    auto lit14 = Literal::make_typed(value, type_iri);
    CHECK(lit14.value<datatypes::xsd::String>() == value);

    value = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"aaaa";
    auto lit15 = Literal::make_typed(value, type_iri);
    CHECK_EQ(std::string{lit15}, R"("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"aaaa")");

    CHECK(lit1 != lit2);
    CHECK(lit2 != lit3);
    CHECK(lit1 == lit4);
    CHECK(lit2 == lit5);
}

TEST_CASE("Datatype String overread UB") {
    std::string const s = "Hello World";
    std::string_view const sv{ s.data(), 5 };

    auto const lit = Literal::make_typed(sv, IRI{datatypes::xsd::String::identifier});
    CHECK(lit.value<datatypes::xsd::String>() == "Hello");
}
