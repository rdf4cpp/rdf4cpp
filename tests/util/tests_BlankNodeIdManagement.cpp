#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

#include <tuple>

using namespace rdf4cpp::rdf::util;
using namespace rdf4cpp::rdf;

TEST_SUITE("blank node id management") {
    TEST_CASE("blank node id generation") {
        SUBCASE("from entropy") {
            auto gen = NodeGenerator::with_backend<RandomIdGenerator>();

            auto const id1 = gen.generate_id();
            auto const id2 = gen.generate_id();
            auto const id3 = gen.generate_id();

            CHECK(id1 != id2);
            CHECK(id2 != id3);
            CHECK(id1 != id3);

            auto gen2 = NodeGenerator::with_backend<RandomIdGenerator>();

            auto const id4 = gen.generate_id();
            auto const id5 = gen.generate_id();
            auto const id6 = gen.generate_id();

            CHECK(id4 != id5);
            CHECK(id5 != id6);
            CHECK(id4 != id6);

            CHECK(std::tie(id1, id2, id3) != std::tie(id4, id5, id6));
        }

        SUBCASE("from seed") {
            auto const seed = 42;

            auto gen = NodeGenerator::with_backend<RandomIdGenerator>(seed);

            auto const id1 = gen.generate_id();
            auto const id2 = gen.generate_id();
            auto const id3 = gen.generate_id();

            CHECK(id1 != id2);
            CHECK(id2 != id3);
            CHECK(id1 != id3);

            auto gen2 = NodeGenerator::with_backend<RandomIdGenerator>(seed);

            auto const id4 = gen2.generate_id();
            auto const id5 = gen2.generate_id();
            auto const id6 = gen2.generate_id();

            CHECK(id4 != id5);
            CHECK(id5 != id6);
            CHECK(id4 != id6);

            CHECK(std::tie(id1, id2, id3) == std::tie(id4, id5, id6));
        }
    }

    TEST_CASE("blank node id manager") {
        {
            auto scope = NodeGenerator::default_instance().scope();

            auto b1 = scope.get_or_generate_node("abc");
            auto b2 = scope.get_or_generate_node("bcd");
            auto b3 = scope.get_or_generate_node("abc");

            CHECK(b1 != b2);
            CHECK(b1 == b3);

            CHECK(scope.try_get_node("hello").null());

            auto fresh = scope.generate_node();
            CHECK(fresh != b1);
            CHECK(fresh != b2);

            Node sibling_1;
            Node sibling_2;

            {
                auto subscope = scope.subscope("some_graph");

                auto sb1 = subscope.get_or_generate_node("abc");
                CHECK(b1 == sb1);

                {
                    auto subsubscope = subscope.subscope("inner");
                    auto ssb1 = subsubscope.get_or_generate_node("abc");
                    CHECK(ssb1 == sb1);
                    CHECK(ssb1 == b1);
                }

                sibling_1 = subscope.get_or_generate_node("siblings");
            }

            {
                auto subscope = scope.subscope("other_graph");
                sibling_2 = subscope.get_or_generate_node("siblings");
            }

            CHECK(sibling_1 != sibling_2);
        }

        NodeScope mng2 = NodeGenerator::default_instance().scope();
        CHECK(mng2.try_get_node("abc").null());
    }

    TEST_CASE("empty subscope id") {
        NodeScope scope = NodeGenerator::default_instance().scope();
        NodeScope subscope = scope.subscope("");

        auto b1 = scope.get_or_generate_node("aaa");
        auto b2 = subscope.get_or_generate_node("aaa");

        CHECK(b1 == b2);
    }

    TEST_CASE("merge semantics") {
        BlankNode b1_1;
        BlankNode b1_2;

        {
            NodeScope scope = NodeGenerator::default_instance().scope();

            auto sub1 = scope.subscope("a");
            auto sub2 = scope.subscope("b");

            b1_1 = static_cast<BlankNode>(sub1.get_or_generate_node("b1"));
            b1_2 = static_cast<BlankNode>(sub2.get_or_generate_node("b1"));

            CHECK(!b1_1.merge_eq(b1_2));
            CHECK(b1_1.union_eq(b1_2) == true);
        }

        CHECK(!b1_1.merge_eq(b1_2));
        CHECK(b1_1.union_eq(b1_2) == std::nullopt);
    }
}
