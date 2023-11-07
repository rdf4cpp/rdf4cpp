#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

#include <tuple>

using namespace rdf4cpp::rdf::util;
using namespace rdf4cpp::rdf;

TEST_SUITE("blank node id management") {
    TEST_CASE("blank node id generation") {
        SUBCASE("from entropy") {
            auto gen = NodeGenerator::new_instance();

            auto const id1 = gen.generate_id();
            auto const id2 = gen.generate_id();
            auto const id3 = gen.generate_id();

            CHECK(id1 != id2);
            CHECK(id2 != id3);
            CHECK(id1 != id3);

            auto gen2 = NodeGenerator::new_instance();

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

            auto gen = NodeGenerator::new_instance_with_generator<RandomIdGenerator>(seed);

            auto const id1 = gen.generate_id();
            auto const id2 = gen.generate_id();
            auto const id3 = gen.generate_id();

            CHECK(id1 != id2);
            CHECK(id2 != id3);
            CHECK(id1 != id3);

            auto gen2 = NodeGenerator::new_instance_with_generator<RandomIdGenerator>(seed);

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
            NodeScope scope = NodeScope::new_instance();

            auto b1 = scope.get_or_generate_node("abc");
            auto b2 = scope.get_or_generate_node("bcd");
            auto b3 = scope.get_or_generate_node("abc");

            CHECK(b1 != b2);
            CHECK(b1 == b3);

            CHECK(scope.try_get_node("hello").null());

            auto fresh = scope.generate_node();
            CHECK(fresh != b1);
            CHECK(fresh != b2);
        }

        NodeScope mng2 = NodeScope::new_instance();
        CHECK(mng2.try_get_node("abc").null());
    }

    TEST_CASE("union and merge semantics") {
        auto &generator = NodeGenerator::default_instance();

        BlankNode b1_1;
        BlankNode b1_2;

        {
            NodeScope scope_1 = NodeScope::new_instance();
            NodeScope scope_2 = NodeScope::new_instance();

            b1_1 = scope_1.get_or_generate_node("b1", generator).as_blank_node();
            b1_2 = scope_2.get_or_generate_node("b1", generator).as_blank_node();

            CHECK(!b1_1.merge_eq(b1_2));
            CHECK(b1_1.union_eq(b1_2) == util::TriBool::True);
        }

        CHECK(!b1_1.merge_eq(b1_2));
        CHECK(b1_1.union_eq(b1_2) == util::TriBool::Err);
    }
}
