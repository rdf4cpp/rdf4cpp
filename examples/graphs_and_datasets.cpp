#include <rdf4cpp.hpp>

#include <iostream>
#include <fstream>

int main(int argc, char *argv[]) {
    using namespace rdf4cpp;

    Dataset dataset;
    auto &g = dataset.graph(IRI{"http://named_graph.com"});

    g.add({IRI{"http://example.com"}, IRI{"http://example.com"}, Literal::make_lang_tagged("text", "en")});
    g.add({IRI{"http://example.com"}, IRI{"http://example.com"}, Literal::make_lang_tagged("text", "fr")});
    g.add({IRI{"http://example.com"}, IRI{"http://example.com"}, Literal::make_simple("txt")});
    g.add({IRI{"http://example.com"}, IRI{"http://example.com"}, Literal::make_simple("text")});

    dataset.add({IRI{"http://named_graph.com"}, IRI{"http://example.com"}, IRI{"http://example.com"}, Literal::make_simple("text")});

    query::TriplePattern triple_pattern{query::Variable("x"), IRI{"http://example.com"}, query::Variable{"z"}};
    std::cout << triple_pattern.subject() << std::endl;
    std::cout << triple_pattern.predicate() << std::endl;
    std::cout << triple_pattern.object() << std::endl;

    std::cout << "g size " << g.size() << std::endl;

    Graph::solution_sequence solutions = g.match(triple_pattern);
    for (const auto &solution : solutions) {
        std::cout << "bound variables: " << solution.bound_count() << std::endl;

        for (size_t i = 0; i < solution.bound_count(); ++i) {
            std::cout << solution.variable(i) << " -> " << solution[i] << std::endl;
        }
    }

    if (argc > 1) {
        Dataset ds2;
        ds2.load_rdf_data(argv[1]);

        std::cout << "ds2 from " << argv[1] << ":" << std::endl;
        std::cout << ds2 << std::endl;
    } else {
        std::cout << "no test file provided." << std::endl;
    }
}