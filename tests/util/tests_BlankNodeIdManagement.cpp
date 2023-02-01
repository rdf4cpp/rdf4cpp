#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

#include <tuple>

using namespace rdf4cpp::rdf::util;
using namespace rdf4cpp::rdf;

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
        {
            auto scope = BlankNodeIdGenerator::default_instance().scope();

            auto b1 = scope.get_or_generate_bnode("abc");
            auto b2 = scope.get_or_generate_bnode("bcd");
            auto b3 = scope.get_or_generate_bnode("abc");

            CHECK(b1 != b2);
            CHECK(b1 == b3);

            CHECK(scope.try_get_bnode("hello").null());

            auto fresh = scope.generate_bnode();
            CHECK(fresh != b1);
            CHECK(fresh != b2);

            BlankNode sibling_1;
            BlankNode sibling_2;

            {
                auto subscope = scope.subscope("some_graph");

                auto sb1 = subscope.get_or_generate_bnode("abc");
                CHECK(b1 == sb1);

                auto sb1_iri = subscope.get_or_generate_skolem_iri("https://skolem-iris.com#", "abc");
                CHECK(sb1_iri.identifier().starts_with("https://skolem-iris.com#"));
                CHECK(sb1.identifier() == sb1_iri.identifier().substr(sb1_iri.identifier().size() - 32));

                {
                    auto subsubscope = subscope.subscope("inner");
                    auto ssb1 = subsubscope.get_or_generate_bnode("abc");
                    CHECK(ssb1 == sb1);
                    CHECK(ssb1 == b1);
                }

                sibling_1 = subscope.get_or_generate_bnode("siblings");
            }

            {
                auto subscope = scope.subscope("other_graph");
                sibling_2 = subscope.get_or_generate_bnode("siblings");
            }

            CHECK(sibling_1 != sibling_2);
        }

        BlankNodeIdScope mng2 = BlankNodeIdGenerator::default_instance().scope();
        CHECK(mng2.try_get_bnode("abc").null());
    }
}
