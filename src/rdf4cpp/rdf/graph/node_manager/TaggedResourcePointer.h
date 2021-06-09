#ifndef RDF4CPP_TAGGEDRESOURCEPOINTER_H
#define RDF4CPP_TAGGEDRESOURCEPOINTER_H

#include "rdf4cpp/rdf/graph/node_manager/ResourceManager.h"

#include <bitset>
#include <compare>
#include <cstddef>
namespace rdf4cpp::rdf::graph::node_manager {

class TaggedResourcePtr {
    constexpr static size_t size_t_bits = ID::size_t_bits;
    std::bitset<size_t_bits> bits{};

    static std::bitset<size_t_bits> encode_ptr(void const *ptr, ID id);

    [[nodiscard]] void *ptr() const;

public:
    TaggedResourcePtr() = default;
    TaggedResourcePtr(void *ptr, ID id);

    [[nodiscard]] RDFNodeType type() const;

    [[nodiscard]] bool is_iri() const;
    [[nodiscard]] bool is_literal() const;
    [[nodiscard]] bool is_bnode() const;
    [[nodiscard]] bool is_variable() const;

    [[nodiscard]] bool empty() const;

    ID id(ResourceManager &node_manager = ResourceManager::default_instance());

    [[nodiscard]] IRIBackend &iri() const;
    [[nodiscard]] LiteralBackend &literal() const;
    [[nodiscard]] BNodeBackend &bnode() const;
    [[nodiscard]] VariableBackend &variable() const;

    bool operator==(const TaggedResourcePtr &other) const {
        return this->bits == other.bits;
    }

    auto operator<=>(const TaggedResourcePtr &other) const {
        return this->bits.to_ulong() <=> other.bits.to_ulong();
    }
};
}  // namespace rdf4cpp::rdf::graph::node_manager

#endif  //RDF4CPP_TAGGEDRESOURCEPOINTER_H
