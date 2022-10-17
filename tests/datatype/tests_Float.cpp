#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

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

TEST_CASE("float serializing") {
    using namespace datatypes::xsd;

    std::array<char, 1024> large_buf;
    SUBCASE("float") {
        auto value = std::numeric_limits<Float::cpp_type>::min();
        std::to_chars_result const res = std::to_chars(large_buf.data(), large_buf.data() + large_buf.size(), value, std::chars_format::fixed);

        auto lit1 = Literal::make<Float>(value);
        CHECK(lit1.lexical_form() == std::string_view{large_buf.data(), res.ptr});
    }

    SUBCASE("double") {
        auto value = std::numeric_limits<Double::cpp_type>::min();
        std::to_chars_result const res = std::to_chars(large_buf.data(), large_buf.data() + large_buf.size(), value, std::chars_format::fixed);

        auto lit1 = Literal::make<Double>(value);
        CHECK(lit1.lexical_form() == std::string_view{large_buf.data(), res.ptr});
    }
}
