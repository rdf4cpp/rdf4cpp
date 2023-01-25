#include <rdf4cpp/rdf/util/BlankNodeIdManager.hpp>
#include <rdf4cpp/rdf/util/private/BlankNodeIdGeneratorImpl.hpp>

#include <random>
#include <utility>
#include <tuple>

namespace rdf4cpp::rdf::util {

BlankNodeIdGenerator::BlankNodeIdGenerator(uint64_t const seed) : impl{std::make_unique<Impl>(seed)} {
}

BlankNodeIdGenerator::BlankNodeIdGenerator(BlankNodeIdGenerator &&other) noexcept = default;
BlankNodeIdGenerator::~BlankNodeIdGenerator() noexcept = default;

BlankNodeIdGenerator BlankNodeIdGenerator::from_seed(uint64_t const seed) {
    return BlankNodeIdGenerator{seed};
}

BlankNodeIdGenerator BlankNodeIdGenerator::from_entropy() {
    return BlankNodeIdGenerator{std::random_device{}()};
}

void BlankNodeIdGenerator::reseed(uint64_t const seed) {
    this->impl->reseed(seed);
}

std::string BlankNodeIdGenerator::generate_id() {
    return this->impl->generate_id();
}


BlankNodeIdManager::BlankNodeIdManager(generator_type &&generator, label_mapping_type mapping_state, NodeStorage &node_storage)
    : generator{std::move(generator)}, label_to_storage{std::move(mapping_state)}, node_storage{node_storage} {
}

BlankNodeIdManager &BlankNodeIdManager::default_instance() {
    static BlankNodeIdManager instance = BlankNodeIdManager::from_entropy();
    return instance;
}

BlankNodeIdManager BlankNodeIdManager::from_entropy(NodeStorage &node_storage) {
    return BlankNodeIdManager{generator_type::from_entropy(), {}, node_storage};
}

BlankNodeIdManager BlankNodeIdManager::from_seed(uint64_t seed, NodeStorage &node_storage) {
    return BlankNodeIdManager{generator_type::from_seed(seed), {}, node_storage};
}

BlankNodeIdManager BlankNodeIdManager::from_seed_with_state(uint64_t seed, BlankNodeIdManager::label_mapping_type mapping_state, BlankNodeIdManager::NodeStorage &node_storage) {
    return BlankNodeIdManager{generator_type::from_seed(seed), std::move(mapping_state), node_storage};
}

BlankNode BlankNodeIdManager::generate() {
    std::unique_lock lock{this->mutex};
    return BlankNode{this->generator.generate_id(), this->node_storage};
}

BlankNode BlankNodeIdManager::try_get(std::string_view const label) const noexcept {
    std::shared_lock lock{this->mutex};

    if (auto it = this->label_to_storage.find(label); it != this->label_to_storage.end()) {
        return BlankNode{it->second};
    }

    return BlankNode{};
}

BlankNode BlankNodeIdManager::get_or_generate(std::string_view const label) {
    std::unique_lock lock{this->mutex};

    if (auto it = this->label_to_storage.find(label); it != this->label_to_storage.end()) {
        return BlankNode{it->second};
    }

    auto const next_id = this->generator.generate_id();
    auto const node_id = this->node_storage.find_or_make_id(storage::node::view::BNodeBackendView{ .identifier = next_id });

    auto const [it, inserted] = this->label_to_storage.emplace(std::piecewise_construct,
                                                               std::forward_as_tuple(label),
                                                               std::forward_as_tuple(node_id,
                                                                                     storage::node::identifier::RDFNodeType::BNode,
                                                                                     this->node_storage.id()));

    assert(inserted);
    return BlankNode{it->second};
}

}  //namespace rdf4cpp::rdf::util
