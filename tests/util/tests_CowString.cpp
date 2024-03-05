#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include "rdf4cpp.hpp"

using namespace rdf4cpp::rdf::util;

TEST_SUITE("CowString") {
    TEST_CASE("borrowed") {
        CowString s{ownership_tag::borrowed, "Hello World"};
        CHECK(s == "Hello World");
        CHECK(s.view() == "Hello World");

        std::string _ignore;
        CHECK_THROWS(_ignore = s.get_owned());

        CHECK(s.get_borrowed() == "Hello World");
    }

    TEST_CASE("owned") {
        CowString bull{ownership_tag::owned, "Moo!"};
        CHECK(bull == "Moo!");
        CHECK(bull.view() == "Moo!");

        std::string_view _ignore;
        CHECK_THROWS(_ignore = bull.get_borrowed());

        CHECK(bull.get_owned() == "Moo!");
        CHECK(std::move(bull).get_owned() == "Moo!");
    }

    TEST_CASE("CowString equality") {
        CowString calf1{ownership_tag::borrowed, "Moo?"};
        CowString calf2{ownership_tag::owned, "Moo?"};

        CHECK(calf1 == calf2);
    }

    TEST_CASE("to_mutable") {
        CowString buffalo{ownership_tag::borrowed, "spherical cow"};

        std::string _ignore;
        CHECK_THROWS(_ignore = buffalo.get_owned());

        buffalo.to_mutable() += "!";
        CHECK_NOTHROW(_ignore = buffalo.get_owned());
        CHECK(buffalo == "spherical cow!");

        buffalo.to_mutable() += "!";
        CHECK(buffalo == "spherical cow!!");
    }
}
