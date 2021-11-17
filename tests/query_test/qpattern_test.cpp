//
// Created by kaimal on 14.11.21.
//
#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

static void test(Node graph, Node sub, Node pred, Node obj, bool result){

    auto qpattern = query::QuadPattern {graph, sub, pred, obj};

    SUBCASE("Check validity & output"){
        CHECK(qpattern.graph() == graph);
        CHECK(qpattern.subject() == sub);
        CHECK(qpattern.predicate() == pred);
        CHECK(qpattern.object() == obj);

        CHECK(qpattern.valid());
        if(result) CHECK(qpattern.valid());
        else CHECK(not qpattern.valid());

        //checks whether a valid quadpattern have a valid string output
        if(qpattern.valid()) std::cout << qpattern << std::endl;
    }

    SUBCASE("Check iterators"){
        auto b_itr = qpattern.begin();
        CHECK(*b_itr++ == graph);
        CHECK(*b_itr++ == sub);
        CHECK(*b_itr++ == pred);
        CHECK(*b_itr++ == obj);

        auto e_itr = qpattern.end();
        CHECK(*(--e_itr) == obj);
        CHECK(*(--e_itr) == pred);
        CHECK(*(--e_itr)== sub);
        CHECK(*(--e_itr) == graph);
    }

    SUBCASE("Check reverse-iterators"){
        auto rb_itr = qpattern.rbegin();
        CHECK(*rb_itr++ == obj);
        CHECK(*rb_itr++ == pred);
        CHECK(*rb_itr++ == sub);
        CHECK(*rb_itr++ == graph);

        auto re_itr = qpattern.rend();
        CHECK(*(--re_itr) == graph);
        CHECK(*(--re_itr) == sub);
        CHECK(*(--re_itr)== pred);
        CHECK(*(--re_itr) == obj);
    }
}

TEST_CASE("QuadPattern - Check for iterators and const-iterators"){

    auto variable1 = query::Variable {"g"};
    auto graph = Node{variable1};

    auto variable2 = query::Variable {"s"};
    auto sub = Node{variable2};

    auto variable3 = query::Variable {"p"};
    auto pred = Node{variable3};

    auto variable4 = query::Variable {"o"};
    auto obj = Node{variable4};

    auto variable5 = query::Variable {"g1"};
    auto graph1 = Node{variable5};

    auto variable6 = query::Variable {"s1"};
    auto sub1 = Node{variable6};

    auto variable7 = query::Variable {"p1"};
    auto pred1 = Node{variable7};

    auto variable8 = query::Variable {"o1"};
    auto obj1 = Node{variable8};

    auto qpattern = query::QuadPattern {graph, sub, pred, obj};

    query::QuadPattern::iterator itr;
    for(itr = qpattern.begin(); itr != qpattern.end(); itr++){
        if(*itr == graph) *itr = graph1;
        else if(*itr == sub) *itr = sub1;
        else if(*itr == pred) *itr = pred1;
        else if(*itr == obj) *itr = obj1;
    }

    query::QuadPattern::const_reverse_iterator rb_const_itr;
    query::QuadPattern::const_reverse_iterator re_const_itr;

    SUBCASE("Check const-reverse-iterators"){
        rb_const_itr = qpattern.rbegin();
        CHECK(*rb_const_itr++ == obj1);
        CHECK(*rb_const_itr++ == pred1);
        CHECK(*rb_const_itr++ == sub1);
        CHECK(*rb_const_itr++ == graph1);

        re_const_itr = qpattern.rend();
        CHECK(*(--re_const_itr) == graph1);
        CHECK(*(--re_const_itr) == sub1);
        CHECK(*(--re_const_itr)== pred1);
        CHECK(*(--re_const_itr) == obj1);
    }

    query::QuadPattern::const_iterator b_const_itr;
    query::QuadPattern::const_iterator e_const_itr;

    SUBCASE("Check const-iterators"){
        b_const_itr = qpattern.begin();
        CHECK(*b_const_itr++ == graph1);
        CHECK(*b_const_itr++ == sub1);
        CHECK(*b_const_itr++ == pred1);
        CHECK(*b_const_itr++ == obj1);

        e_const_itr = qpattern.end();
        CHECK(*(--e_const_itr) == obj1);
        CHECK(*(--e_const_itr) == pred1);
        CHECK(*(--e_const_itr)== sub1);
        CHECK(*(--e_const_itr)== graph1);
    }
}

TEST_CASE("QuadPattern - Check for variable as graph"){

    auto variable1 = query::Variable {"g"};
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
                auto literal = Literal{"Bugs Bunny"};
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
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, true);
            }
        }
        SUBCASE("Check for blanknode as predicate"){
            auto bnode2 = BlankNode{};
            auto pred = Node{bnode2};

            SUBCASE("Check for variable as object"){
                auto variable3 = query::Variable {"o"};
                auto obj = Node{variable3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode3 = BlankNode{};
                auto obj = Node{bnode3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri3 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for literal as predicate"){
            auto literal1 = Literal{"name"};
            auto pred = Node{literal1};

            SUBCASE("Check for variable as object"){
                auto variable3 = query::Variable {"o"};
                auto obj = Node{variable3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode2 = BlankNode{};
                auto obj = Node{bnode2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri3 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal2 = Literal{"Bugs Bunny"};
                auto obj = Node{literal2};

                test(graph, sub, pred, obj, false);
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
                auto literal = Literal{"Bugs Bunny"};
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
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, true);
            }
        }
        SUBCASE("Check for blanknode as predicate"){
            auto bnode2 = BlankNode{};
            auto pred = Node{bnode2};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode3 = BlankNode{};
                auto obj = Node{bnode3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri3 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for literal as predicate"){
            auto literal1 = Literal{"name"};
            auto pred = Node{literal1};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode2 = BlankNode{};
                auto obj = Node{bnode2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri3 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal2 = Literal{"Bugs Bunny"};
                auto obj = Node{literal2};

                test(graph, sub, pred, obj, false);
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
                auto literal = Literal{"Bugs Bunny"};
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
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for blanknode as predicate"){
            auto bnode2 = BlankNode{};
            auto pred = Node{bnode2};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode3 = BlankNode{};
                auto obj = Node{bnode3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri3 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for literal as predicate"){
            auto literal1 = Literal{"name"};
            auto pred = Node{literal1};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode2 = BlankNode{};
                auto obj = Node{bnode2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri3 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal2 = Literal{"Bugs Bunny"};
                auto obj = Node{literal2};

                test(graph, sub, pred, obj, false);
            }
        }
    }
    SUBCASE("Check for literal as subject") {

        auto literal1 = Literal{"Bugs Bunny"};
        auto sub = Node{literal1};

        SUBCASE("Check for variable as predicate"){
            auto variable2 = query::Variable {"p"};
            auto pred = Node{variable2};

            SUBCASE("Check for variable as object"){
                auto variable3 = query::Variable {"o"};
                auto obj = Node{variable3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode = BlankNode{};
                auto obj = Node{bnode};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for iri as predicate"){
            auto iri2 = IRI {"http://looneytunes-graph.com#made_debut_appearance_in"};
            auto pred = Node{iri2};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode = BlankNode{};
                auto obj = Node{bnode};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri3 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for blanknode as predicate"){
            auto bnode1 = BlankNode{};
            auto pred = Node{bnode1};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode2 = BlankNode{};
                auto obj = Node{bnode2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for literal as predicate"){
            auto literal2 = Literal{"name"};
            auto pred = Node{literal2};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode = BlankNode{};
                auto obj = Node{bnode};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal3 = Literal{"Bugs Bunny"};
                auto obj = Node{literal3};

                test(graph, sub, pred, obj, false);
            }
        }
    }
}

TEST_CASE("QuadPattern - Check for iri as graph"){

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
                auto literal = Literal{"Bugs Bunny"};
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
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, true);
            }
        }
        SUBCASE("Check for blanknode as predicate"){
            auto bnode2 = BlankNode{};
            auto pred = Node{bnode2};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode3 = BlankNode{};
                auto obj = Node{bnode3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for literal as predicate"){
            auto literal1 = Literal{"name"};
            auto pred = Node{literal1};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode2 = BlankNode{};
                auto obj = Node{bnode2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal2 = Literal{"Bugs Bunny"};
                auto obj = Node{literal2};

                test(graph, sub, pred, obj, false);
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
                auto literal = Literal{"Bugs Bunny"};
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
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, true);
            }
        }
        SUBCASE("Check for blanknode as predicate"){
            auto bnode2 = BlankNode{};
            auto pred = Node{bnode2};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode3 = BlankNode{};
                auto obj = Node{bnode3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri3 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for literal as predicate"){
            auto literal1 = Literal{"name"};
            auto pred = Node{literal1};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode2 = BlankNode{};
                auto obj = Node{bnode2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri3 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal2 = Literal{"Bugs Bunny"};
                auto obj = Node{literal2};

                test(graph, sub, pred, obj, false);
            }
        }
    }
    SUBCASE("Check for blank node as subject") {

        auto bnode1 = BlankNode {};
        auto sub = Node{bnode1};

        SUBCASE("Check for variable as predicate"){
            auto variable1 = query::Variable {"p"};
            auto pred = Node{variable1};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode2 = BlankNode{};
                auto obj = Node{bnode2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"Bugs Bunny"};
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
                auto bnode2 = BlankNode{};
                auto obj = Node{bnode2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri3 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for blanknode as predicate"){
            auto bnode2 = BlankNode{};
            auto pred = Node{bnode2};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode3 = BlankNode{};
                auto obj = Node{bnode3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for literal as predicate"){
            auto literal1 = Literal{"name"};
            auto pred = Node{literal1};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode2 = BlankNode{};
                auto obj = Node{bnode2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal2 = Literal{"Bugs Bunny"};
                auto obj = Node{literal2};

                test(graph, sub, pred, obj, false);
            }
        }
    }
    SUBCASE("Check for literal as subject") {

        auto literal1 = Literal{"Bugs Bunny"};
        auto sub = Node{literal1};

        SUBCASE("Check for variable as predicate"){
            auto variable1 = query::Variable {"p"};
            auto pred = Node{variable1};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode = BlankNode{};
                auto obj = Node{bnode};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for iri as predicate"){
            auto iri2 = IRI {"http://looneytunes-graph.com#made_debut_appearance_in"};
            auto pred = Node{iri2};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode = BlankNode{};
                auto obj = Node{bnode};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri3 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for blanknode as predicate"){
            auto bnode1 = BlankNode{};
            auto pred = Node{bnode1};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode2 = BlankNode{};
                auto obj = Node{bnode2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for literal as predicate"){
            auto literal2 = Literal{"name"};
            auto pred = Node{literal2};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode = BlankNode{};
                auto obj = Node{bnode};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal3 = Literal{"Bugs Bunny"};
                auto obj = Node{literal3};

                test(graph, sub, pred, obj, false);
            }
        }
    }
}

TEST_CASE("QuadPattern - Check for blanknode as graph"){

    auto bnode = BlankNode{};
    auto graph = Node{bnode};

    SUBCASE("Check for variable as subject") {

        auto variable1 = query::Variable {"s"};
        auto sub = Node{variable1};

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
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for iri as predicate"){
            auto iri2 = IRI {"http://looneytunes-graph.com#made_debut_appearance_in"};
            auto pred = Node{iri2};

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
                auto iri3 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for blanknode as predicate"){
            auto bnode2 = BlankNode{};
            auto pred = Node{bnode2};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode3 = BlankNode{};
                auto obj = Node{bnode3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for literal as predicate"){
            auto literal1 = Literal{"name"};
            auto pred = Node{literal1};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode2 = BlankNode{};
                auto obj = Node{bnode2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal2 = Literal{"Bugs Bunny"};
                auto obj = Node{literal2};

                test(graph, sub, pred, obj, false);
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
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for iri as predicate"){
            auto iri3 = IRI {"http://looneytunes-graph.com#made_debut_appearance_in"};
            auto pred = Node{iri3};

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
                auto iri4 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri4};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for blanknode as predicate"){
            auto bnode2 = BlankNode{};
            auto pred = Node{bnode2};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode3 = BlankNode{};
                auto obj = Node{bnode3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri3 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for literal as predicate"){
            auto literal1 = Literal{"name"};
            auto pred = Node{literal1};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode2 = BlankNode{};
                auto obj = Node{bnode2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri3 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal2 = Literal{"Bugs Bunny"};
                auto obj = Node{literal2};

                test(graph, sub, pred, obj, false);
            }
        }
    }
    SUBCASE("Check for blank node as subject") {

        auto bnode1 = BlankNode {};
        auto sub = Node{bnode1};

        SUBCASE("Check for variable as predicate"){
            auto variable1 = query::Variable {"p"};
            auto pred = Node{variable1};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode2 = BlankNode{};
                auto obj = Node{bnode2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"Bugs Bunny"};
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
                auto bnode2 = BlankNode{};
                auto obj = Node{bnode2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri3 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for blanknode as predicate"){
            auto bnode2 = BlankNode{};
            auto pred = Node{bnode2};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode3 = BlankNode{};
                auto obj = Node{bnode3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for literal as predicate"){
            auto literal1 = Literal{"name"};
            auto pred = Node{literal1};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode2 = BlankNode{};
                auto obj = Node{bnode2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal2 = Literal{"Bugs Bunny"};
                auto obj = Node{literal2};

                test(graph, sub, pred, obj, false);
            }
        }
    }
    SUBCASE("Check for literal as subject") {

        auto literal1 = Literal{"Bugs Bunny"};
        auto sub = Node{literal1};

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
                auto iri = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for iri as predicate"){
            auto iri2 = IRI {"http://looneytunes-graph.com#made_debut_appearance_in"};
            auto pred = Node{iri2};

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
                auto iri3 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for blanknode as predicate"){
            auto bnode1 = BlankNode{};
            auto pred = Node{bnode1};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode2 = BlankNode{};
                auto obj = Node{bnode2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal = Literal{"Bugs Bunny"};
                auto obj = Node{literal};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for literal as predicate"){
            auto literal2 = Literal{"name"};
            auto pred = Node{literal2};

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
                auto literal3 = Literal{"Bugs Bunny"};
                auto obj = Node{literal3};

                test(graph, sub, pred, obj, false);
            }
        }
    }
}

TEST_CASE("QuadPattern - Check for literal as graph"){

    auto literal = Literal{"http://looneytunes-graph.com", "en"};
    auto graph = Node{literal};

    SUBCASE("Check for variable as subject") {

        auto variable1 = query::Variable {"s"};
        auto sub = Node{variable1};

        SUBCASE("Check for variable as predicate"){
            auto variable2 = query::Variable {"p"};
            auto pred = Node{variable2};

            SUBCASE("Check for variable as object"){
                auto variable3 = query::Variable {"o"};
                auto obj = Node{variable3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode = BlankNode{};
                auto obj = Node{bnode};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal1 = Literal{"Bugs Bunny"};
                auto obj = Node{literal1};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for iri as predicate"){
            auto iri2 = IRI {"http://looneytunes-graph.com#made_debut_appearance_in"};
            auto pred = Node{iri2};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode = BlankNode{};
                auto obj = Node{bnode};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri3 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal1 = Literal{"Bugs Bunny"};
                auto obj = Node{literal1};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for blanknode as predicate"){
            auto bnode2 = BlankNode{};
            auto pred = Node{bnode2};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode3 = BlankNode{};
                auto obj = Node{bnode3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal1 = Literal{"Bugs Bunny"};
                auto obj = Node{literal1};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for literal as predicate"){
            auto literal1 = Literal{"name"};
            auto pred = Node{literal1};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode2 = BlankNode{};
                auto obj = Node{bnode2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal2 = Literal{"Bugs Bunny"};
                auto obj = Node{literal2};

                test(graph, sub, pred, obj, false);
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

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode = BlankNode{};
                auto obj = Node{bnode};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri3 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal1 = Literal{"Bugs Bunny"};
                auto obj = Node{literal1};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for iri as predicate"){
            auto iri3 = IRI {"http://looneytunes-graph.com#made_debut_appearance_in"};
            auto pred = Node{iri3};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode = BlankNode{};
                auto obj = Node{bnode};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri4 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri4};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal1 = Literal{"Bugs Bunny"};
                auto obj = Node{literal1};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for blanknode as predicate"){
            auto bnode2 = BlankNode{};
            auto pred = Node{bnode2};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode3 = BlankNode{};
                auto obj = Node{bnode3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri3 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal1 = Literal{"Bugs Bunny"};
                auto obj = Node{literal1};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for literal as predicate"){
            auto literal1 = Literal{"name"};
            auto pred = Node{literal1};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode2 = BlankNode{};
                auto obj = Node{bnode2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri3 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal2 = Literal{"Bugs Bunny"};
                auto obj = Node{literal2};

                test(graph, sub, pred, obj, false);
            }
        }
    }
    SUBCASE("Check for blank node as subject") {

        auto bnode1 = BlankNode {};
        auto sub = Node{bnode1};

        SUBCASE("Check for variable as predicate"){
            auto variable1 = query::Variable {"p"};
            auto pred = Node{variable1};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode2 = BlankNode{};
                auto obj = Node{bnode2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal1 = Literal{"Bugs Bunny"};
                auto obj = Node{literal1};

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
                auto bnode2 = BlankNode{};
                auto obj = Node{bnode2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri3 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal1 = Literal{"Bugs Bunny"};
                auto obj = Node{literal1};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for blanknode as predicate"){
            auto bnode2 = BlankNode{};
            auto pred = Node{bnode2};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode3 = BlankNode{};
                auto obj = Node{bnode3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal1 = Literal{"Bugs Bunny"};
                auto obj = Node{literal1};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for literal as predicate"){
            auto literal1 = Literal{"name"};
            auto pred = Node{literal1};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode2 = BlankNode{};
                auto obj = Node{bnode2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal2 = Literal{"Bugs Bunny"};
                auto obj = Node{literal2};

                test(graph, sub, pred, obj, false);
            }
        }
    }
    SUBCASE("Check for literal as subject") {

        auto literal1 = Literal{"Bugs Bunny"};
        auto sub = Node{literal1};

        SUBCASE("Check for variable as predicate"){
            auto variable1 = query::Variable {"p"};
            auto pred = Node{variable1};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode = BlankNode{};
                auto obj = Node{bnode};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal2 = Literal{"Bugs Bunny"};
                auto obj = Node{literal2};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for iri as predicate"){
            auto iri2 = IRI {"http://looneytunes-graph.com#made_debut_appearance_in"};
            auto pred = Node{iri2};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode = BlankNode{};
                auto obj = Node{bnode};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri3 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri3};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal2 = Literal{"Bugs Bunny"};
                auto obj = Node{literal2};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for blanknode as predicate"){
            auto bnode1 = BlankNode{};
            auto pred = Node{bnode1};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode2 = BlankNode{};
                auto obj = Node{bnode2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal2 = Literal{"Bugs Bunny"};
                auto obj = Node{literal2};

                test(graph, sub, pred, obj, false);
            }
        }
        SUBCASE("Check for literal as predicate"){
            auto literal2 = Literal{"name"};
            auto pred = Node{literal2};

            SUBCASE("Check for variable as object"){
                auto variable2 = query::Variable {"o"};
                auto obj = Node{variable2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for blank node as object"){
                auto bnode = BlankNode{};
                auto obj = Node{bnode};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for iri as object"){
                auto iri2 = IRI {"http://looneytunes-graph.com#A_Wild_Hare"};
                auto obj = Node{iri2};

                test(graph, sub, pred, obj, false);
            }

            SUBCASE("Check for literal as object"){
                auto literal3 = Literal{"Bugs Bunny"};
                auto obj = Node{literal3};

                test(graph, sub, pred, obj, false);
            }
        }
    }
}

