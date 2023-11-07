#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("IRIView") {
    CHECK(IRIView{"example"}.is_relative() == true);
    CHECK(IRIView{"example/foo"}.is_relative() == true);
    CHECK(IRIView{"http://example"}.is_relative() == false);

    CHECK(IRIView{"http://example"}.scheme() == "http");
    CHECK(IRIView{"example/foo"}.scheme() == "");

    CHECK(IRIView{"http://example"}.authority() == "example");
    CHECK(IRIView{"http://example/foo"}.authority() == "example");
    CHECK(IRIView{"//example?foo"}.authority() == "example");
    CHECK(IRIView{"http://example#foo"}.authority() == "example");
    CHECK(IRIView{"http:/example"}.authority() == "");

    CHECK(IRIView{"http://example"}.path() == "");
    CHECK(IRIView{"http://example/"}.path() == "/");
    CHECK(IRIView{"http://example/a/b/c"}.path() == "/a/b/c");
    CHECK(IRIView{"http://example/a/b/c#d"}.path() == "/a/b/c");
    CHECK(IRIView{"http://example/a/b/c?q"}.path() == "/a/b/c");
    CHECK(IRIView{"abc"}.path() == "abc");
    CHECK(IRIView{"/abc"}.path() == "/abc");
    CHECK(IRIView{"http:abc"}.path() == "abc");
    CHECK(IRIView{"http:/abc"}.path() == "/abc");

    CHECK(IRIView{"http://example?q"}.query() == "?q");
    CHECK(IRIView{"http://example/a/b/c?q/d"}.query() == "?q/d");
    CHECK(IRIView{"http://example/a/b/c?q/d#f"}.query() == "?q/d");
    CHECK(IRIView{"http://example/a/b/c"}.query() == "");
    CHECK(IRIView{"http://example/a/b/c#f"}.query() == "");

    CHECK(IRIView{"http://example#f"}.fragment() == "f");
    CHECK(IRIView{"http://example/a/b/c?q/d#f"}.fragment() == "f");
    CHECK(IRIView{"http://example/a/b/c?q/d"}.fragment() == "");

    CHECK(IRIView{"http://example"}.valid() == IRIFactoryError::Ok);
    CHECK(IRIView{"example"}.valid() == IRIFactoryError::Relative);
    CHECK(IRIView{"htt?p://example"}.valid() == IRIFactoryError::InvalidScheme);
}

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
