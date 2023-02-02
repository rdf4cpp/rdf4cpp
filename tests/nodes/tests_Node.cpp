#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <charconv>
#include <set>
#include <unordered_set>

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

namespace rdf4cpp::rdf::datatypes::registry {

constexpr static util::ConstexprString Incomparable{"Incomparable"};

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
        Literal lit1 = Literal::make_typed<String>("testlit1");
        Literal lit2 = Literal::make_typed<String>("testlit2");
        CHECK(iri2 < iri1);
        CHECK(iri3 < iri1);
        CHECK(iri3 < iri2);
        CHECK(iri1 < lit1);
        CHECK(lit1 < lit2);
    }

    TEST_CASE("filter compare tests") {
        SUBCASE("nulls") {
            CHECK(Literal{} <=> Literal{} == std::partial_ordering::equivalent);
            CHECK(Literal{} <=> Literal::make_typed<Int>(1) == std::partial_ordering::unordered);
            CHECK(Literal::make_typed<Decimal>(1.0) <=> Literal{} == std::partial_ordering::unordered);
        }

        SUBCASE("inconvertibility") {
            CHECK(Literal::make_typed<String>("hello") <=> Literal::make_typed<Int>(5) == std::partial_ordering::unordered);
            CHECK(Literal::make_typed<Float>(1.f) <=> Literal::make_typed<String>("world") == std::partial_ordering::unordered);
        }

        SUBCASE("incomparability") {
            CHECK(Literal::make_typed<Incomparable>(1) <=> Literal::make_typed<Incomparable>(2) == std::partial_ordering::unordered);
            CHECK(Literal::make_typed<Incomparable>(1) <=> Literal::make_typed<Incomparable>(1) == std::partial_ordering::equivalent); // exactly the same object so still equal
            CHECK(Literal::make_typed<Int>(1) <=> Literal::make_typed<Incomparable>(1) == std::partial_ordering::unordered);
            CHECK(Literal::make_typed<Incomparable>(1) <=> Literal::make_typed<Int>(1) == std::partial_ordering::unordered);
        }

        SUBCASE("conversion") {
            CHECK(Literal::make_typed<Int>(1) <=> Literal::make_typed<Integer>(10) == std::partial_ordering::less);
            CHECK(Literal::make_typed<Integer>(0) <=> Literal::make_typed<Float>(1.2f) == std::partial_ordering::less);
            CHECK(Literal::make_typed<Float>(1.f) <=> Literal::make_typed<Decimal>(1.0) == std::partial_ordering::equivalent);
        }
    }

    TEST_CASE("order by compare tests") {
        // Incomparable <=> Incomparable
        SUBCASE("incomparability") {
            CHECK(Literal::make_typed<Incomparable>(5).compare_with_extensions(Literal::make_typed<Incomparable>(10)) == std::weak_ordering::greater);

            // reason: float has fixed id and any fixed id type is always less than a dynamic one
            CHECK(Literal::make_typed<Float>(10.f).compare_with_extensions(Literal::make_typed<Incomparable>(1)) == std::weak_ordering::less);

            // reason: decimal has fixed id and any fixed id type is always less than a dynamic one
            CHECK(Literal::make_typed<Incomparable>(1).compare_with_extensions(Literal::make_typed<Decimal>(10.0)) == std::weak_ordering::greater);
        }

        SUBCASE("nulls") {
            CHECK(Literal{}.compare_with_extensions(Literal{}) == std::weak_ordering::equivalent);

            // null <=> other (expecting null < any other a)
            CHECK(Literal{}.compare_with_extensions(Literal::make_typed<Int>(1)) == std::weak_ordering::less);
            CHECK(Literal::make_typed<String>("123").compare_with_extensions(Literal{}) == std::weak_ordering::greater);
        }

        SUBCASE("test type ordering extensions") {
            // expected: string < float < decimal < integer < int

            CHECK(Literal::make_typed<Decimal>(1.0).compare_with_extensions(Literal::make_typed<Float>(1)) == std::weak_ordering::greater);
            CHECK(Literal::make_typed<Decimal>(1.0).compare_with_extensions(Literal::make_typed<Int>(1)) == std::weak_ordering::less);
            CHECK(Literal::make_typed<Decimal>(1.0).compare_with_extensions(Literal::make_typed<Integer>(1)) == std::weak_ordering::less);
            CHECK(Literal::make_typed<Decimal>(1.0).compare_with_extensions(Literal::make_typed<String>("hello")) == std::weak_ordering::greater);

            CHECK(Literal::make_typed<Float>(1.f).compare_with_extensions(Literal::make_typed<Int>(1)) == std::weak_ordering::less);
            CHECK(Literal::make_typed<Float>(1.f).compare_with_extensions(Literal::make_typed<Integer>(1)) == std::weak_ordering::less);
            CHECK(Literal::make_typed<Float>(1.f).compare_with_extensions(Literal::make_typed<String>("hello")) == std::weak_ordering::greater);

            CHECK(Literal::make_typed<Int>(1).compare_with_extensions(Literal::make_typed<Integer>(1)) == std::weak_ordering::greater);
            CHECK(Literal::make_typed<Int>(1).compare_with_extensions(Literal::make_typed<String>("hello")) == std::weak_ordering::greater);
        }

        SUBCASE("test ordering extensions ignored when not equal") {
            CHECK(Literal::make_typed<Float>(2.f).compare_with_extensions(Literal::make_typed<Integer>(1)) == std::weak_ordering::greater);
        }
    }

    TEST_CASE("std::set") {
        Literal const lit = Literal::make_typed<Int>(5);
        Literal const lit2 = Literal::make_typed<Int>(5); // duplicate to check if it's filtered out
        Literal const lit3 = Literal::make_typed<Integer>(10);
        Literal const lit4 = Literal::make_typed<String>("hello world");
        IRI const iri{"some random iri"};
        IRI const null_iri{};
        Node const node{};
        BlankNode const blank_node{"some_random_id"};
        query::Variable const variable{"a_variable"};

        std::set<Node> const s{
                lit, iri, node, lit2, blank_node, null_iri, lit3, variable, lit4};

        std::vector<Node> const v{s.begin(), s.end()};

        // - lexical order of types, and null smallest
        // - null Node has type BNode
        // - literal comparison
        std::vector<Node> const expected{node, null_iri, blank_node, iri, lit4, lit, lit3, variable};

        CHECK(v == expected);
    }

    TEST_CASE("std::unordered_set") {
        Literal const lit = Literal::make_typed<Int>(5);
        Literal const lit2 = Literal::make_typed<Int>(5); // duplicate to check if it's filtered out
        Literal const lit3 = Literal::make_typed<Integer>(10);
        Literal const lit4 = Literal::make_typed<String>("hello world");
        IRI const iri{"some random iri"};
        IRI const null_iri{};
        Node const node{};
        BlankNode const blank_node{"some_random_id"};
        query::Variable const variable{"a_variable"};

        std::unordered_set<Node> s{
                iri, null_iri, lit, lit2, lit3, lit4, node, blank_node, variable};

        CHECK(s.size() == 8);
        CHECK(s.contains(lit));
        CHECK(s.contains(lit2));
        CHECK(s.contains(lit3));
        CHECK(s.contains(lit4));
        CHECK(s.contains(iri));
        CHECK(s.contains(null_iri));
        CHECK(s.contains(node));
        CHECK(s.contains(blank_node));
        CHECK(s.contains(variable));
    }
}

TEST_CASE("effective boolean value") {
    Node const iri = IRI{"http://hello.com"};
    Node const bnode = BlankNode{"asd"};
    Node const var = query::Variable{"x"};
    Node const falsy_lit = Literal::make_typed<datatypes::xsd::Float>(0.f);
    Node const truthy_lit = Literal::make_typed<datatypes::xsd::Integer>(100);
    Node const null_lit = Literal{};
    Node const null_bnode = BlankNode{};

    CHECK(iri.ebv() == util::TriBool::Err);
    CHECK(bnode.ebv() == util::TriBool::Err);
    CHECK(var.ebv() == util::TriBool::Err);
    CHECK(falsy_lit.ebv() == util::TriBool::False);
    CHECK(truthy_lit.ebv() == util::TriBool::True);
    CHECK(null_lit.ebv() == util::TriBool::Err);
    CHECK(null_bnode.ebv() == util::TriBool::Err);
}
