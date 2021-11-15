//
// Created by kaimal on 14.11.21.
//
#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

static void test(Node graph, Node sub, Node pred, Node obj, bool result){

    auto qpattern = query::QuadPattern {graph, sub, pred, obj};

    CHECK(qpattern.graph() == graph);
    CHECK(qpattern.subject() == sub);
    CHECK(qpattern.predicate() == pred);
    CHECK(qpattern.object() == obj);
    if(result) CHECK(qpattern.valid());
    else CHECK(not qpattern.valid());
}

TEST_CASE("Check for variable as graph"){

    auto variable1 = query::Variable {"G"};
    auto graph = Node{variable1};

    SUBCASE("Check for variable as subject") {

        auto variable2 = query::Variable {"s"};
        auto sub = Node{variable2};

        SUBCASE("Check for variable as predicate"){
            auto variable3 = query::Variable {"p"};
            auto pred = Node{variable3};

            SUBCASE("Check for variable as object"){
                auto variable4 = query::Variable {"o"};
                auto obj = Node{variable4};

                test(graph, sub, pred, obj, true);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode = BlankNode{};
                auto obj = Node{bnode};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri};

                test(graph, sub, pred, obj, true);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"xxxx","http://looneytunes-graph.com/en"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, true);
            }
        }
        SUBCASE("Check for iri as predicate"){
            auto iri1 = IRI {"http://looneytunes-graph.com#made_debut_appearance_in"};
            auto pred = Node{iri1};

            SUBCASE("Check for variable as object"){
                auto variable3 = query::Variable {"o"};
                auto obj = Node{variable3};

                test(graph, sub, pred, obj, true);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode = BlankNode{};
                auto obj = Node{bnode};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, true);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"xxxx","http://looneytunes-graph.com/en"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, true);
            }
        }
    }

    SUBCASE("Check for iri as subject") {

        auto iri1 = IRI {"http://looneytunes-graph.com#Bugs_Bunny"};
        auto sub = Node{iri1};

        SUBCASE("Check for variable as predicate"){
            auto variable2 = query::Variable {"p"};
            auto pred = Node{variable2};

            SUBCASE("Check for variable as object"){
                auto variable3 = query::Variable {"o"};
                auto obj = Node{variable3};

                test(graph, sub, pred, obj, true);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode = BlankNode{};
                auto obj = Node{bnode};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, true);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"xxxx","http://looneytunes-graph.com/en"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, true);
            }
        }
        SUBCASE("Check for iri as predicate"){
            auto iri2 = IRI {"http://looneytunes-graph.com#made_debut_appearance_in"};
            auto pred = Node{iri2};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, true);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode = BlankNode{};
                auto obj = Node{bnode};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri3 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri3};

                test(graph, sub, pred, obj, true);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"xxxx","http://looneytunes-graph.com/en"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, true);
            }
        }
    }

    SUBCASE("Check for blank node as subject") {

        auto bnode = BlankNode {};
        auto sub = Node{bnode};

        SUBCASE("Check for variable as predicate"){
            auto variable2 = query::Variable {"p"};
            auto pred = Node{variable2};

            SUBCASE("Check for variable as object"){
                auto variable3 = query::Variable {"o"};
                auto obj = Node{variable3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode1 = BlankNode{};
                auto obj = Node{bnode1};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri1 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri1};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"xxxx","http://looneytunes-graph.com/en"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for iri as predicate"){
            auto iri1 = IRI {"http://looneytunes-graph.com#made_debut_appearance_in"};
            auto pred = Node{iri1};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode1 = BlankNode{};
                auto obj = Node{bnode1};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"xxxx","http://looneytunes-graph.com/en"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
    }
}

TEST_CASE("Check for iri as graph"){

    auto iri1 = IRI {"http://looneytunes-graph.com"};
    auto graph = Node{iri1};

    SUBCASE("Check for variable as subject") {

        auto variable1 = query::Variable {"s"};
        auto sub = Node{variable1};

        SUBCASE("Check for variable as predicate"){
            auto variable2 = query::Variable {"p"};
            auto pred = Node{variable2};

            SUBCASE("Check for variable as object"){
                auto variable3 = query::Variable {"o"};
                auto obj = Node{variable3};

                test(graph, sub, pred, obj, true);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode = BlankNode{};
                auto obj = Node{bnode};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, true);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"xxxx","http://looneytunes-graph.com/en"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, true);
            }
        }
        SUBCASE("Check for iri as predicate"){
            auto iri2 = IRI {"http://looneytunes-graph.com#made_debut_appearance_in"};
            auto pred = Node{iri2};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, true);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode = BlankNode{};
                auto obj = Node{bnode};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri3 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri3};

                test(graph, sub, pred, obj, true);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"xxxx","http://looneytunes-graph.com/en"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, true);
            }
        }
    }

    SUBCASE("Check for iri as subject") {

        auto iri2 = IRI {"http://looneytunes-graph.com#Bugs_Bunny"};
        auto sub = Node{iri2};

        SUBCASE("Check for variable as predicate"){
            auto variable1 = query::Variable {"p"};
            auto pred = Node{variable1};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, true);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode = BlankNode{};
                auto obj = Node{bnode};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri3 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri3};

                test(graph, sub, pred, obj, true);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"xxxx","http://looneytunes-graph.com/en"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, true);
            }
        }
        SUBCASE("Check for iri as predicate"){
            auto iri3 = IRI {"http://looneytunes-graph.com#made_debut_appearance_in"};
            auto pred = Node{iri3};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, true);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode = BlankNode{};
                auto obj = Node{bnode};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri4 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri4};

                test(graph, sub, pred, obj, true);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"xxxx","http://looneytunes-graph.com/en"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, true);
            }
        }
    }

    SUBCASE("Check for blank node as subject") {

        auto bnode = BlankNode {};
        auto sub = Node{bnode};

        SUBCASE("Check for variable as predicate"){
            auto variable1 = query::Variable {"p"};
            auto pred = Node{variable1};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode1 = BlankNode{};
                auto obj = Node{bnode1};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"xxxx","http://looneytunes-graph.com/en"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for iri as predicate"){
            auto iri2 = IRI {"http://looneytunes-graph.com#made_debut_appearance_in"};
            auto pred = Node{iri2};

            SUBCASE("Check for variable as object"){
                auto variable1 = query::Variable {"o"};
                auto obj = Node{variable1};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode1 = BlankNode{};
                auto obj = Node{bnode1};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri3 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"xxxx","http://looneytunes-graph.com/en"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
    }
}

