#include "Node.hpp"

#include <cassert>
#include <rdf4cpp/BlankNode.hpp>
#include <rdf4cpp/IRI.hpp>
#include <rdf4cpp/Literal.hpp>
#include <rdf4cpp/query/Variable.hpp>


namespace rdf4cpp {

Node::Node(storage::identifier::NodeBackendHandle id) noexcept : handle_(id) {}

Node Node::make_null() noexcept {
    return Node{};
}

Node Node::to_node_storage(storage::DynNodeStoragePtr node_storage) const {
    switch (handle_.type()) {
        case storage::identifier::RDFNodeType::Variable: {
            return query::Variable{handle_}.to_node_storage(node_storage);
        }
        case storage::identifier::RDFNodeType::BNode: {
            return BlankNode{handle_}.to_node_storage(node_storage);
        }
        case storage::identifier::RDFNodeType::IRI: {
            return IRI{handle_}.to_node_storage(node_storage);
        }
        case storage::identifier::RDFNodeType::Literal: {
            return Literal{handle_}.to_node_storage(node_storage);
        }
        default: {
            assert(false);
            __builtin_unreachable();
        }
    }
}

Node Node::try_get_in_node_storage(storage::DynNodeStoragePtr node_storage) const noexcept {
    switch (handle_.type()) {
        case storage::identifier::RDFNodeType::Variable: {
            return query::Variable{handle_}.try_get_in_node_storage(node_storage);
        }
        case storage::identifier::RDFNodeType::BNode: {
            return BlankNode{handle_}.try_get_in_node_storage(node_storage);
        }
        case storage::identifier::RDFNodeType::IRI: {
            return IRI{handle_}.try_get_in_node_storage(node_storage);
        }
        case storage::identifier::RDFNodeType::Literal: {
            return Literal{handle_}.try_get_in_node_storage(node_storage);
        }
        default: {
            assert(false);
            __builtin_unreachable();
        }
    }
}

bool Node::serialize(writer::BufWriterParts const writer) const noexcept {
    switch (handle_.type()) {
        [[likely]] case storage::identifier::RDFNodeType::IRI: {
            return IRI{handle_}.serialize(writer);
        }
        case storage::identifier::RDFNodeType::Variable: {
            return query::Variable{handle_}.serialize(writer);
        }
        case storage::identifier::RDFNodeType::BNode: {
            return BlankNode{handle_}.serialize(writer);
        }
        case storage::identifier::RDFNodeType::Literal: {
            return Literal{handle_}.serialize(writer);
        }
        default: {
            assert(false);
            __builtin_unreachable();
        }
    }
}
bool Node::serialize_short_form(writer::BufWriterParts const writer) const noexcept {
    switch (handle_.type()) {
        [[likely]] case storage::identifier::RDFNodeType::IRI: {
            return IRI{handle_}.serialize(writer);
        }
        case storage::identifier::RDFNodeType::Variable: {
            return query::Variable{handle_}.serialize(writer);
        }
        case storage::identifier::RDFNodeType::BNode: {
            return BlankNode{handle_}.serialize(writer);
        }
        case storage::identifier::RDFNodeType::Literal: {
            return Literal{handle_}.serialize_short_form(writer);
        }
        default: {
            assert(false);
            __builtin_unreachable();
        }
    }
}

Node::operator std::string() const noexcept {
    switch (handle_.type()) {
        [[likely]] case storage::identifier::RDFNodeType::IRI: {
            return std::string{IRI{handle_}};
        }
        case storage::identifier::RDFNodeType::Variable: {
            return std::string{query::Variable{handle_}};
        }
        case storage::identifier::RDFNodeType::BNode: {
            return std::string{BlankNode{handle_}};
        }
        case storage::identifier::RDFNodeType::Literal: {
            return std::string{Literal{handle_}};
        }
        default: {
            assert(false);
            __builtin_unreachable();
        }
    }
}

bool Node::is_literal() const noexcept {
    return handle_.is_literal();
}
bool Node::is_variable() const noexcept {
    return handle_.is_variable();
}
bool Node::is_blank_node() const noexcept {
    return handle_.is_blank_node();
}
bool Node::is_iri() const noexcept {
    return handle_.is_iri();
}

bool Node::is_inlined() const noexcept {
    return handle_.is_inlined();
}

std::weak_ordering Node::operator<=>(const Node &other) const noexcept {
    if (this->handle_ == other.handle_){
        return std::strong_ordering::equivalent;
    }

    if (this->null() && other.null()) {
        return this->handle_.type() <=> other.handle_.type();
    }

    // unbound
    if (this->null()) {
        return std::strong_ordering::less;
    } else if (other.null()) {
        return std::strong_ordering::greater;
    }

    // different type
    if (std::strong_ordering type_comp = this->handle_.type() <=> other.handle_.type(); type_comp != std::strong_ordering::equivalent){
        return type_comp;
    } else {
        switch (this->handle_.type()) {
            case storage::identifier::RDFNodeType::IRI:
                return this->handle_.iri_backend() <=> other.handle_.iri_backend();
            case storage::identifier::RDFNodeType::BNode:
                return this->handle_.bnode_backend() <=> other.handle_.bnode_backend();
            case storage::identifier::RDFNodeType::Literal:
                return Literal{handle_}.compare_with_extensions(Literal{other.handle_});
            case storage::identifier::RDFNodeType::Variable:
                return this->handle_.variable_backend() <=> other.handle_.variable_backend();
            default:{
                assert(false); // this will never be reached because RDFNodeType has only 4 values.
                return std::strong_ordering::less;
            }
        }
    }
}

bool Node::operator==(const Node &other) const noexcept {
    if (!this->is_literal() && !other.is_literal() && this->backend_handle().storage() == other.backend_handle().storage()) {
        // this short check does not work for Literals, because Literals that contain the same value as different Datatypes have different backend_handles.
        return this->backend_handle() == other.backend_handle();
    }
    return *this <=> other == std::strong_ordering::equivalent;
}

BlankNode Node::as_blank_node() const noexcept {
    if (!this->is_blank_node()) {
        return BlankNode{};
    }

    return BlankNode{handle_};
}

IRI Node::as_iri() const noexcept {
    if (!this->is_iri()) {
        return IRI{};
    }

    return IRI{handle_};
}

Literal Node::as_literal() const noexcept {
    if (!this->is_literal()) {
        return Literal{};
    }

    return Literal{handle_};
}

query::Variable Node::as_variable() const noexcept {
    if (!this->is_variable()) {
        return query::Variable{};
    }

    return query::Variable{handle_};
}

bool Node::null() const noexcept {
    return handle_.null();
}
std::ostream &operator<<(std::ostream &os, Node const &node) {
    writer::BufOStreamWriter w{os};
    node.serialize(w);
    w.finalize();

    return os;
}
storage::identifier::NodeBackendHandle const &Node::backend_handle() const noexcept {
    return handle_;
}
storage::identifier::NodeBackendHandle &Node::backend_handle() noexcept {
    return handle_;
}

TriBool Node::ebv() const noexcept {
    if (this->null() || !this->is_literal()) {
        return TriBool::Err;
    }

    return Literal{handle_}.ebv();
}

Literal Node::as_ebv(storage::DynNodeStoragePtr node_storage) const noexcept {
    return this->as_literal().as_ebv(node_storage);
}

}  // namespace rdf4cpp

auto std::formatter<rdf4cpp::Node>::format(rdf4cpp::Node n, format_context &ctx) const -> decltype(ctx.out()) {
    rdf4cpp::writer::BufOutputIteratorWriter w{ctx.out()};
    n.serialize(w);
    w.finalize();
    return w.buffer().iter;
}
