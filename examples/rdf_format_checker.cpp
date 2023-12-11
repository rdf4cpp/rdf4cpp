#include <fstream>
#include <iostream>
#include <sstream>

#include <rdf4cpp/rdf.hpp>


int main(int argc, char *argv[]) {
    using namespace rdf4cpp::rdf;
    using namespace parser;
    if (argc != 2) {
        std::cerr << "usage: pass file to be checked as only parameter, found errors are written to stdout";
        return 1;
    }

    rdf4cpp::rdf::datatypes::registry::dbpedia_mode = true;

    std::ifstream in{argv[1]};
    std::string line{};
    auto nst = storage::node::NodeStorage::new_instance();
    int c = 0;
    while (std::getline(in, line)) {
        std::stringstream p{line};
        IStreamQuadIterator::state_type state{.node_storage = nst};
        IStreamQuadIterator i{p, ParsingFlag::NTriples, &state};
        bool lineerr = false;
        while (i != IStreamQuadIterator{}) {
            if (!i->has_value()) {
                lineerr = true;
                std::cerr << i->error() << '\n';
            }
            ++i;
        }
        if (lineerr) {
            std::cout << line << '\n';
            std::cerr << line << '\n';
        }
        ++c;
        if (c >= 1000) {
            nst = storage::node::NodeStorage::new_instance();
            c = 0;
        }
    }

    return 0;
}
