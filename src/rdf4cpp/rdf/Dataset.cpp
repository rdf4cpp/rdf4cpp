#include "Dataset.hpp"
#include <rdf4cpp/rdf/Graph.hpp>
#include <rdf4cpp/rdf/writer/TryWrite.hpp>

#include <utility>

namespace rdf4cpp::rdf {
Dataset::Dataset(Dataset::DatasetStorage dataset_storage) : dataset_storage(std::move(dataset_storage)) {}

Dataset::Dataset(Dataset::NodeStorage node_storage)
    : dataset_storage(Dataset::DatasetStorage::new_instance<storage::tuple::DefaultDatasetBackend>(std::move(node_storage))) {}

void Dataset::add(const Quad &quad) {
    dataset_storage.add(quad.to_node_storage(backend().node_storage()));
}
bool Dataset::contains(const Quad &quad) const {
    return dataset_storage.contains(quad.to_node_storage(backend().node_storage()));
}
query::SolutionSequence Dataset::match(const query::QuadPattern &quad_pattern) const {
    return dataset_storage.match(quad_pattern.to_node_storage(backend().node_storage()));
}
size_t Dataset::size() const {
    return dataset_storage.size();
}
size_t Dataset::size(const IRI &graph_name) const {
    return dataset_storage.size(static_cast<IRI>(graph_name.to_node_storage(backend().node_storage())));
}
Graph Dataset::graph(const IRI &graph_name) {
    return {dataset_storage, graph_name};
}
Graph Dataset::graph() {
    return {dataset_storage, IRI::default_graph(backend().node_storage())};
}
Dataset::iterator Dataset::begin() const {
    return dataset_storage.begin();
}
Dataset::iterator Dataset::end() const {
    return dataset_storage.end();
}
Dataset::DatasetStorage &Dataset::backend() {
    return dataset_storage;
}
const Dataset::DatasetStorage &Dataset::backend() const {
    return dataset_storage;
}
bool Dataset::serialize(void *const buffer, writer::Cursor *cursor, writer::FlushFunc const flush) const noexcept {
    for (Quad const &quad : *this) {
        RDF4CPP_DETAIL_TRY_SERIALIZE(quad.graph());
        RDF4CPP_DETAIL_TRY_WRITE_STR(" ");

        RDF4CPP_DETAIL_TRY_SERIALIZE(quad.subject());
        RDF4CPP_DETAIL_TRY_WRITE_STR(" ");

        RDF4CPP_DETAIL_TRY_SERIALIZE(quad.predicate());
        RDF4CPP_DETAIL_TRY_WRITE_STR(" ");

        RDF4CPP_DETAIL_TRY_SERIALIZE(quad.object());
        RDF4CPP_DETAIL_TRY_WRITE_STR(" .\n");
    }

    return true;
}
std::ostream &operator<<(std::ostream &os, Dataset const &ds) {
    writer::BufOStreamWriter w{os};
    ds.serialize(w);
    w.finalize();
    return os;
}
}  // namespace rdf4cpp::rdf
