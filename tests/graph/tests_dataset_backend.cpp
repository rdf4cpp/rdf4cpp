#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "rdf4cpp.hpp"

using namespace rdf4cpp::rdf;

TEST_CASE("base functions") {
    Dataset set{};
    CHECK((set.begin() == set.end()));
    set.add(Quad{IRI{"www.example.com/sub"}, IRI{"www.example.com/pred"}, IRI{"www.example.com/obj"}});
    set.add(Quad{IRI{"www.example.com/graph"}, IRI{"www.example.com/sub"}, IRI{"www.example.com/pred"}, Literal::make_simple("some string")});
    SUBCASE("simple functions") {
        CHECK((set.begin() != set.end()));
        for (const auto &q : set) {
            CHECK(q.subject() == IRI{"www.example.com/sub"});
            CHECK(q.predicate() == IRI{"www.example.com/pred"});
            if (q.object().is_iri())
                CHECK(q.object() == IRI{"www.example.com/obj"});
            else if (q.object().is_literal())
                CHECK(q.object() == Literal::make_simple("some string"));
            else
                CHECK(false);
            CHECK((q.graph() == IRI{"www.example.com/graph"} || q.graph() == IRI{""}));
        }
        CHECK(set.size() == 2);
        CHECK(set.size(IRI{"www.example.com/graph"}) == 1);
        CHECK(set.contains(Quad{IRI{"www.example.com/sub"}, IRI{"www.example.com/pred"}, IRI{"www.example.com/obj"}}));
        CHECK(!set.contains(Quad{IRI{"www.example.com/sub"}, IRI{"www.example.com/pred2"}, IRI{"www.example.com/obj"}}));
    }

    SUBCASE("empty pattern") {
        query::QuadPattern pattern{query::Variable("g"), query::Variable("x"), IRI{"www.example.com/pred2"}, query::Variable{"z"}};
        auto sol = set.match(pattern);
        CHECK((sol.begin() == sol.end()));
    }

    SUBCASE("pattern") {
        query::QuadPattern pattern{query::Variable("g"), query::Variable("x"), IRI{"www.example.com/pred"}, query::Variable{"z"}};
        auto sol = set.match(pattern);
        int i = 0;
        for (const auto& s : sol) {
            CHECK(s.bound_count() == 3);
            ++i;
        }
        CHECK(i == 2);
    }
}
