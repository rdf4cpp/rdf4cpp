
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf/node/BlankNode.h>

TEST_CASE("Builds at least") {
    auto bnode = rdf4cpp::rdf::node::BlankNode{};
    CHECK(bnode.is_blank_node());
    CHECK(not bnode.is_iri());
}