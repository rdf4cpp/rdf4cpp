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
        util::NodeScope scope = generator.scope();

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

        {
            // subscopes for e.g. multiple graphs in one file
            util::NodeScope subscope = scope.subscope("http://some-graph.com");

            Node remembered_b1_sub = subscope.get_or_generate_node("some-bnode-label");
            assert(remembered_b1 == remembered_b1_sub); // remembers mapping from parent scope
            print(remembered_b1_sub);

            {
                // nested subscopes
                util::NodeScope subsubscope = subscope.subscope("queryA");

                Node remembered_b1_sub_sub = subsubscope.get_or_generate_node("some-bnode-label");
                assert(remembered_b1 == remembered_b1_sub_sub); // also remembers mapping of parent's-parent
                assert(remembered_b1_sub == remembered_b1_sub_sub);
            }
        }

        {
            // subscopes are reusable
            util::NodeScope subscope = scope.subscope("http://some-graph.com");

            Node remembered_b1_sub_again = subscope.try_get_node("some-bnode-label");
            assert(!remembered_b1_sub_again.null());
            print(remembered_b1_sub_again);
        }
    }

    // all old scope state is destroyed here, labels are not remembered by new scopes
}

void skolem_iris() {
    auto skolem_factory = util::SkolemIRIFactory{"http://skolem-iris.org#"};
    auto &generator = util::NodeGenerator::default_instance();

    Node i1 = generator.generate_node(skolem_factory);
    print(i1);

    {
        util::NodeScope skolem_scope = generator.scope<util::ReferenceSkolemIRIScope>(skolem_factory);

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
}
