#include "DefaultDatasetBackend.hpp"

namespace rdf4cpp::rdf::storage::tuple {
storage::node::NodeStorage &DefaultDatasetBackend::node_storage() const {
    return *node_storage_;
}

void DefaultDatasetBackend::add(const Quad &quad) {
    if (not quad.valid())
        throw std::logic_error{"Quad is not valid"};
    // TODO: check that RDFNodes live in node_storage_
    quads_.emplace(quad);
}

bool DefaultDatasetBackend::contains(const Quad &quad) const {
    return quads_.contains(quad);
}

size_t DefaultDatasetBackend::size() const {
    return quads_.size();
}

query::PatternSolutions DefaultDatasetBackend::match(const QuadPattern &quad_pattern) const {
    return PatternSolutions(quads_, quad_pattern);
}

size_t DefaultDatasetBackend::size(const IRI &graph_name) const {
    return std::count_if(quads_.begin(), quads_.end(), [&](const Quad &quad) { return (IRI) quad.graph() == graph_name; });
}

std::string DefaultDatasetBackend::as_string() const {
    std::string str;
    for (const auto &item : quads_) {
        str += item.as_string() + "\n";
    }
    return str;
}
}  // namespace rdf4cpp::rdf::storage::tuple