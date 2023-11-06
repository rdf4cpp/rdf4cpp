#include <rdf4cpp/rdf/storage/tuple/DefaultDatasetBackend.hpp>

#include <utility>

namespace rdf4cpp::rdf::storage::tuple {
node::NodeStorage &DefaultDatasetBackend::node_storage() const {
    return this->node_storage_;
}

DefaultDatasetBackend::DefaultDatasetBackend(node::NodeStorage &node_storage) : IDatasetBackend{node_storage} {}

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

query::SolutionSequence DefaultDatasetBackend::match(const QuadPattern &quad_pattern) const {
    return query::SolutionSequence::new_instance<tuple::DefaultSolutionSequenceBackend>(quad_pattern, &quads_);
}

size_t DefaultDatasetBackend::size(const IRI &graph_name) const {
    return std::count_if(quads_.begin(), quads_.end(), [&](const Quad &quad) { return quad.graph().as_iri() == graph_name; });
}

IDatasetBackend::const_iterator DefaultDatasetBackend::begin() const {
    return const_iterator(quads_.begin());
}
IDatasetBackend::const_iterator DefaultDatasetBackend::end() const {
    return const_iterator(quads_.end());
}
}  // namespace rdf4cpp::rdf::storage::tuple