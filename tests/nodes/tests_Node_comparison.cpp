#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

namespace rdf4cpp::rdf::datatypes::registry {

constexpr static registry::ConstexprString Incomparable{"Incomparable"};

// Z
template<>
struct DatatypeMapping<Incomparable> {
    using cpp_datatype = double;
};

template<>
inline capabilities::Default<Incomparable>::cpp_type capabilities::Default<Incomparable>::from_string(std::string_view s) {
    double value;
    auto const parse_res = std::from_chars(s.data(), s.data() + s.size(), value);

    if (parse_res.ptr != s.data() + s.size()) {
        throw std::runtime_error("XSD Parsing Error");
    } else {
        return value;
    }
}

} // rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {

using Incomparable = registry::LiteralDatatypeImpl<registry::Incomparable>;

} // rdf4cpp::rdf::datatypes::xsd



TEST_SUITE("comparisions") {
    using namespace datatypes::xsd;

    TEST_CASE("Ordering") {
        IRI iri1 = IRI{"http://www.example.org/test2"};
        IRI iri2 = IRI{"http://www.example.org/test1"};
        IRI iri3 = IRI{"http://www.example.org/oest"};
        Literal lit1 = Literal::make<String>("testlit1");
        Literal lit2 = Literal::make<String>("testlit2");
        CHECK(iri2 < iri1);
        CHECK(iri3 < iri1);
        CHECK(iri3 < iri2);
        CHECK(iri1 < lit1);
        CHECK(lit1 < lit2);
    }

    TEST_CASE("filter compare tests") {
        SUBCASE("nulls") {
            CHECK(Literal{} <=> Literal{} == std::partial_ordering::equivalent);
            CHECK(Literal{} <=> Literal::make<Int>(1) == std::partial_ordering::unordered);
            CHECK(Literal::make<Decimal>(1.0) <=> Literal{} == std::partial_ordering::unordered);
        }

        SUBCASE("inconvertibility") {
            CHECK(Literal::make<String>("hello") <=> Literal::make<Int>(5) == std::partial_ordering::unordered);
            CHECK(Literal::make<Float>(1.f) <=> Literal::make<String>("world") == std::partial_ordering::unordered);
        }

        SUBCASE("incomparability") {
            CHECK(Literal::make<Incomparable>(1) <=> Literal::make<Incomparable>(1) == std::partial_ordering::unordered);
            CHECK(Literal::make<Int>(1) <=> Literal::make<Incomparable>(1) == std::partial_ordering::unordered);
            CHECK(Literal::make<Incomparable>(1) <=> Literal::make<Int>(1) == std::partial_ordering::unordered);
        }

        SUBCASE("conversion") {
            CHECK(Literal::make<Int>(1) <=> Literal::make<Integer>(10) == std::partial_ordering::less);
            CHECK(Literal::make<Integer>(0) <=> Literal::make<Float>(1.2f) == std::partial_ordering::less);
            CHECK(Literal::make<Float>(1.f) <=> Literal::make<Decimal>(1.0) == std::partial_ordering::equivalent);
        }
    }

    TEST_CASE("order by compare tests") {
        // Incomparable <=> Incomparable
        SUBCASE("incomparability") {
            CHECK(Literal::make<Incomparable>(5).compare_with_extensions(Literal::make<Incomparable>(10)) == std::weak_ordering::equivalent);

            // reason: "http://www.w3.org/2001/XMLSchema#float" > "Incomparable" (with ascii lexicographical compare)
            CHECK(Literal::make<Float>(10.f).compare_with_extensions(Literal::make<Incomparable>(1)) == std::weak_ordering::greater);

            // reason: "Incomparable" < "http://www.w3.org/2001/XMLSchema#decimal" (with ascii lexicographical compare)
            CHECK(Literal::make<Incomparable>(1).compare_with_extensions(Literal::make<Decimal>(10.0)) == std::weak_ordering::less);
        }

        SUBCASE("nulls") {
            CHECK(Literal{}.compare_with_extensions(Literal{}) == std::weak_ordering::equivalent);

            // null <=> other (expecting null < any other a)
            CHECK(Literal{}.compare_with_extensions(Literal::make<Int>(1)) == std::weak_ordering::less);
            CHECK(Literal::make<String>("123").compare_with_extensions(Literal{}) == std::weak_ordering::greater);
        }

        SUBCASE("test type ordering extensions") {
            // expected: decimal < float < int < integer < string

            CHECK(Literal::make<Decimal>(1.0).compare_with_extensions(Literal::make<Float>(1)) == std::weak_ordering::less);
            CHECK(Literal::make<Decimal>(1.0).compare_with_extensions(Literal::make<Int>(1)) == std::weak_ordering::less);
            CHECK(Literal::make<Decimal>(1.0).compare_with_extensions(Literal::make<Integer>(1)) == std::weak_ordering::less);
            CHECK(Literal::make<Decimal>(1.0).compare_with_extensions(Literal::make<String>("hello")) == std::weak_ordering::less);

            CHECK(Literal::make<Float>(1.f).compare_with_extensions(Literal::make<Int>(1)) == std::weak_ordering::less);
            CHECK(Literal::make<Float>(1.f).compare_with_extensions(Literal::make<Integer>(1)) == std::weak_ordering::less);
            CHECK(Literal::make<Float>(1.f).compare_with_extensions(Literal::make<String>("hello")) == std::weak_ordering::less);

            CHECK(Literal::make<Int>(1).compare_with_extensions(Literal::make<Integer>(1)) == std::weak_ordering::less);
            CHECK(Literal::make<Int>(1).compare_with_extensions(Literal::make<String>("hello")) == std::weak_ordering::less);
        }

        SUBCASE("test ordering extensions ignored when not equal") {
            CHECK(Literal::make<Float>(2.f).compare_with_extensions(Literal::make<Integer>(1)) == std::weak_ordering::greater);
        }
    }
}
