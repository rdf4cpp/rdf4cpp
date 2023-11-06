#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("base") {
    IRIFactory fact{"http://ex.org/"};

    CHECK(fact.from_relative("foo").value().identifier() == "http://ex.org/foo");

    fact.base = "http://bar.org/";
    CHECK(fact.from_relative("foo").value().identifier() == "http://bar.org/foo");
}

TEST_CASE("prefix") {
    IRIFactory fact{};

    fact.assign_prefix("pre", "http://ex.org/");
    fact.assign_prefix("foo", "http://foo.org/");

    CHECK(fact.from_prefix("pre", "bar").value().identifier() == "http://ex.org/bar");
    CHECK(fact.from_prefix("foo", "bar").value().identifier() == "http://foo.org/bar");
    CHECK(fact.from_prefix("bar", "bar").error() == IRIFactoryError::UnknownPrefix);

    fact.assign_prefix("pre", "http://ex.org/pre2/");
    CHECK(fact.from_prefix("pre", "bar").value().identifier() == "http://ex.org/pre2/bar");

    fact.clear_prefix("pre");
    CHECK(fact.from_prefix("pre", "bar").error() == IRIFactoryError::UnknownPrefix);
}

TEST_CASE("relative prefix") {
    IRIFactory fact{"http://ex.org/"};

    fact.assign_prefix("pre", "pre/");
    fact.assign_prefix("foo", "foo/");

    CHECK(fact.from_prefix("pre", "bar").value().identifier() == "http://ex.org/pre/bar");
    CHECK(fact.from_prefix("foo", "bar").value().identifier() == "http://ex.org/foo/bar");
    CHECK(fact.from_prefix("bar", "bar").error() == IRIFactoryError::UnknownPrefix);

    fact.assign_prefix("pre", "pre2/");
    CHECK(fact.from_prefix("pre", "bar").value().identifier() == "http://ex.org/pre2/bar");

    fact.clear_prefix("pre");
    CHECK(fact.from_prefix("pre", "bar").error() == IRIFactoryError::UnknownPrefix);
}
