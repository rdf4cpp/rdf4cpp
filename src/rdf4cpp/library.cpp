// TODO: remove this file

#include "utils/sec/Err.h"
#include "utils/sec/Ok.h"
#include "utils/sec/Result.h"
#include <rdf4cpp/rdf/node/all.h>

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
    using namespace rdf4cpp::rdf::node;
    Variable variable = RDFNode::make_variable("x");
    std::cout << variable.as_string() << std::endl;

    Variable variable2 = RDFNode::make_variable("x1", true);
    std::cout << "variable2 =" << variable2.as_string() << std::endl;

    BlankNode blankNode = RDFNode::make_bnode("x1");
    std::cout << "blankNode =" << blankNode.as_string() << std::endl;

    std::cout << "(variable2 != blankNode) = " << (variable2 != blankNode) << std::endl;

    IRIResource iri_pred = RDFNode::make_iri("http://example.com/pred");
    std::cout << iri_pred.as_string() << std::endl;

    auto print_literal_info = [] (Literal lit){
      std::cout << "---" << std::endl;
      std::cout << "unquoted: " << lit.as_string() << std::endl;
      std::cout << "quoted: " << lit.as_string(true) << std::endl;
      std::cout << "lexical_form: " << lit.lexical_form() << std::endl;
      std::cout << "datatype: " << lit.datatype().as_string() << std::endl;
      std::cout << "language_tag: " << lit.language_tag() << std::endl;
      std::cout << "---" << std::endl;
    };

    Literal lit1 = RDFNode::make_string_literal("xxxx");
    Literal lit2 = RDFNode::make_typed_literal("xxxx", iri_pred);
    Literal lit3 = RDFNode::make_typed_literal("xxxx", "http://example.com/pred2");
    Literal lit4 = RDFNode::make_lang_literal("xxxx", "de");

    print_literal_info(lit1);
    print_literal_info(lit2);
    print_literal_info(lit3);
    print_literal_info(lit4);

    std::vector<RDFNode> nodes {};
    nodes.push_back(variable);
    nodes.push_back(variable2);
    nodes.push_back(blankNode);
    nodes.push_back(iri_pred);
    nodes.push_back(lit1);
    nodes.push_back(lit2);
    nodes.push_back(lit3);
    nodes.push_back(lit4);
    nodes.push_back(lit4.datatype());

    std::sort(nodes.begin(), nodes.end());
    std::cout << "sorted:" << std::endl;
    for (const auto &item : nodes){
        std::cout << item.as_string() << std::endl;
    }


    //    hello();
}