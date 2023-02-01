#include <rdf4cpp/rdf/util/reference_backends/ReferenceBlankNodeIdScope.hpp>

namespace rdf4cpp::rdf::util {

ReferenceBlankNodeIdScope::ReferenceBlankNodeIdScope(generator_type &generator) : generator{&generator} {
}

ReferenceBlankNodeIdScope::ReferenceBlankNodeIdScope(ReferenceBlankNodeIdScope &&other) noexcept : label_to_storage{std::move(other.label_to_storage)},
                                                                                                   generator{other.generator},
                                                                                                   subscopes{std::move(other.subscopes)} {
}

BlankNode ReferenceBlankNodeIdScope::generate_bnode(NodeStorage &node_storage) {
    return this->generator->generated_bnode(node_storage);
}

IRI ReferenceBlankNodeIdScope::generate_skolem_iri(std::string_view const iri_prefix, NodeStorage &node_storage) {
    return this->generator->generate_skolem_iri(iri_prefix, node_storage);
}

BlankNode ReferenceBlankNodeIdScope::try_get_bnode(std::string_view const label) const noexcept {
    std::shared_lock lock{this->mutex};

    if (auto it = this->label_to_storage.find(label); it != this->label_to_storage.end()) {
        if (!it->second.bnode_handle.null()) {
            return BlankNode{it->second.bnode_handle};
        }
    }

    return BlankNode{};
}

IRI ReferenceBlankNodeIdScope::try_get_skolem_iri(std::string_view const label) const noexcept {
    std::shared_lock lock{this->mutex};

    if (auto it = this->label_to_storage.find(label); it != this->label_to_storage.end()) {
        if (!it->second.iri_handle.null()) {
            return IRI{it->second.iri_handle};
        }
    }

    return IRI{};
}

BlankNode ReferenceBlankNodeIdScope::get_or_generate_bnode(std::string_view const label, NodeStorage &node_storage) {
    std::unique_lock lock{this->mutex};

    if (auto it = this->label_to_storage.find(label); it != this->label_to_storage.end()) {
        if (!it->second.bnode_handle.null()) {
            return BlankNode{it->second.bnode_handle}.to_node_storage(node_storage);
        }

        auto const iri = it->second.iri_handle.iri_backend().identifier;
        auto const id = iri.substr(iri.size() - this->generator->max_generated_id_size());

        BlankNode bnode{id, node_storage};
        it.value().bnode_handle = bnode.backend_handle();

        return bnode;
    }

    auto next = this->generate_bnode(node_storage);

    Handle hnd{
            .iri_handle = {},
            .bnode_handle = next.backend_handle()};

    auto const [_, inserted] = this->label_to_storage.emplace(label, hnd);
    assert(inserted);
    this->forward_mapping(label, hnd);

    return next;
}

IRI ReferenceBlankNodeIdScope::get_or_generate_skolem_iri(std::string_view const iri_prefix, std::string_view const label, NodeStorage &node_storage) {
    std::unique_lock lock{this->mutex};

    if (auto it = this->label_to_storage.find(label); it != this->label_to_storage.end()) {
        if (!it->second.iri_handle.null()) {
            return IRI{it->second.iri_handle}.to_node_storage(node_storage);
        }

        auto const id = it->second.bnode_handle.bnode_backend().identifier;

        std::string iri_str;
        iri_str.reserve(iri_prefix.size() + id.size());
        iri_str.append(iri_prefix);
        iri_str.append(id);

        IRI iri{iri_str, node_storage};
        it.value().iri_handle = iri.backend_handle();

        return iri;
    }

    auto next = this->generate_skolem_iri(iri_prefix, node_storage);

    Handle hnd{
            .iri_handle = next.backend_handle(),
            .bnode_handle = {}};

    auto const [_, inserted] = this->label_to_storage.emplace(label, hnd);
    assert(inserted);
    this->forward_mapping(label, hnd);

    return next;
}

ReferenceBlankNodeIdScope &ReferenceBlankNodeIdScope::subscope(std::string scope_name) noexcept {
    if (scope_name.empty()) {
        return *this;
    }

    if (auto it = this->subscopes.find(scope_name); it != this->subscopes.end()) {
        return *it.value();
    }

    auto [it, inserted] = this->subscopes.emplace(std::move(scope_name), std::make_unique<ReferenceBlankNodeIdScope>(*this->generator));
    assert(inserted);

    for (auto const &[key, value] : this->label_to_storage) {
        it.value()->label_to_storage.emplace(key, value);
    }

    return *it.value();
}

void ReferenceBlankNodeIdScope::forward_mapping(std::string_view label, Handle hnd) {
    for (auto &[_, child] : this->subscopes) {
        auto it = child->label_to_storage.find(label);
        if (it != child->label_to_storage.end()) {
            continue;
        }

        child->label_to_storage.emplace(label, hnd);
        child->forward_mapping(label, hnd);
    }
}

}  //namespace rdf4cpp::rdf::util
