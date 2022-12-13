#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

#include <limits>

using namespace rdf4cpp::rdf;

TEST_SUITE("inlining util") {
    using namespace datatypes::registry::util;

    TEST_CASE("unsigned smaller into bigger") {
        auto const smaller = std::numeric_limits<uint32_t>::max();
        auto const packed = pack<uint64_t>(smaller);

        CHECK(packed < (1ul << 32));

        auto const unpacked = unpack<uint32_t>(packed);
        CHECK(unpacked == smaller);
    }

    TEST_CASE("signed smaller into bigger") {
        auto const smaller = std::numeric_limits<int32_t>::min();
        auto const packed = pack<uint64_t>(smaller);

        CHECK(packed < (1ul << 32));

        auto const unpacked = unpack<int32_t>(packed);
        CHECK(unpacked == smaller);
    }

    TEST_CASE("unsigned bigger into smaller") {
        auto const bigger = static_cast<uint64_t>(std::numeric_limits<uint32_t>::max());
        auto const packed = pack<uint32_t>(bigger);

        auto const unpacked = unpack<uint64_t>(packed);
        CHECK(bigger == unpacked);
    }

    // TODO?: this does not work, do we want it work? (usecase: negative 64bit number into 42bit LiteralID)
    //TEST_CASE("signed bigger into smaller") {
    //    int64_t const bigger = -256l;
    //    auto const packed = pack<uint32_t>(bigger);
    //
    //    auto const unpacked = unpack<int64_t>(packed);
    //    CHECK(bigger == unpacked);
    //}
}

TEST_SUITE("literal inlining") {
    using namespace datatypes;

    TEST_CASE("32bit positive int inlining") {
        auto const i = std::numeric_limits<xsd::Int::cpp_type>::max();
        auto const lit1 = Literal::make<xsd::Int>(i);
        auto const lit2 = Literal::make(std::to_string(i), IRI{xsd::Int::identifier});
        CHECK(lit1.backend_handle().is_inlined());
        CHECK(lit2.backend_handle().is_inlined());
        CHECK(lit1 == lit2);

        auto const extracted1 = lit1.template value<xsd::Int>();
        auto const extracted2 = lit2.value();
        CHECK(extracted1 == i);
        CHECK(std::any_cast<xsd::Int::cpp_type>(extracted2) == i);
    }

    TEST_CASE("32bit negative int inlining") {
        auto const i = std::numeric_limits<xsd::Int::cpp_type>::min();
        auto const lit1 = Literal::make<xsd::Int>(i);
        auto const lit2 = Literal::make(std::to_string(i), IRI{xsd::Int::identifier});
        CHECK(lit1.backend_handle().is_inlined());
        CHECK(lit2.backend_handle().is_inlined());
        CHECK(lit1 == lit2);

        auto const extracted1 = lit1.template value<xsd::Int>();
        auto const extracted2 = lit2.value();
        CHECK(extracted1 == i);
        CHECK(std::any_cast<xsd::Int::cpp_type>(extracted2) == i);
    }

    TEST_CASE("small 64bit positive int inlining") {
        auto const i = 1l << (storage::node::identifier::LiteralID::width - 1);
        auto const lit1 = Literal::make<xsd::Long>(i);
        auto const lit2 = Literal::make(std::to_string(i), IRI{xsd::Long::identifier});
        CHECK(lit1.backend_handle().is_inlined());
        CHECK(lit2.backend_handle().is_inlined());
        CHECK(lit1 == lit2);

        auto const extracted1 = lit1.template value<xsd::Long>();
        auto const extracted2 = lit2.value();
        CHECK(extracted1 == i);
        CHECK(std::any_cast<xsd::Long::cpp_type>(extracted2) == i);
    }

    TEST_CASE("negative 64bit int not-inlining") {
        auto const i = -256;
        auto const lit1 = Literal::make<xsd::Long>(i);
        auto const lit2 = Literal::make(std::to_string(i), IRI{xsd::Long::identifier});
        CHECK(!lit1.backend_handle().is_inlined()); // cannot be inlined bits left of 42bit boundary would need to be set
        CHECK(!lit2.backend_handle().is_inlined());
        CHECK(lit1 == lit2);
    }
}
