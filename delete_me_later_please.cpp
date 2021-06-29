// TODO: remove this file

#include "rdf4cpp/utils/sec/Err.h"
#include "rdf4cpp/utils/sec/Ok.h"
#include "rdf4cpp/utils/sec/Result.h"
#include <rdf4cpp/rdf.hpp>


#include <iostream>
#include <optional>
#include <variant>

void mover2(const rdf4cpp::utils::sec::Result<int, std::string> &test);

rdf4cpp::utils::sec::Result<int, std::string> get() {
    return rdf4cpp::utils::sec::Ok<int, std::string>(9);
}

void hello() {
    const auto test = get();
    //test.unwrap();
    mover2(test);
    std::cout << test.is_ok() << std::endl;

    auto bnode = rdf4cpp::rdf::BlankNode{};
    std::cout << "Bnode? " << bnode.is_blank_node() << " IRI? " << bnode.is_iri() << std::endl;
}

void mover2(const rdf4cpp::utils::sec::Result<int, std::string> &test) {
    //test->is_err();
    if (test.is_ok()) {
        auto unwrapped = test.unwrap();
        auto myint = test.ok().value();
        std::cout << "Test " << myint << " " << unwrapped << std::endl;
    }
    if (test.is_err()) {
        std::cout << "Has ok value? " << test.ok().has_value() << std::endl;
        auto myint = test.err().value();
        std::cout << "Err " << myint << std::endl;
    }
}

int main() {
    using namespace rdf4cpp::rdf;

//    storage::node::NodeStorage::new_instance<storage::node::DefaultNodeStorageBackend>();

    using namespace storage::node;

    NodeID node_id;
    std::cout << sizeof(node_id) << std::endl;
    NodeIDValue id = {0};
    std::cout << sizeof(id) << std::endl;
    node_id = {NodeStorageID{1}, RDFNodeType::Literal, id};
    std::cout << node_id.as_string() << std::endl;


    Graph g;

    Dataset dataset = g.dataset();
    Dataset dataset2(NodeStorage::new_instance());  // Dataset with an independent NodeStorage
    std::cout << "dataset node storage id: " << dataset.backend()->node_storage().id().value << std::endl;
    std::cout << "dataset2 node storage id: " << dataset2.backend()->node_storage().id().value << std::endl;

    g.add({IRI{"http://example.com"}, IRI{"http://example.com"}, Literal("text", "en")});
    g.add({IRI{"http://example.com"}, IRI{"http://example.com"}, Literal("text", "fr")});
    g.add({IRI{"http://example.com"}, IRI{"http://example.com"}, Literal("txt")});
    g.add({IRI{"http://example.com"}, IRI{"http://example.com"}, Literal("text")});

    dataset.add({IRI{"http://named_graph.com"}, IRI{"http://example.com"}, IRI{"http://example.com"}, Literal("text")});

    std::cout << "dataset string: \n"
              << writer::NQuadsWriter(g.dataset()) << std::endl;
    query::TriplePattern triple_pattern{query::Variable("x"), IRI{"http://example.com"}, query::Variable{"z"}};
    std::cout << triple_pattern.subject() << std::endl;
    std::cout << triple_pattern.predicate() << std::endl;
    std::cout << triple_pattern.object() << std::endl;
    query::PatternSolutions solutions = g.match(triple_pattern);

    std::cout << "g size " << g.size() << std::endl;

    for (const auto &solution : solutions) {
        std::cout << "bound variables: " << solution.bound_count() << std::endl;

        for (size_t i = 0; i < solution.bound_count(); ++i) {
            std::cout << solution.variable(i) << " -> " << solution[i] << std::endl;
        }
    }

    const std::string ttl_file = "/home/me/Code/rdf4cpp/test.ttl";
    Dataset ds2;
    ds2.add_ttl_file("/home/me/Code/rdf4cpp/test.ttl");
    std::cout << "ds2 from " << ttl_file << ":" << std::endl;
    std::cout << writer::NQuadsWriter(ds2) << std::endl;

    query::Variable variable("x");
    std::cout << variable << std::endl;

    query::Variable variable2 ("x1", true);
    std::cout << "variable2 =" << variable2 << std::endl;

    query::Variable variable3{"x1", true};
    std::cout << "variable3 =" << variable2 << std::endl;
    std::cout << "(variable 2 == variable3) = " << (variable2 == variable3) << std::endl;

    BlankNode blankNode ("x1");
    std::cout << "blankNode =" << blankNode << std::endl;

    std::cout << "(variable2 != blankNode) = " << (variable2 != blankNode) << std::endl;

    IRI iri = IRI("http://example.com/");
    std::cout << iri << std::endl;

    IRI iri_pred("http://example.com/pred");
    std::cout << iri_pred << std::endl;

    auto print_literal_info = [](Literal lit) {
        std::cout << "---" << std::endl;
        std::cout << "operator<<: " << lit << std::endl;
        std::cout << "NNodeWriter: " << writer::NNodeWriter(lit) << std::endl;
        std::cout << "lexical_form: " << lit.lexical_form() << std::endl;
        std::cout << "datatype: " << lit.datatype() << std::endl;
        std::cout << "language_tag: " << lit.language_tag() << std::endl;
        std::cout << "---" << std::endl;
    };

    Literal lit1 ("xxxx");
    Literal lit2 ("xxxx", iri_pred);
    Literal lit3 ("xxxx", "http://example.com/pred2");
    Literal lit4 ("xxxx", "de");
    Literal lit5{"xxxx", "de"};

    print_literal_info(lit1);
    print_literal_info(lit2);
    print_literal_info(lit3);
    print_literal_info(lit4);
    print_literal_info(lit5);

    std::vector<Node> nodes{};
    nodes.push_back(variable);
    nodes.push_back(variable2);
    nodes.push_back(blankNode);
    nodes.push_back(iri_pred);
    nodes.push_back(lit1);
    nodes.push_back(lit2);
    nodes.push_back(lit3);
    nodes.push_back(lit4);
    nodes.push_back(lit5);
    nodes.push_back(lit4.datatype());

    std::sort(nodes.begin(), nodes.end());
    std::cout << "sorted:" << std::endl;
    for (const auto &item : nodes) {
        std::cout << item << std::endl;
    }

    if (nodes[1].is_variable()) {
        auto v = (query::Variable) nodes[1];
        std::cout << v << std::endl;
    }

    std::cout << "fails in debug (is actually variable): auto b = (BlankNode) nodes[1];" << std::endl;

    auto v = (BlankNode) nodes[1];

    //    hello();
}