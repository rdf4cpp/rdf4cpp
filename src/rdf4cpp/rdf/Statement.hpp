#ifndef RDF4CPP_STATEMENT_HPP
#define RDF4CPP_STATEMENT_HPP

#include <rdf4cpp/rdf/query/TriplePattern.hpp>

namespace rdf4cpp::rdf {

class Statement : public query::TriplePattern {
    // TODO: adjust API to Quad
public:
    Statement() = default;

    Statement(Node subject, Node predicate, Node object);

    [[nodiscard]] bool valid() const;

    [[nodiscard]] Statement to_node_storage(storage::node::DynNodeStorage node_storage) const {
        Statement st;
        auto it = st.begin();
        for (auto const &item : *this) {
            *(it++) = item.to_node_storage(node_storage);
        }
        return st;
    }

    [[nodiscard]] Statement try_get_in_node_storage(storage::node::DynNodeStorage node_storage) const noexcept {
        Statement st;
        auto it = st.begin();
        for (auto const &item : *this) {
            *(it++) = item.try_get_in_node_storage(node_storage);
        }
        return st;
    }
};
}  // namespace rdf4cpp::rdf
#endif  //RDF4CPP_STATEMENT_HPP
