#include <rdf4cpp/rdf.hpp>

#include <cassert>
#include <iostream>

#define print(x) { std::cout << #x << " = " << std::string{x} << '\n'; }

int main() {
    using namespace rdf4cpp::rdf;

    auto &generator = util::BlankNodeIdGenerator::default_instance();

    // generate fresh ids, every call will generate a different id
    BlankNode b1 = generator.generated_bnode();
    print(b1);

    IRI i1 = generator.generate_skolem_iri("http://skolem-iris.org#");
    print(i1);

    {
        util::BlankNodeIdScope scope = generator.scope();

        // scan still generate fresh ids
        BlankNode not_remembered_b1 = scope.generate_bnode();
        IRI not_remembered_i1 = scope.generate_skolem_iri("http://skolem-iris.org#");
        print(not_remembered_b1);
        print(not_remembered_i1);

        // generating ids based on blank node labels, these are remembered
        BlankNode remembered_b1 = scope.get_or_generate_bnode("some-bnode-label");
        IRI remembered_i1 = scope.get_or_generate_skolem_iri("http://skolem-iris.org#", "some-bnode-label");
        assert(std::string{"http://skolem-iris.org#"} + std::string{remembered_b1.identifier()} == remembered_i1.identifier());
        print(remembered_b1);
        print(remembered_i1);

        BlankNode remembered_b1_2 = scope.get_or_generate_bnode("some-bnode-label");
        assert(remembered_b1 == remembered_b1_2);

        BlankNode remembered_b2 = scope.get_or_generate_bnode("other-bnode-label");
        assert(remembered_b1 != remembered_b2);
        print(remembered_b2);

        {
            // subscopes for e.g. multiple graphs in one file
            util::BlankNodeIdScope subscope = scope.subscope("http://some-graph.com");

            BlankNode remembered_b1_sub = subscope.get_or_generate_bnode("some-bnode-label");
            assert(remembered_b1 == remembered_b1_sub); // remembers mapping from parent scope
            print(remembered_b1_sub);

            {
                // nested subscopes
                util::BlankNodeIdScope subsubscope = subscope.subscope("queryA");

                BlankNode remembered_b1_sub_sub = subsubscope.get_or_generate_bnode("some-bnode-label");
                assert(remembered_b1 == remembered_b1_sub_sub); // also remembers mapping of parent's-parent
                assert(remembered_b1_sub == remembered_b1_sub_sub);
            }
        }

        {
            // subscopes are reusable
            util::BlankNodeIdScope subscope = scope.subscope("http://some-graph.com");

            BlankNode remembered_b1_sub_again = subscope.try_get_bnode("some-bnode-label");
            assert(!remembered_b1_sub_again.null());
            print(remembered_b1_sub_again);
        }
    }

    // all old scope state is destroyed here, labels are not remembered by new scopes
}
