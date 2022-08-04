#ifndef RDF4CPP_PREFIX_HPP
#define RDF4CPP_PREFIX_HPP

#include <rdf4cpp/rdf/IRI.hpp>
#include <rdf4cpp/rdf/storage/util/robin-hood-hashing/robin_hood_hash.hpp>
#include <rdf4cpp/rdf/storage/util/tsl/sparse_map.h>

#include <string>

namespace rdf4cpp::rdf {

class Namespace {
public:
    using NodeStorage = storage::node::NodeStorage;

protected:
    std::string namespace_iri_;
    std::string abbreviation_;

    mutable NodeStorage node_storage_;

    // TODO: here a faster, less memory efficient map would be better.
    storage::util::tsl::sparse_map<std::string, storage::node::identifier::NodeBackendHandle,
                                   storage::util::robin_hood::hash<std::string>,
                                   std::equal_to<std::string>>
            cache_;

public:
    explicit Namespace(std::string_view namespace_iri,
                       NodeStorage &node_storage = NodeStorage::default_instance())
        : namespace_iri_(namespace_iri), node_storage_(node_storage) {}

    std::string const &prefix() const {
        return namespace_iri_;
    }
    const NodeStorage &node_storage() const {
        return node_storage_;
    }

    virtual IRI operator+(std::string const &);

    virtual void clear();
};
}  // namespace rdf4cpp::rdf

#endif  //RDF4CPP_PREFIX_HPP
