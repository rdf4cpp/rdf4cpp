#include <rdf4cpp.hpp>

#include <cassert>
#include <iostream>

#define print(x) { std::cout << #x << " = " << std::string{x} << '\n'; }

using namespace rdf4cpp;

void blank_nodes() {
    auto &generator = bnode_mngt::NodeGenerator::default_instance();

    // generate fresh ids, every call will generate a different id
    Node b1 = generator.generate_node();
    print(b1);

    {
        bnode_mngt::NodeScope scope = bnode_mngt::NodeScope::new_instance();

        // can still generate fresh ids
        Node not_remembered_b1 = scope.generate_node(generator);
        print(not_remembered_b1);

        // generating ids based on blank node labels, these are remembered
        Node remembered_b1 = scope.get_or_generate_node("some-bnode-label", generator);
        print(remembered_b1);

        Node remembered_b1_2 = scope.get_or_generate_node("some-bnode-label", generator);
        assert(remembered_b1 == remembered_b1_2);

        Node remembered_b2 = scope.get_or_generate_node("other-bnode-label", generator);
        assert(remembered_b1 != remembered_b2);
        print(remembered_b2);
    }

    // all old scope state is destroyed here, labels are not remembered by new scopes
}

void working_with_graphs() {
    // subscopes for e.g. multiple graphs in one file
    bnode_mngt::ReferenceNodeScopeManager mng;
    bnode_mngt::NodeScope subscope_1 = mng.scope("http://some-graph.com");
    bnode_mngt::NodeScope subscope_2 = mng.scope("http://other-graph.com");

    Node new_sub1_b1 = subscope_1.get_or_generate_node("new-bnode-label"); // bnode for graph 1
    Node new_sub2_b1 = subscope_2.get_or_generate_node("new-bnode-label"); // bnode for graph 2

    assert(new_sub1_b1 != new_sub2_b1);
}

void skolem_iris() {
    auto generator = bnode_mngt::NodeGenerator::new_instance_with_factory<bnode_mngt::SkolemIRIFactory>("http://skolem-iris.org#");

    Node i1 = generator.generate_node();
    print(i1);

    {
        bnode_mngt::NodeScope skolem_scope = bnode_mngt::NodeScope::new_instance();

        // can still generate fresh ids
        Node not_remembered_b1 = skolem_scope.generate_node(generator);
        print(not_remembered_b1);

        // generating ids based on blank node labels, these are remembered
        Node remembered_b1 = skolem_scope.get_or_generate_node("some-bnode-label", generator);
        print(remembered_b1);

        Node remembered_b1_2 = skolem_scope.get_or_generate_node("some-bnode-label", generator);
        assert(remembered_b1 == remembered_b1_2);

        // everything else is identical to handling blank nodes
    }

    // all old scope state is destroyed here, labels are not remembered by new scopes
}

int main() {
    blank_nodes();
    skolem_iris();
    working_with_graphs();
}
