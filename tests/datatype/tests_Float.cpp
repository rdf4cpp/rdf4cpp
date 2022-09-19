#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("float capabilities") {
    static_assert(datatypes::LiteralDatatype<datatypes::xsd::Float>);
    static_assert(datatypes::NumericLiteralDatatype<datatypes::xsd::Float>);
    static_assert(datatypes::LogicalLiteralDatatype<datatypes::xsd::Float>);
    static_assert(!datatypes::PromotableLiteralDatatype<datatypes::xsd::Float>); // float is end of promotion hierarchy for now, so not promotable
    static_assert(!datatypes::SubtypedLiteralDatatype<datatypes::xsd::Float>);
    static_assert(datatypes::ComparableLiteralDatatype<datatypes::xsd::Float>);
    static_assert(datatypes::FixedIdLiteralDatatype<datatypes::xsd::Float>);
}
