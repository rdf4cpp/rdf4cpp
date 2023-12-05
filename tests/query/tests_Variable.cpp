#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Variable - Check for single node with anonymous default") {

    auto variable = query::Variable{"x"};

    CHECK(not variable.is_anonymous());
    CHECK(not variable.is_blank_node());
    CHECK(not variable.is_literal());
    CHECK(variable.is_variable());
    CHECK(not variable.is_iri());
    CHECK(variable.name() == "x");
    CHECK(variable.type() == storage::node::identifier::RDFNodeType::Variable);
}

TEST_CASE("Variable - Check for single node with anonymous true") {

    auto variable = query::Variable{"x", true};

    CHECK(variable.is_anonymous());
    CHECK(not variable.is_blank_node());
    CHECK(not variable.is_literal());
    CHECK(variable.is_variable());
    CHECK(not variable.is_iri());
    CHECK(variable.name() == "x");
    CHECK(variable.type() == storage::node::identifier::RDFNodeType::Variable);
}

TEST_CASE("Variable - Check for single node with anonymous false") {

    auto variable = query::Variable{"x", false};

    CHECK(not variable.is_anonymous());
    CHECK(not variable.is_blank_node());
    CHECK(not variable.is_literal());
    CHECK(variable.is_variable());
    CHECK(not variable.is_iri());
    CHECK(variable.name() == "x");
    CHECK(variable.type() == storage::node::identifier::RDFNodeType::Variable);
}

TEST_CASE("Variable::find") {
    auto nst = storage::node::NodeStorage::new_instance();
    static constexpr std::string_view v = "var";
    static constexpr std::string_view v2 = "var2";

    CHECK(query::Variable::find_named(v, nst) == query::Variable{});
    auto qv = query::Variable::make_named(v, nst);
    CHECK(query::Variable::find_named(v, nst) == qv);
    CHECK(query::Variable::find_named(v, nst).backend_handle() == qv.backend_handle());
    CHECK(query::Variable::find_named(v2, nst) == query::Variable{});

    CHECK(query::Variable::find_anonymous(v, nst) == query::Variable{});
    qv = query::Variable::make_anonymous(v, nst);
    CHECK(query::Variable::find_anonymous(v, nst) == qv);
    CHECK(query::Variable::find_anonymous(v, nst).backend_handle() == qv.backend_handle());
    CHECK(query::Variable::find_anonymous(v2, nst) == query::Variable{});
}
