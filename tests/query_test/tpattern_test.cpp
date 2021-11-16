//
// Created by kaimal on 14.11.21.
//
#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

static void test(Node sub, Node pred, Node obj, bool result){
    auto tpattern = query::TriplePattern {sub, pred, obj};

    CHECK(tpattern.subject() == sub);
    CHECK(tpattern.predicate() == pred);
    CHECK(tpattern.object() == obj);
    if (result) CHECK(tpattern.valid());
    else CHECK(not tpattern.valid());

    //CHECK(tpattern.begin() == &sub);
    //CHECK(tpattern.end() == &obj);
}

TEST_CASE("TriplePattern - Check for variable as subject") {

    auto variable1 = query::Variable {"s"};
    auto sub = Node{variable1};

    SUBCASE("Check for variable as predicate"){
        auto variable2 = query::Variable {"p"};
        auto pred = Node{variable2};

        SUBCASE("Check for variable as object"){
            auto variable3 = query::Variable {"o"};
            auto obj = Node{variable3};

            test(sub, pred, obj, true);
        }

        SUBCASE("Check for blank node as object"){
            auto bnode = BlankNode{};
            auto obj = Node{bnode};

            test(sub, pred, obj, true);
        }

        SUBCASE("Check for iri as object"){
            auto iri = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
            auto obj = Node{iri};

            test(sub, pred, obj, true);
        }

        SUBCASE("Check for literal as object"){
            auto literal = Literal{"xxxx","http://looneytunes-graph.com/en"};
            auto obj = Node{literal};

            test(sub, pred, obj, true);
        }
    }
    SUBCASE("Check for iri as predicate"){
        auto iri = IRI {"http://looneytunes-graph.com#made_debut_appearance_in"};
        auto pred = Node{iri};

        SUBCASE("Check for variable as object"){
            auto variable2 = query::Variable {"o"};
            auto obj = Node{variable2};

            test(sub, pred, obj, true);
        }

        SUBCASE("Check for blank node as object"){
            auto bnode = BlankNode{};
            auto obj = Node{bnode};

            test(sub, pred, obj, true);
        }

        SUBCASE("Check for iri as object"){
            auto iri1 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
            auto obj = Node{iri1};

            test(sub, pred, obj, true);
        }

        SUBCASE("Check for literal as object"){
            auto literal = Literal{"xxxx","http://looneytunes-graph.com/en"};
            auto obj = Node{literal};

            test(sub, pred, obj, true);
        }
    }
    SUBCASE("Check for blanknode as predicate"){
        auto bnode = BlankNode{};
        auto pred = Node{bnode};

        SUBCASE("Check for variable as object"){
            auto variable2 = query::Variable {"o"};
            auto obj = Node{variable2};

            test(sub, pred, obj, false);
        }

        SUBCASE("Check for blank node as object"){
            auto bnode1 = BlankNode{};
            auto obj = Node{bnode1};

            test(sub, pred, obj, false);
        }

        SUBCASE("Check for iri as object"){
            auto iri1 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
            auto obj = Node{iri1};

            test(sub, pred, obj, false);
        }

        SUBCASE("Check for literal as object"){
            auto literal = Literal{"xxxx","http://looneytunes-graph.com/en"};
            auto obj = Node{literal};

            test(sub, pred, obj, false);
        }
    }

}

TEST_CASE("TriplePattern - Check for blank node as subject") {

    using namespace rdf4cpp::rdf;

    auto bnode = BlankNode {};
    auto sub = Node{bnode};

    SUBCASE("Check for variable as predicate"){
        auto variable2 = query::Variable {"p"};
        auto pred = Node{variable2};

        SUBCASE("Check for variable as object"){
            auto variable3 = query::Variable {"o"};
            auto obj = Node{variable3};

            test(sub, pred, obj, true);
        }

        SUBCASE("Check for blank node as object"){
            auto bnode1 = BlankNode{};
            auto obj = Node{bnode1};

            test(sub, pred, obj, true);
        }

        SUBCASE("Check for iri as object"){
            auto iri = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
            auto obj = Node{iri};

            test(sub, pred, obj, true);
        }

        SUBCASE("Check for literal as object"){
            auto literal = Literal{"xxxx","http://looneytunes-graph.com/en"};
            auto obj = Node{literal};

            test(sub, pred, obj, true);
        }
    }
    SUBCASE("Check for iri as predicate"){
        auto iri = IRI {"http://looneytunes-graph.com#made_debut_appearance_in"};
        auto pred = Node{iri};

        SUBCASE("Check for variable as object"){
            auto variable2 = query::Variable {"o"};
            auto obj = Node{variable2};

            test(sub, pred, obj, true);
        }

        SUBCASE("Check for blank node as object"){
            auto bnode1 = BlankNode{};
            auto obj = Node{bnode1};

            test(sub, pred, obj, true);
        }

        SUBCASE("Check for iri as object"){
            auto iri1 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
            auto obj = Node{iri1};

            test(sub, pred, obj, true);
        }

        SUBCASE("Check for literal as object"){
            auto literal = Literal{"xxxx","http://looneytunes-graph.com/en"};
            auto obj = Node{literal};

            test(sub, pred, obj, true);
        }
    }
    SUBCASE("Check for blanknode as predicate"){
        auto bnode1 = BlankNode{};
        auto pred = Node{bnode1};

        SUBCASE("Check for variable as object"){
            auto variable2 = query::Variable {"o"};
            auto obj = Node{variable2};

            test(sub, pred, obj, false);
        }

        SUBCASE("Check for blank node as object"){
            auto bnode2 = BlankNode{};
            auto obj = Node{bnode2};

            test(sub, pred, obj, false);
        }

        SUBCASE("Check for iri as object"){
            auto iri1 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
            auto obj = Node{iri1};

            test(sub, pred, obj, false);
        }

        SUBCASE("Check for literal as object"){
            auto literal = Literal{"xxxx","http://looneytunes-graph.com/en"};
            auto obj = Node{literal};

            test(sub, pred, obj, false);
        }
    }

}

TEST_CASE("TriplePattern - Check for iri as subject") {

    using namespace rdf4cpp::rdf;

    auto iri = IRI {"http://looneytunes-graph.com#Bugs_Bunny"};
    auto sub = Node{iri};

    SUBCASE("Check for variable as predicate"){
        auto variable2 = query::Variable {"p"};
        auto pred = Node{variable2};

        SUBCASE("Check for variable as object"){
            auto variable3 = query::Variable {"o"};
            auto obj = Node{variable3};

            test(sub, pred, obj, true);
        }

        SUBCASE("Check for blank node as object"){
            auto bnode = BlankNode{};
            auto obj = Node{bnode};

            test(sub, pred, obj, true);
        }

        SUBCASE("Check for iri as object"){
            auto iri = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
            auto obj = Node{iri};

            test(sub, pred, obj, true);
        }

        SUBCASE("Check for literal as object"){
            auto literal = Literal{"xxxx","http://looneytunes-graph.com/en"};
            auto obj = Node{literal};

            test(sub, pred, obj, true);
        }
    }
    SUBCASE("Check for iri as predicate"){
        auto iri = IRI {"http://looneytunes-graph.com#made_debut_appearance_in"};
        auto pred = Node{iri};

        SUBCASE("Check for variable as object"){
            auto variable2 = query::Variable {"o"};
            auto obj = Node{variable2};

            test(sub, pred, obj, true);
        }

        SUBCASE("Check for blank node as object"){
            auto bnode = BlankNode{};
            auto obj = Node{bnode};

            test(sub, pred, obj, true);
        }

        SUBCASE("Check for iri as object"){
            auto iri1 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
            auto obj = Node{iri1};

            test(sub, pred, obj, true);
        }

        SUBCASE("Check for literal as object"){
            auto literal = Literal{"xxxx","http://looneytunes-graph.com/en"};
            auto obj = Node{literal};

            test(sub, pred, obj, true);
        }
    }
    SUBCASE("Check for blanknode as predicate"){
        auto bnode = BlankNode{};
        auto pred = Node{bnode};

        SUBCASE("Check for variable as object"){
            auto variable2 = query::Variable {"o"};
            auto obj = Node{variable2};

            test(sub, pred, obj, false);
        }

        SUBCASE("Check for blank node as object"){
            auto bnode1 = BlankNode{};
            auto obj = Node{bnode1};

            test(sub, pred, obj, false);
        }

        SUBCASE("Check for iri as object"){
            auto iri1 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
            auto obj = Node{iri1};

            test(sub, pred, obj, false);
        }

        SUBCASE("Check for literal as object"){
            auto literal = Literal{"xxxx","http://looneytunes-graph.com/en"};
            auto obj = Node{literal};

            test(sub, pred, obj, false);
        }
    }

}