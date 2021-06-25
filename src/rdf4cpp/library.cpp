// TODO: remove this file

#include "utils/sec/Err.h"
#include "utils/sec/Ok.h"
#include "utils/sec/Result.h"
#include <rdf4cpp/rdf/node/all.h>

#include <iostream>
#include <optional>
#include <rdf4cpp/rdf/graph/Dataset.h>
#include <rdf4cpp/rdf/graph/Graph.h>
#include <rdf4cpp/rdf/graph/Quad.h>
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

    auto bnode = rdf4cpp::rdf::node::BlankNode{};
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
    using namespace rdf4cpp::rdf::graph;
    using namespace rdf4cpp::rdf::node;

    node_storage::NodeStorage::new_instance<node_storage::DefaultNodeStorageBackend>();

    using namespace node_storage;

    NodeID node_id;
    std::cout << sizeof(node_id) << std::endl;
    NodeIDValue id = {0};
    std::cout << sizeof(id) << std::endl;
    node_id = {NodeStorageID{1}, RDFNodeType::Literal, id};
    std::cout << node_id.as_string() << std::endl;


    Graph g;

    Dataset dataset = g.dataset();

    g.add({IRIResource{"http://example.com"}, IRIResource{"http://example.com"}, Literal("text", "en")});
    g.add({IRIResource{"http://example.com"}, IRIResource{"http://example.com"}, Literal("text", "fr")});
    g.add({IRIResource{"http://example.com"}, IRIResource{"http://example.com"}, Literal("txt")});
    g.add({IRIResource{"http://example.com"}, IRIResource{"http://example.com"}, Literal("text")});

    dataset.add({IRIResource{"http://named_graph.com"}, IRIResource{"http://example.com"}, IRIResource{"http://example.com"}, Literal("text")});

    std::cout << "dataset string: \n"
              << g.dataset().as_string() << std::endl;
    TriplePattern triple_pattern{Variable("x"), IRIResource{"http://example.com"}, Variable{"z"}};
    std::cout << triple_pattern.subject().as_string() << std::endl;
    std::cout << triple_pattern.predicate().as_string() << std::endl;
    std::cout << triple_pattern.object().as_string() << std::endl;
    PatternSolutions solutions = g.match(triple_pattern);

    std::cout << "g size " << g.size() << std::endl;

    for (const PatternSolution &solution : solutions) {
        std::cout << "bound variables: " << solution.bound_count() << std::endl;

        for (size_t i = 0; i < solution.bound_count(); ++i) {
            std::cout << solution.variable(i).as_string() << " -> " << solution[i].as_string(true) << std::endl;
        }
    }

    const std::string ttl_file = "/home/me/Code/rdf4cpp/test.ttl";
    Dataset ds2;
    ds2.add_ttl_file("/home/me/Code/rdf4cpp/test.ttl");
    std::cout << "ds2 from " << ttl_file << ":" << std::endl;
    std::cout << ds2.as_string() << std::endl;

    Variable variable("x");
    std::cout << variable.as_string() << std::endl;

    Variable variable2 ("x1", true);
    std::cout << "variable2 =" << variable2.as_string() << std::endl;

    Variable variable3{"x1", true};
    std::cout << "variable3 =" << variable2.as_string() << std::endl;
    std::cout << "(variable 2 == variable3) = " << (variable2 == variable3) << std::endl;

    BlankNode blankNode ("x1");
    std::cout << "blankNode =" << blankNode.as_string() << std::endl;

    std::cout << "(variable2 != blankNode) = " << (variable2 != blankNode) << std::endl;

    IRIResource iri = IRIResource("http://example.com/");
    std::cout << iri.as_string() << std::endl;

    IRIResource iri_pred("http://example.com/pred");
    std::cout << iri_pred.as_string() << std::endl;

    auto print_literal_info = [](Literal lit) {
        std::cout << "---" << std::endl;
        std::cout << "unquoted: " << lit.as_string() << std::endl;
        std::cout << "quoted: " << lit.as_string(true) << std::endl;
        std::cout << "lexical_form: " << lit.lexical_form() << std::endl;
        std::cout << "datatype: " << lit.datatype().as_string() << std::endl;
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

    std::vector<RDFNode> nodes{};
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
        std::cout << item.as_string() << std::endl;
    }

    if (nodes[1].is_variable()) {
        auto v = (Variable) nodes[1];
        std::cout << v.as_string() << std::endl;
    }

    std::cout << "fails in debug (is actually variable): auto b = (BlankNode) nodes[1];" << std::endl;

    auto v = (BlankNode) nodes[1];

    //    hello();
}