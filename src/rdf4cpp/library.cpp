#include "library.h"
#include "rdf/node/RDFNode.h"
#include "utils/sec/Result.h"
#include "utils/sec/Ok.h"
#include "utils/sec/Err.h"
#include "rdf/node/BlankNode.h"

#include <iostream>
#include <optional>
#include <variant>

void mover2(const rdf4cpp::utils::sec::Result<int, std::string> * pResult);

rdf4cpp::utils::sec::Result<int, std::string> get(){
    return rdf4cpp::utils::sec::Ok<int, std::string>(9);
}

void hello() {
    const auto test = get();
    //test.unwrap();
    mover2(&test);
    std::cout << test.is_ok() << std::endl;

    auto bnode = rdf4cpp::rdf::node::BlankNode{};
    std::cout << "Bnode? " << bnode.is_blank_node() << " IRI? " << bnode.is_iri() << std::endl;
  }

void mover2(const rdf4cpp::utils::sec::Result<int, std::string> * const test) {
    if(test->is_ok()){
        auto unwrapped = test->unwrap();
        auto myint = test->ok().value();
        std::cout << "Test " << myint << " " << unwrapped << std::endl;
    }
    if(test->is_err()){
        std::cout << "Has ok value? " << test->ok().has_value() <<std::endl;
        auto myint = test->err().value();
        std::cout << "Err " << myint << std::endl;
    }
}

int main() {

    hello();
}