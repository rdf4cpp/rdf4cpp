//
// Created by kaimal on 14.11.21.
//
#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Variable - Check for single node with anonymous default") {

    auto variable = query::Variable {"x"};

    CHECK(not variable.is_anonymous());
    CHECK(not variable.is_blank_node());
    CHECK(not variable.is_literal());
    CHECK(variable.is_variable());
    CHECK(not variable.is_iri());
    CHECK(variable.name() == "x");
    CHECK(variable.type() == storage::node::RDFNodeType::Variable);
}

TEST_CASE("Variable - Check for single node with anonymous true") {

    auto variable = query::Variable {"x", true};

    CHECK(variable.is_anonymous());
    CHECK(not variable.is_blank_node());
    CHECK(not variable.is_literal());
    CHECK(variable.is_variable());
    CHECK(not variable.is_iri());
    CHECK(variable.name() == "x");
    CHECK(variable.type() == storage::node::RDFNodeType::Variable);
}

TEST_CASE("Variable - Check for single node with anonymous false") {

    auto variable = query::Variable {"x", false};

    CHECK(not variable.is_anonymous());
    CHECK(not variable.is_blank_node());
    CHECK(not variable.is_literal());
    CHECK(variable.is_variable());
    CHECK(not variable.is_iri());
    CHECK(variable.name() == "x");
    CHECK(variable.type() == storage::node::RDFNodeType::Variable);
}