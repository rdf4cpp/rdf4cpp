#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

#include <tuple>

using namespace rdf4cpp::rdf::util;

TEST_SUITE("blank node id management") {
    TEST_CASE("blank node id generation") {
        SUBCASE("from entropy") {
            auto gen = BlankNodeIdGenerator::from_entropy();

            auto const id1 = gen.generate_id();
            auto const id2 = gen.generate_id();
            auto const id3 = gen.generate_id();

            CHECK(id1 != id2);
            CHECK(id2 != id3);
            CHECK(id1 != id3);

            auto gen2 = BlankNodeIdGenerator::from_entropy();

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

            auto gen = BlankNodeIdGenerator::from_seed(seed);

            auto const id1 = gen.generate_id();
            auto const id2 = gen.generate_id();
            auto const id3 = gen.generate_id();

            CHECK(id1 != id2);
            CHECK(id2 != id3);
            CHECK(id1 != id3);

            auto gen2 = BlankNodeIdGenerator::from_seed(seed);

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
        auto &mng = BlankNodeIdManager::default_instance();

        auto b1 = mng.get_or_generate("abc");
        auto b2 = mng.get_or_generate("bcd");
        auto b3 = mng.get_or_generate("abc");

        CHECK(b1 != b2);
        CHECK(b1 == b3);

        CHECK(mng.try_get("hello").null());

        auto fresh = mng.generate();
        CHECK(fresh != b1);
        CHECK(fresh != b2);
    }
}
