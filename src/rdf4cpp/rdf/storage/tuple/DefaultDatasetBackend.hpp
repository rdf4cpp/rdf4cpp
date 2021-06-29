#ifndef RDF4CPP_DEFAULTDATASETBACKEND_HPP
#define RDF4CPP_DEFAULTDATASETBACKEND_HPP

#include <rdf4cpp/rdf/Quad.hpp>
#include <rdf4cpp/rdf/query/PatternSolutions.hpp>
#include <rdf4cpp/rdf/query/QuadPattern.hpp>
#include <rdf4cpp/rdf/storage/tuple/IDatasetBackend.hpp>

#include <algorithm>
#include <set>
namespace rdf4cpp::rdf::storage::tuple {

struct DefaultDatasetBackend : public IDatasetBackend {
private:
    using PatternSolutions = rdf4cpp::rdf::query::PatternSolutions;
    using QuadPattern = rdf4cpp::rdf::query::QuadPattern;

public:
    // TODO: thread safety
    std::set<Quad> quads_{};

    // TODO: make NodeStorage configurable
    node::NodeStorage *node_storage_ = &node::NodeStorage::primary_instance();

    [[nodiscard]] node::NodeStorage &node_storage() const override;

    void add(const Quad &quad) override;

    [[nodiscard]] bool contains(const Quad &quad) const override;

    [[nodiscard]] size_t size() const override;

    [[nodiscard]] PatternSolutions match(const QuadPattern &quad_pattern) const override;

    [[nodiscard]] size_t size(const IRI &graph_name) const override;


    const_iterator begin() const override;
    const_iterator end() const override;
};
}  // namespace rdf4cpp::rdf::storage::tuple

#endif  //RDF4CPP_DEFAULTDATASETBACKEND_HPP
