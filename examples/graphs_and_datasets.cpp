#include <rdf4cpp/rdf.hpp>

#include <iostream>
#include <fstream>

int main(int argc, char *argv[]) {
    using namespace rdf4cpp::rdf;
    Graph g;

    Dataset dataset = g.dataset();
    Dataset dataset2(storage::node::NodeStorage::new_instance());  // Dataset with an independent NodeStorage
    std::cout << "dataset node storage id: " << dataset.backend().node_storage().id().value << std::endl;
    std::cout << "dataset2 node storage id: " << dataset2.backend().node_storage().id().value << std::endl;

    g.add({IRI{"http://example.com"}, IRI{"http://example.com"}, Literal::make_lang_tagged("text", "en")});
    g.add({IRI{"http://example.com"}, IRI{"http://example.com"}, Literal::make_lang_tagged("text", "fr")});
    g.add({IRI{"http://example.com"}, IRI{"http://example.com"}, Literal::make_simple("txt")});
    g.add({IRI{"http://example.com"}, IRI{"http://example.com"}, Literal::make_simple("text")});

    dataset.add({IRI{"http://named_graph.com"}, IRI{"http://example.com"}, IRI{"http://example.com"}, Literal("text")});

    std::cout << "dataset string: \n"
              << writer::NQuadsWriter(g.dataset()) << std::endl;
    query::TriplePattern triple_pattern{query::Variable("x"), IRI{"http://example.com"}, query::Variable{"z"}};
    std::cout << triple_pattern.subject() << std::endl;
    std::cout << triple_pattern.predicate() << std::endl;
    std::cout << triple_pattern.object() << std::endl;
    query::SolutionSequence solutions = g.match(triple_pattern);

    std::cout << "g size " << g.size() << std::endl;

    for (const auto &solution : solutions) {
        std::cout << "bound variables: " << solution.bound_count() << std::endl;

        for (size_t i = 0; i < solution.bound_count(); ++i) {
            std::cout << solution.variable(i) << " -> " << solution[i] << std::endl;
        }
    }

    if (argc > 1) {
        Dataset ds2;
        std::ifstream ifs{argv[1]};

        if (!ifs.is_open()) {
            std::cerr << "unable to open provided file\n";
            return 1;
        }

        for (parser::IStreamQuadIterator qit{ifs}; qit != parser::IStreamQuadIterator{}; ++qit) {
            if (qit->has_value()) {
                ds2.add(qit->value());
            } else {
                std::cerr << qit->error() << '\n';
            }
        }

        std::cout << "ds2 from " << argv[1] << ":" << std::endl;
        std::cout << writer::NQuadsWriter(ds2) << std::endl;
    } else {
        std::cout << "no test file provided." << std::endl;
    }
}