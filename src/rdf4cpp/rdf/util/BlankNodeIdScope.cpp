#include <rdf4cpp/rdf/util/BlankNodeIdGenerator.hpp>
#include <rdf4cpp/rdf/util/BlankNodeIdScope.hpp>

namespace rdf4cpp::rdf::util {

BlankNodeIdScope::BlankNodeIdScope(generator_type &generator, NodeStorage node_storage) : generator{&generator},
                                                                                          node_storage{std::move(node_storage)} {
}

BlankNodeIdScope::BlankNodeIdScope(BlankNodeIdScope &&other) noexcept : label_to_storage{std::move(other.label_to_storage)},
                                                                        generator{other.generator},
                                                                        node_storage{std::move(other.node_storage)},
                                                                        subscopes{std::move(other.subscopes)} {
}

BlankNode BlankNodeIdScope::generate_bnode() {
    return this->generator->generated_bnode(this->node_storage);
}

IRI BlankNodeIdScope::generate_skolem_iri(std::string_view const iri_prefix) {
    return this->generator->generate_skolem_iri(iri_prefix, this->node_storage);
}

BlankNode BlankNodeIdScope::try_get_bnode(std::string_view const label) const noexcept {
    std::shared_lock lock{this->mutex};

    if (auto it = this->label_to_storage.find(label); it != this->label_to_storage.end()) {
        if (!it->second.bnode_handle.null()) {
            return BlankNode{it->second.bnode_handle};
        }
    }

    return BlankNode{};
}

IRI BlankNodeIdScope::try_get_skolem_iri(std::string_view const label) const noexcept {
    std::shared_lock lock{this->mutex};

    if (auto it = this->label_to_storage.find(label); it != this->label_to_storage.end()) {
        if (!it->second.iri_handle.null()) {
            return IRI{it->second.iri_handle};
        }
    }

    return IRI{};
}

BlankNode BlankNodeIdScope::get_or_generate_bnode(std::string_view const label) {
    std::unique_lock lock{this->mutex};

    if (auto it = this->label_to_storage.find(label); it != this->label_to_storage.end()) {
        if (!it->second.bnode_handle.null()) {
            return BlankNode{it->second.bnode_handle};
        }

        auto const iri = it->second.iri_handle.iri_backend().identifier;
        auto const id = iri.substr(iri.size() - generator_type::generated_id_length());

        BlankNode bnode{id, node_storage};
        it.value().bnode_handle = bnode.backend_handle();

        return bnode;
    }

    auto next = this->generate_bnode();

    Handle hnd{
            .iri_handle = {},
            .bnode_handle = next.backend_handle()};

    auto const [_, inserted] = this->label_to_storage.emplace(label, hnd);
    assert(inserted);

    return next;
}

IRI BlankNodeIdScope::get_or_generate_skolem_iri(std::string_view const iri_prefix, std::string_view const label) {
    std::unique_lock lock{this->mutex};

    if (auto it = this->label_to_storage.find(label); it != this->label_to_storage.end()) {
        if (!it->second.iri_handle.null()) {
            return IRI{it->second.iri_handle};
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

    auto next = this->generate_skolem_iri(iri_prefix);

    Handle hnd{
            .iri_handle = next.backend_handle(),
            .bnode_handle = {}};

    auto const [_, inserted] = this->label_to_storage.emplace(label, hnd);
    assert(inserted);

    return next;
}

BlankNodeIdScope &BlankNodeIdScope::subscope(std::string const &scope_name) noexcept {
    if (scope_name.empty()) {
        return *this;
    }

    if (auto it = this->subscopes.find(scope_name); it != this->subscopes.end()) {
        return *it.value();
    }

    auto [it, inserted] = this->subscopes.emplace(scope_name, std::make_unique<BlankNodeIdScope>(*this->generator, this->node_storage));
    assert(inserted);

    return *it.value();
}

BlankNodeIdScope &BlankNodeIdScope::subscope(std::string &&scope_name) noexcept {
    if (scope_name.empty()) {
        return *this;
    }

    if (auto it = this->subscopes.find(scope_name); it != this->subscopes.end()) {
        return *it.value();
    }

    auto [it, inserted] = this->subscopes.emplace(std::move(scope_name), std::make_unique<BlankNodeIdScope>(*this->generator, this->node_storage));
    assert(inserted);

    return *it.value();
}

BlankNodeIdScope::subscope_iterator BlankNodeIdScope::subscopes_begin() const noexcept {
    return this->subscopes.cbegin();
}

BlankNodeIdScope::subscope_iterator BlankNodeIdScope::subscopes_end() const noexcept {
    return this->subscopes.cend();
}

}  //namespace rdf4cpp::rdf::util
