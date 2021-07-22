#include <rdf4cpp/rdf.hpp>


#include <iostream>

int main() {
    using namespace rdf4cpp::rdf;

    query::Variable variable("x");
    std::cout << variable << std::endl;

    query::Variable variable2("x1", true);
    std::cout << "variable2 =" << variable2 << std::endl;

    query::Variable variable3{"x1", true};
    std::cout << "variable3 =" << variable2 << std::endl;
    std::cout << "(variable 2 == variable3) = " << (variable2 == variable3) << std::endl;

    BlankNode blankNode("x1");
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

    Literal lit1("xxxx");
    Literal lit2("xxxx", iri_pred);
    Literal lit3("xxxx", "http://example.com/pred2");
    Literal lit4("xxxx", "de");
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
}