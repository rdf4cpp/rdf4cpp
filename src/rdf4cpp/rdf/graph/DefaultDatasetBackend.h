#ifndef RDF4CPP_DEFAULTDATASETBACKEND_H
#define RDF4CPP_DEFAULTDATASETBACKEND_H

#include <rdf4cpp/rdf/node/all.h>

#include <rdf4cpp/rdf/graph/Quad.h>
#include <rdf4cpp/rdf/graph/QuadPattern.h>

#include <rdf4cpp/rdf/graph/PatternSolutions.h>

#include <rdf4cpp/rdf/graph/DatasetBackend.h>

#include <algorithm>
#include <set>
namespace rdf4cpp::rdf::graph {

struct DefaultDatasetBackend : public DatasetBackend {
    using IRIResource = rdf4cpp::rdf::node::IRIResource;
    using NodeManager = rdf4cpp::rdf::graph::node_storage::NodeStorage;

    std::set<Quad> quads_{};

    NodeManager *node_storage_ = &NodeManager::primary_instance();

    [[nodiscard]] NodeManager &node_storage() const;

    void add(const Quad &quad) override;

    [[nodiscard]] bool contains(const Quad &quad) const override;

    [[nodiscard]] size_t size() const override;

    [[nodiscard]] PatternSolutions match(const QuadPattern &quad_pattern) const override;

    [[nodiscard]] size_t size(const node::IRIResource &graph_name) const override;

    [[nodiscard]] std::string as_string() const override;
};
}  // namespace rdf4cpp::rdf::graph

#endif  //RDF4CPP_DEFAULTDATASETBACKEND_H
