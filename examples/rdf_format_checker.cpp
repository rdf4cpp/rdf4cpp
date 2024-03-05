#include <fstream>
#include <iostream>
#include <sstream>

#include "rdf4cpp.hpp"


int main(int argc, char *argv[]) {
    using namespace rdf4cpp::rdf;
    using namespace parser;
    if (argc != 2) {
        std::cerr << "usage: pass file to be checked as only parameter, found errors are written to stdout";
        return 1;
    }

    rdf4cpp::rdf::datatypes::registry::relaxed_parsing_mode = true;

    std::ifstream in{argv[1]};
    auto nst = storage::NodeStorage::new_instance();
    int c = 0;

    IStreamQuadIterator::state_type state{.node_storage = nst};
    IStreamQuadIterator i{in, ParsingFlag::NTriples, &state};
    while (i != std::default_sentinel) {
        if (!i->has_value()) {
            std::cout << i->error() << '\n';
        }
        ++i;
        ++c;
        if (c >= 1000) {
            nst = storage::NodeStorage::new_instance();
            state.node_storage = nst;
            c = 0;
        }
    }

    std::cout << "done";
    return 0;
}
