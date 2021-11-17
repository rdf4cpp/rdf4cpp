//
// Created by kaimal on 14.11.21.
//
#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

static void test(Node sub, Node pred, Node obj, bool result){

    auto tpattern = query::TriplePattern {sub, pred, obj};

    SUBCASE("Check validity & output"){

        CHECK(tpattern.subject() == sub);
        CHECK(tpattern.predicate() == pred);
        CHECK(tpattern.object() == obj);

        CHECK(tpattern.valid());

        if(tpattern.valid()) std::cout << tpattern << std::endl;
    }
    SUBCASE("Check iterators"){
        auto b_itr = tpattern.begin();
        CHECK(*b_itr++ == sub);
        CHECK(*b_itr++ == pred);
        CHECK(*b_itr++ == obj);

        auto e_itr = tpattern.end();
        CHECK(*(--e_itr) == obj);
        CHECK(*(--e_itr) == pred);
        CHECK(*(--e_itr)== sub);
    }
    SUBCASE("Check reverse-iterators"){
        auto rb_itr = tpattern.rbegin();
        CHECK(*rb_itr++ == obj);
        CHECK(*rb_itr++ == pred);
        CHECK(*rb_itr++ == sub);

        auto re_itr = tpattern.rend();
        CHECK(*(--re_itr) == sub);
        CHECK(*(--re_itr)== pred);
        CHECK(*(--re_itr) == obj);
    }
}

TEST_CASE("TriplePattern - Check for iterators and const-iterators"){

    auto variable1 = query::Variable {"s"};
    auto sub = Node{variable1};

    auto variable2 = query::Variable {"p"};
    auto pred = Node{variable2};

    auto variable3 = query::Variable {"o"};
    auto obj = Node{variable3};

    auto variable4 = query::Variable {"s1"};
    auto sub1 = Node{variable4};

    auto variable5 = query::Variable {"p1"};
    auto pred1 = Node{variable5};

    auto variable6 = query::Variable {"o1"};
    auto obj1 = Node{variable6};

    auto tpattern = query::TriplePattern {sub, pred, obj};

    query::TriplePattern::iterator itr;
    for(itr = tpattern.begin(); itr != tpattern.end(); itr++){
        if(*itr == sub) *itr = sub1;
        else if(*itr == pred) *itr = pred1;
        else if(*itr == obj) *itr = obj1;
    }

    query::TriplePattern::const_reverse_iterator rb_const_itr;
    query::TriplePattern::const_reverse_iterator re_const_itr;

    SUBCASE("Check const-reverse-iterators"){
        rb_const_itr = tpattern.rbegin();
        CHECK(*rb_const_itr++ == obj1);
        CHECK(*rb_const_itr++ == pred1);
        CHECK(*rb_const_itr++ == sub1);

        re_const_itr = tpattern.rend();
        CHECK(*(--re_const_itr) == sub1);
        CHECK(*(--re_const_itr)== pred1);
        CHECK(*(--re_const_itr) == obj1);
    }

    query::TriplePattern::const_iterator b_const_itr;
    query::TriplePattern::const_iterator e_const_itr;

    SUBCASE("Check const-iterators"){
        b_const_itr = tpattern.begin();
        CHECK(*b_const_itr++ == sub1);
        CHECK(*b_const_itr++ == pred1);
        CHECK(*b_const_itr++ == obj1);

        e_const_itr = tpattern.end();
        CHECK(*(--e_const_itr) == obj1);
        CHECK(*(--e_const_itr) == pred1);
        CHECK(*(--e_const_itr)== sub1);
    }
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
            auto literal = Literal{"Bugs Bunny"};
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
            auto literal = Literal{"Bugs Bunny"};
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
            auto literal = Literal{"Bugs Bunny"};
            auto obj = Node{literal};

            test(sub, pred, obj, false);
        }
    }
}

TEST_CASE("TriplePattern - Check for blank node as subject") {

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
            auto literal = Literal{"Bugs Bunny"};
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
            auto literal = Literal{"Bugs Bunny"};
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
            auto literal = Literal{"Bugs Bunny"};
            auto obj = Node{literal};

            test(sub, pred, obj, false);
        }
    }
}

TEST_CASE("TriplePattern - Check for iri as subject") {

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
            auto literal = Literal{"Bugs Bunny"};
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
            auto literal = Literal{"Bugs Bunny"};
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
            auto literal = Literal{"Bugs Bunny"};
            auto obj = Node{literal};

            test(sub, pred, obj, false);
        }
    }
}