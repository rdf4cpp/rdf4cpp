#include <rdf4cpp.hpp>

#include <cassert>
#include <iostream>

#define print(x) { std::cout << #x << " = " << std::string{x} << '\n'; }

using namespace rdf4cpp;

void blank_nodes() {
    bnode_mngt::RandomIdGenerator generator;

    // generate fresh ids, every call will generate a different id
    Node b1 = generator.generate();
    print(b1);

    {
        bnode_mngt::ReferenceNodeScope<> scope;

        // can still generate fresh ids
        Node not_remembered_b1 = scope.generate_node();
        print(not_remembered_b1);

        // generating ids based on blank node labels, these are remembered
        Node remembered_b1 = scope.get_or_generate_node("some-bnode-label");
        print(remembered_b1);

        Node remembered_b1_2 = scope.get_or_generate_node("some-bnode-label");
        assert(remembered_b1 == remembered_b1_2);

        Node remembered_b2 = scope.get_or_generate_node("other-bnode-label");
        assert(remembered_b1 != remembered_b2);
        print(remembered_b2);
    }

    // all old scope state is destroyed here, labels are not remembered by new scopes
}

void working_with_graphs() {
    { // rdf-merge semantics
        bnode_mngt::MergeNodeScopeManager<> mng;
        auto &subscope_1 = mng.scope("http://some-graph.com");
        auto &subscope_2 = mng.scope("http://other-graph.com");

        Node new_sub1_b1 = subscope_1.get_or_generate_node("bnode-label"); // bnode for graph 1
        Node new_sub2_b1 = subscope_2.get_or_generate_node("bnode-label"); // bnode for graph 2

        assert(new_sub1_b1 != new_sub2_b1);
    }

    { // rdf-union semantics
        bnode_mngt::UnionNodeScopeManager<> mng;
        auto &subscope_1 = mng.scope("http://some-graph.com");
        auto &subscope_2 = mng.scope("http://other-graph.com");

        Node new_sub1_b1 = subscope_1.get_or_generate_node("bnode-label"); // bnode for graph 1
        Node new_sub2_b1 = subscope_2.get_or_generate_node("bnode-label"); // bnode for graph 2

        assert(new_sub1_b1 == new_sub2_b1);
    }
}

void skolem_iris() {
    struct SkolemIRIGenerator {
        uint64_t id_ = 0;

        IRI generate(storage::DynNodeStoragePtr node_storage = storage::default_node_storage) {
            auto id = std::format("https://your-website.com/resources#{}", id_++);
            return IRI::make(id, node_storage);
        }
    };
    static_assert(bnode_mngt::NodeGenerator<SkolemIRIGenerator>);

    {
        bnode_mngt::ReferenceNodeScope<SkolemIRIGenerator> skolem_scope;

        // can still generate fresh ids
        Node not_remembered_b1 = skolem_scope.generate_node();
        print(not_remembered_b1);

        // generating ids based on blank node labels, these are remembered
        Node remembered_b1 = skolem_scope.get_or_generate_node("some-bnode-label");
        print(remembered_b1);

        Node remembered_b1_2 = skolem_scope.get_or_generate_node("some-bnode-label");
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
