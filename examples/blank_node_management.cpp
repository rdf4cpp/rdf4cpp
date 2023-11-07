#include <rdf4cpp/rdf.hpp>

#include <cassert>
#include <iostream>

#define print(x) { std::cout << #x << " = " << std::string{x} << '\n'; }

using namespace rdf4cpp::rdf;

void blank_nodes() {
    auto &generator = util::NodeGenerator::default_instance();

    // generate fresh ids, every call will generate a different id
    Node b1 = generator.generate_node();
    print(b1);

    {
        util::NodeScope scope = util::NodeScope::new_instance();

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

        {
            // subscopes for e.g. multiple graphs in one file
            util::ReferenceNodeScopeManager mng = util::ReferenceNodeScopeManager::new_instance();
            util::NodeScope subscope_1 = mng.scope("http://some-graph.com");
            util::NodeScope subscope_2 = mng.scope("http://other-graph.com");

            Node remembered_b1_sub = subscope_1.get_or_generate_node("some-bnode-label");
            assert(remembered_b1 == remembered_b1_sub); // remembers mapping from parent scope
            print(remembered_b1_sub);

            {
                Node new_sub1_b1 = subscope_1.get_or_generate_node("new-bnode-label");
                Node new_sub2_b1 = subscope_2.get_or_generate_node("new-bnode-label");

                assert(new_sub1_b1 != new_sub2_b1);
            }
        }
    }

    // all old scope state is destroyed here, labels are not remembered by new scopes
}

void skolem_iris() {
    auto generator = util::NodeGenerator::new_instance_with_factory<util::SkolemIRIFactory>("http://skolem-iris.org#");

    Node i1 = generator.generate_node();
    print(i1);

    {
        util::NodeScope skolem_scope = util::NodeScope::new_instance();

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
}
