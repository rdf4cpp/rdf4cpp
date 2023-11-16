#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("IRIView") {
    CHECK(IRIView{"example"}.is_relative() == true);
    CHECK(IRIView{"example/foo"}.is_relative() == true);
    CHECK(IRIView{"http://example"}.is_relative() == false);

    CHECK(IRIView{"http://example"}.scheme() == "http");
    CHECK(IRIView{"example/foo"}.scheme() == std::nullopt);
    CHECK(IRIView{":example/foo"}.scheme() == "");

    CHECK(IRIView{"http://example"}.authority() == "example");
    CHECK(IRIView{"http://example/foo"}.authority() == "example");
    CHECK(IRIView{"//example?foo"}.authority() == "example");
    CHECK(IRIView{"http://example#foo"}.authority() == "example");
    CHECK(IRIView{"http:/example"}.authority() == std::nullopt);
    CHECK(IRIView{"http:///foo"}.authority() == "");

    CHECK(IRIView{"http://example"}.path() == "");
    CHECK(IRIView{"http://example/"}.path() == "/");
    CHECK(IRIView{"http://example/a/b/c"}.path() == "/a/b/c");
    CHECK(IRIView{"http://example/a/b/c#d"}.path() == "/a/b/c");
    CHECK(IRIView{"http://example/a/b/c?q"}.path() == "/a/b/c");
    CHECK(IRIView{"abc"}.path() == "abc");
    CHECK(IRIView{"/abc"}.path() == "/abc");
    CHECK(IRIView{"http:abc"}.path() == "abc");
    CHECK(IRIView{"http:/abc"}.path() == "/abc");

    CHECK(IRIView{"http://example?q"}.query() == "q");
    CHECK(IRIView{"http://example/a/b/c?q/d"}.query() == "q/d");
    CHECK(IRIView{"http://example/a/b/c?q/d#f"}.query() == "q/d");
    CHECK(IRIView{"http://example/a/b/c"}.query() == std::nullopt);
    CHECK(IRIView{"http://example/a/b/c#f"}.query() == std::nullopt);
    CHECK(IRIView{"http://example/a/b/c?"}.query() == "");

    CHECK(IRIView{"http://example#f"}.fragment() == "f");
    CHECK(IRIView{"http://example/a/b/c?q/d#f"}.fragment() == "f");
    CHECK(IRIView{"http://example/a/b/c?q/d#"}.fragment() == "");
    CHECK(IRIView{"http://example/a/b/c?q/d"}.fragment() == std::nullopt);

    CHECK(IRIView{"http://example#f"}.to_absolute() == "http://example");
    CHECK(IRIView{"http://example"}.to_absolute() == "http://example");

    CHECK(IRIView{"http://example"}.valid() == IRIFactoryError::Ok);
    CHECK(IRIView{"example"}.valid() == IRIFactoryError::Relative);
    CHECK(IRIView{"htt?p://example"}.valid() == IRIFactoryError::InvalidScheme);

    // from https://datatracker.ietf.org/doc/html/rfc3986#section-3
    auto [scheme, auth, path, query, frag] = IRIView{"foo://example.com:8042/over/there?name=ferret#nose"}.all_parts();
    CHECK(scheme == "foo");
    CHECK(auth == "example.com:8042");
    CHECK(path == "/over/there");
    CHECK(query == "name=ferret");
    CHECK(frag == "nose");

    CHECK(IRIView{"http://user@example:1234/foo"}.userinfo() == "user");
    CHECK(IRIView{"http://@example:1234/foo"}.userinfo() == "");
    CHECK(IRIView{"http://example:1234/foo"}.userinfo() == std::nullopt);
    CHECK(IRIView{"http://user@example:1234/foo"}.host() == "example");
    CHECK(IRIView{"http://example:1234/foo"}.host() == "example");
    CHECK(IRIView{"http://user@example/foo"}.host() == "example");
    CHECK(IRIView{"http://example/foo"}.host() == "example");
    CHECK(IRIView{"http://user@example:1234/foo"}.port() == "1234");
    CHECK(IRIView{"http://user@example:/foo"}.port() == "");
    CHECK(IRIView{"http://user@example/foo"}.port() == std::nullopt);
}

TEST_CASE("base") {
    // Test cases from: https://datatracker.ietf.org/doc/html/rfc3986#section-5.4
    // these are for URIs, but each URI is also a IRI

    IRIFactory fact{"http://a/b/c/d;p?q"};

    // normal
    CHECK(fact.from_relative("g:h").value().identifier() == "g:h");
    CHECK(fact.from_relative("g").value().identifier() == "http://a/b/c/g");
    CHECK(fact.from_relative("./g").value().identifier() == "http://a/b/c/g");
    CHECK(fact.from_relative("g/").value().identifier() == "http://a/b/c/g/");
    CHECK(fact.from_relative("/g").value().identifier() == "http://a/g");
    CHECK(fact.from_relative("//g").value().identifier() == "http://g");
    CHECK(fact.from_relative("?y").value().identifier() == "http://a/b/c/d;p?y");
    CHECK(fact.from_relative("g?y").value().identifier() == "http://a/b/c/g?y");
    CHECK(fact.from_relative("#s").value().identifier() == "http://a/b/c/d;p?q#s");
    CHECK(fact.from_relative("g#s").value().identifier() == "http://a/b/c/g#s");
    CHECK(fact.from_relative("g?y#s").value().identifier() == "http://a/b/c/g?y#s");
    CHECK(fact.from_relative(";x").value().identifier() == "http://a/b/c/;x");
    CHECK(fact.from_relative("g;x").value().identifier() == "http://a/b/c/g;x");
    CHECK(fact.from_relative("g;x?y#s").value().identifier() == "http://a/b/c/g;x?y#s");
    CHECK(fact.from_relative("").value().identifier() == "http://a/b/c/d;p?q");
    CHECK(fact.from_relative(".").value().identifier() == "http://a/b/c/");
    CHECK(fact.from_relative("./").value().identifier() == "http://a/b/c/");
    CHECK(fact.from_relative("..").value().identifier() == "http://a/b/");
    CHECK(fact.from_relative("../").value().identifier() == "http://a/b/");
    CHECK(fact.from_relative("../g").value().identifier() == "http://a/b/g");
    CHECK(fact.from_relative("../..").value().identifier() == "http://a/");
    CHECK(fact.from_relative("../../").value().identifier() == "http://a/");
    CHECK(fact.from_relative("../../g").value().identifier() == "http://a/g");

    //abnormal
    // more dots
    CHECK(fact.from_relative("../../../g").value().identifier() == "http://a/g");
    CHECK(fact.from_relative("../../../../g").value().identifier() == "http://a/g");
    // not dots
    CHECK(fact.from_relative("/./g").value().identifier() == "http://a/g");
    CHECK(fact.from_relative("/../g").value().identifier() == "http://a/g");
    CHECK(fact.from_relative("g.").value().identifier() == "http://a/b/c/g.");
    CHECK(fact.from_relative(".g").value().identifier() == "http://a/b/c/.g");
    CHECK(fact.from_relative("g..").value().identifier() == "http://a/b/c/g..");
    CHECK(fact.from_relative("..g").value().identifier() == "http://a/b/c/..g");
    // stupid dots
    CHECK(fact.from_relative("./../g").value().identifier() == "http://a/b/g");
    CHECK(fact.from_relative("./g/.").value().identifier() == "http://a/b/c/g/");
    CHECK(fact.from_relative("g/./h").value().identifier() == "http://a/b/c/g/h");
    CHECK(fact.from_relative("g/../h").value().identifier() == "http://a/b/c/h");
    CHECK(fact.from_relative("g;x=1/./y").value().identifier() == "http://a/b/c/g;x=1/y");
    CHECK(fact.from_relative("g;x=1/../y").value().identifier() == "http://a/b/c/y");
    // unexpected dots
    CHECK(fact.from_relative("g?y/./x").value().identifier() == "http://a/b/c/g?y/./x");
    CHECK(fact.from_relative("g?y/../x").value().identifier() == "http://a/b/c/g?y/../x");
    CHECK(fact.from_relative("g#y/./x").value().identifier() == "http://a/b/c/g#y/./x");
    CHECK(fact.from_relative("g#y/../x").value().identifier() == "http://a/b/c/g#y/../x");
}

TEST_CASE("base reassign") {
    IRIFactory fact{"http://example/foo"};

    CHECK(fact.from_relative("bar").value().identifier() == "http://example/bar");

    CHECK(fact.get_base() == "http://example/foo");

    CHECK(fact.set_base("/bar") == IRIFactoryError::Relative);

    CHECK(fact.get_base() == "http://example/foo");

    CHECK(fact.set_base("http://example/bar") == IRIFactoryError::Ok);

    CHECK(fact.get_base() == "http://example/bar");

    CHECK(fact.from_relative("foo").value().identifier() == "http://example/foo");
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
