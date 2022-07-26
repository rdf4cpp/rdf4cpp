#include "Literal.hpp"

#include <rdf4cpp/rdf/IRI.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/LiteralBackend.hpp>

namespace rdf4cpp::rdf {

Literal::Literal() noexcept : Node(NodeBackendHandle{{}, storage::node::identifier::RDFNodeType::Literal, {}}) {}
Literal::Literal(std::string_view lexical_form, Node::NodeStorage &node_storage)
    : Node(NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::LiteralBackendView{
                                     .datatype_id = storage::node::identifier::NodeID::xsd_string_iri.first,
                                     .lexical_form = lexical_form,
                                     .language_tag = ""}),
                             storage::node::identifier::RDFNodeType::Literal,
                             node_storage.id()}) {}

Literal::Literal(std::string_view lexical_form, const IRI &datatype, Node::NodeStorage &node_storage)
    : Literal(make(lexical_form, datatype, node_storage)) {}
Literal::Literal(std::string_view lexical_form, std::string_view lang, Node::NodeStorage &node_storage)
    : Node(NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::LiteralBackendView{
                                     .datatype_id = storage::node::identifier::NodeID::rdf_langstring_iri.first,
                                     .lexical_form = lexical_form,
                                     .language_tag = lang}),
                             storage::node::identifier::RDFNodeType::Literal,
                             node_storage.id()}) {}

IRI Literal::datatype() const {
    NodeBackendHandle iri_handle{handle_.literal_backend().datatype_id, storage::node::identifier::RDFNodeType::IRI, backend_handle().node_storage_id()};
    return IRI(iri_handle);
}

std::string_view Literal::lexical_form() const {
    return handle_.literal_backend().lexical_form;
}

std::string_view Literal::language_tag() const {
    return handle_.literal_backend().language_tag;
}
Literal::operator std::string() const {
    // TODO: escape non-standard chars correctly
    auto quote_lexical = [](std::string_view lexical) -> std::string {
        // TODO: escape everything that needs to be escaped in N-Tripels/N-Quads
        std::ostringstream out{};
        out << "\"";

        for (auto const &character : lexical) {
            switch (character) {
                case '\\': {
                    out << R"(\\)";
                    break;
                }
                case '\n': {
                    out << R"(\n)";
                    break;
                }
                case '\r': {
                    out << R"(\r)";
                    break;
                }
                case '"': {
                    out << R"(\")";
                    break;
                }
                    [[likely]] default : {
                        out << character;
                        break;
                    }
            }
        }
        out << "\"";
        return out.str();
    };
    const auto &literal = handle_.literal_backend();
    if (literal.datatype_id == NodeID::rdf_langstring_iri.first) {
        return quote_lexical(literal.lexical_form) + "@" + std::string{literal.language_tag};
    } else {
        auto const &dtype_iri = NodeStorage::find_iri_backend_view(NodeBackendHandle{literal.datatype_id, storage::node::identifier::RDFNodeType::IRI, backend_handle().node_storage_id()});
        return quote_lexical(literal.lexical_form) + "^^" + dtype_iri.n_string();
    }
}
bool Literal::is_literal() const { return true; }
bool Literal::is_variable() const { return false; }
bool Literal::is_blank_node() const { return false; }
bool Literal::is_iri() const { return false; }
Literal::Literal(Node::NodeBackendHandle handle) : Node(handle) {}

std::partial_ordering Literal::operator<=>(const Literal &other) const {
    if (auto comp_id = this->handle_ <=> other.handle_; comp_id == std::partial_ordering::equivalent) {
        return std::strong_ordering::equal;
    } else if (auto comp_type = this->handle_.type() <=> other.handle_.type(); comp_type != std::strong_ordering::equal) {
        return comp_type;
    } else {  // same type, different id.
        switch (this->handle_.type()) {
            case RDFNodeType::IRI:
                return this->handle_.iri_backend() <=> other.handle_.iri_backend();
            case RDFNodeType::BNode:
                return this->handle_.bnode_backend() <=> other.handle_.bnode_backend();
            case RDFNodeType::Literal:
                return this->handle_.literal_backend() <=> other.handle_.literal_backend();
            case RDFNodeType::Variable:
                return this->handle_.variable_backend() <=> other.handle_.variable_backend();
        }
        return std::strong_ordering::less;
    }
}
bool Literal::operator==(const Literal &other) const {
    if (this->datatype() != other.datatype()) {
        return false;
    } else if (this->lexical_form() == other.lexical_form()) {
        return true;
    } else {
        return false;
        /*if(this->handle_.type() == RDFNodeType::Literal) return this->handle_.literal_backend() == other.handle_.literal_backend();
        else return false;*/
    }
}

std::ostream &operator<<(std::ostream &os, const Literal &literal) {
    os << (std::string) literal;
    return os;
}
std::any Literal::value() const {
    datatypes::registry::DatatypeRegistry::factory_fptr_t factory = datatypes::registry::DatatypeRegistry::get_factory(this->datatype().identifier());
    if (factory != nullptr)
        return factory(lexical_form());
    else
        return {};
}

Literal Literal::make(std::string_view lexical_form, const IRI &datatype, Node::NodeStorage &node_storage) {
    // retrieving the datatype.identifier() requires a lookup in the backend -> cache
    std::string_view const datatype_identifier = datatype.identifier();  // string_view
    auto const factory_func = datatypes::registry::DatatypeRegistry::get_factory(datatype_identifier);

    if (factory_func) {  // this is a know datatype -> canonize the string representation
        auto const native_type = factory_func(lexical_form);
        // if factory_func exists, to_string_func must exist, too
        auto const to_string_func = datatypes::registry::DatatypeRegistry::get_to_string(datatype_identifier);

        return Literal(NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::LiteralBackendView{
                                                 .datatype_id = datatype.to_node_storage(node_storage).backend_handle().node_id(),
                                                 .lexical_form = to_string_func(native_type),
                                                 .language_tag = ""}),
                                         storage::node::identifier::RDFNodeType::Literal,
                                         node_storage.id()});
    } else { // datatype is not registered, so we cannot parse the lexical_form nor canonize it
        return Literal(NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::LiteralBackendView{
                                                 .datatype_id = datatype.to_node_storage(node_storage).backend_handle().node_id(),
                                                 .lexical_form = lexical_form,
                                                 .language_tag = ""}),
                                         storage::node::identifier::RDFNodeType::Literal,
                                         node_storage.id()});
    }
}

template<typename OpSelect>
Literal Literal::numeric_binop_impl(OpSelect op_select, Literal const &other, NodeStorage &node_storage) const {
    auto const this_type = this->handle_.type();
    auto const other_type = other.handle_.type();

    if (this_type != other_type || this_type != RDFNodeType::Literal) {
        throw std::runtime_error{ "node type mismatch, expected literal" };
    }

    auto const [res_identifier, lhs_val, rhs_val] = [&]() {
        auto const this_lit_type = this->handle_.literal_backend().datatype_id;
        auto const other_lit_type = other.handle_.literal_backend().datatype_id;

        if (this_lit_type == other_lit_type) {
            return std::make_tuple(this->datatype().identifier(), this->value(), other.value());
        } else {
            auto const lhs_datatype = this->datatype().identifier();
            auto const rhs_datatype = other.datatype().identifier();

            auto const equalizer = datatypes::registry::DatatypeRegistry::get_common_type_conversion(
                    lhs_datatype,
                    rhs_datatype);

            if (!equalizer.has_value()) {
                throw std::runtime_error{ "datatype mismatch and not in same promotion hierarchy" };
            }

            return std::make_tuple(equalizer->target_type_iri(),
                                   equalizer->convert(lhs_datatype, this->value()),
                                   equalizer->convert(rhs_datatype, other.value()));
        }
    }();

    auto const result_entry = datatypes::registry::DatatypeRegistry::get_entry(res_identifier).value().get();

    if (!result_entry.numeric_ops.has_value()) {
        throw std::runtime_error{ "common datatype is not numeric" };
    }

    auto op_res = op_select(*result_entry.numeric_ops)(lhs_val, rhs_val);

    return Literal {
        result_entry.to_string_fptr(op_res),
        IRI{ result_entry.datatype_iri, node_storage },
        node_storage
    };
}

template<typename OpSelect>
Literal Literal::numeric_unop_impl(OpSelect op_select, NodeStorage &node_storage) const {
    auto const this_type = this->handle_.type();

    if (this_type != RDFNodeType::Literal) {
        throw std::runtime_error{ "node type mismatch, expected literal" };
    }

    auto const &entry = datatypes::registry::DatatypeRegistry::get_entry(this->datatype().identifier()).value().get();

    if (!entry.numeric_ops.has_value()) {
        throw std::runtime_error{ "datatype not numeric" };
    }

    auto op_res = op_select(*entry.numeric_ops)(this->value());

    return Literal {
        entry.to_string_fptr(op_res),
        this->datatype(),
        node_storage
    };
}

template<typename BinOp>
Literal Literal::logical_binop_impl(BinOp bin_op, Literal const &other, NodeStorage &node_storage) const {
    auto const this_type = this->handle_.type();
    auto const other_type = other.handle_.type();

    if (this_type != other_type || this_type != RDFNodeType::Literal) {
        throw std::runtime_error{ "node type mismatch, expected literal" };
    }

    auto const ebv_this = datatypes::registry::DatatypeRegistry::get_ebv(this->datatype().identifier());

    if (ebv_this == nullptr) {
        throw std::runtime_error{ "lhs not convertible to bool" };
    }

    auto const ebv_other = datatypes::registry::DatatypeRegistry::get_ebv(other.datatype().identifier());

    if (ebv_other == nullptr) {
        throw std::runtime_error{ "rhs not convertible to bool" };
    }

    return Literal::make<datatypes::xsd::Boolean>(bin_op(ebv_this(this->value()), ebv_other(other.value())), node_storage);
}

Literal Literal::logical_not_impl(NodeStorage &node_storage) const {
    auto const this_type = this->handle_.type();

    if (this_type != RDFNodeType::Literal) {
        throw std::runtime_error{ "node type mismatch, expected literal" };
    }

    auto const ebv_fptr = datatypes::registry::DatatypeRegistry::get_ebv(this->datatype().identifier());

    if (ebv_fptr == nullptr) {
        throw std::runtime_error{ "datatype not convertible to bool" };
    }

    return Literal::make<datatypes::xsd::Boolean>(!ebv_fptr(this->value()), node_storage);
}

Literal Literal::add(Literal const &other, Node::NodeStorage &node_storage) const {
    return this->numeric_binop_impl([](auto const &num_ops) {
        return num_ops.add_fptr;
    }, other, node_storage);
}

Literal Literal::operator+(Literal const &other) const {
    return this->add(other);
}

Literal Literal::sub(Literal const &other, Node::NodeStorage &node_storage) const {
    return this->numeric_binop_impl([](auto const &num_ops) {
        return num_ops.sub_fptr;
    }, other, node_storage);
}

Literal Literal::operator-(Literal const &other) const {
    return this->sub(other);
}

Literal Literal::mul(Literal const &other, Node::NodeStorage &node_storage) const {
    return this->numeric_binop_impl([](auto const &num_ops) {
        return num_ops.mul_fptr;
    }, other, node_storage);
}

Literal Literal::operator*(Literal const &other) const {
    return this->mul(other);
}

Literal Literal::div(Literal const &other, Node::NodeStorage &node_storage) const {
    return this->numeric_binop_impl([](auto const &num_ops) {
        return num_ops.div_fptr;
    }, other, node_storage);
}

Literal Literal::operator/(Literal const &other) const {
    return this->div(other);
}

Literal Literal::pos(Node::NodeStorage &node_storage) const {
    return this->numeric_unop_impl([](auto const &num_ops) {
        return num_ops.pos_fptr;
    }, node_storage);
}

Literal Literal::operator+() const {
    return this->pos();
}

Literal Literal::neg(Node::NodeStorage &node_storage) const {
    return this->numeric_unop_impl([](auto const &num_ops) {
        return num_ops.neg_fptr;
    }, node_storage);
}

Literal Literal::operator-() const {
    return this->neg();
}

Literal Literal::logical_and(Literal const &other, Node::NodeStorage &node_storage) const {
    return this->logical_binop_impl(std::logical_and{}, other, node_storage);
}

Literal Literal::operator&&(Literal const &other) const {
    return this->logical_and(other);
}

Literal Literal::logical_or(Literal const &other, Node::NodeStorage &node_storage) const {
    return this->logical_binop_impl(std::logical_or{}, other, node_storage);
}

Literal Literal::operator||(Literal const &other) const {
    return this->logical_or(other);
}

Literal Literal::logical_not(Node::NodeStorage &node_storage) const {
    return this->logical_not_impl(node_storage);
}

Literal Literal::operator!() const {
    return this->logical_not();
}

}  // namespace rdf4cpp::rdf