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

/**
 * @brief use type substitution to convert lhs and rhs into the same type
 * @param lhs_entry the datatype entry for the type of lhs
 * @param lhs_value the value of lhs
 * @param rhs_entry the datatype entry for the type of rhs
 * @param rhs_value the value of rhs
 */
void substitute(
        datatypes::registry::DatatypeRegistry::DatatypeEntry &lhs_entry,
        std::any &lhs_value,
        datatypes::registry::DatatypeRegistry::DatatypeEntry &rhs_entry,
        std::any &rhs_value) {

    assert(lhs_entry.datatype_iri != rhs_entry.datatype_iri);

    using DatatypeEntry = datatypes::registry::DatatypeRegistry::DatatypeEntry;
    using ConversionResult = datatypes::registry::DatatypeRegistry::ConversionResult;

    auto const substitute_impl = [](auto const &to_substitute_entry, auto const &to_substitute_value, auto const &target_entry) {
        DatatypeEntry result_entry = to_substitute_entry;
        ConversionResult pr{ to_substitute_entry.datatype_iri, to_substitute_value };

        while (result_entry.subtype_rank > target_entry.subtype_rank) {
            pr = result_entry.into_supertype_fptr(pr.converted_value);
            result_entry = datatypes::registry::DatatypeRegistry::get_entry(pr.converted_iri).value();
        }

        if (result_entry.subtype_rank != target_entry.subtype_rank) {
            throw std::runtime_error{ "subtype substitution error, invalid hierarchy" };
        }

        return std::make_pair(result_entry, pr.converted_value);
    };

    if (lhs_entry.subtype_rank == 0 && rhs_entry.subtype_rank == 0) {
        // nothing to do
        return;
    } else if (lhs_entry.subtype_rank < rhs_entry.subtype_rank) {
        // substitute rhs
        auto const [substituted_entry, substituted_value] = substitute_impl(rhs_entry, rhs_value, lhs_entry);

        rhs_entry = substituted_entry;
        rhs_value = substituted_value;
    } else if (lhs_entry.subtype_rank > rhs_entry.subtype_rank) {
        // substitute lhs
        auto const [substituted_entry, substituted_value] = substitute_impl(lhs_entry, lhs_value, rhs_entry);

        lhs_entry = substituted_entry;
        lhs_value = substituted_value;
    } else {
        // substitute both once
        // This path will be used if the types are at the same subtype level but promotion failed.
        // When this happens it means there is no connecting promotion path between the types at the current
        // subtype level, and we have to try to promote again one level up.

        if (lhs_entry.into_supertype_fptr == nullptr || rhs_entry.into_supertype_fptr == nullptr) {
            throw std::runtime_error{ "subtype substitution unsupported for type" };
        }

        ConversionResult cr_lhs = lhs_entry.into_supertype_fptr(lhs_value);
        ConversionResult cr_rhs = rhs_entry.into_supertype_fptr(rhs_value);

        DatatypeEntry e_lhs = datatypes::registry::DatatypeRegistry::get_entry(cr_lhs.converted_iri).value();
        DatatypeEntry e_rhs = datatypes::registry::DatatypeRegistry::get_entry(cr_rhs.converted_iri).value();

        lhs_entry = e_lhs;
        lhs_value = cr_lhs.converted_value;
        rhs_entry = e_rhs;
        rhs_value = cr_rhs.converted_value;
    }
}

/**
 * @brief use type promotion to try to convert lhs and rhs to the same type
 *
 * @param lhs_entry the datatype entry for the type of lhs
 * @param lhs_value the value of lhs
 * @param rhs_entry the datatype entry for the type of rhs
 * @param rhs_value the value of rhs
 */
void promote(
        datatypes::registry::DatatypeRegistry::DatatypeEntry &lhs_entry,
        std::any &lhs_value,
        datatypes::registry::DatatypeRegistry::DatatypeEntry &rhs_entry,
        std::any &rhs_value) {

    assert(lhs_entry.datatype_iri != rhs_entry.datatype_iri);

    using DatatypeEntry = datatypes::registry::DatatypeRegistry::DatatypeEntry;
    using ConversionResult = datatypes::registry::DatatypeRegistry::ConversionResult;

    auto const promote_impl = [](auto const &to_promote_entry, auto const &to_promote_value, auto const &target_entry) {
        DatatypeEntry result_entry = to_promote_entry;
        ConversionResult pr{ to_promote_entry.datatype_iri, to_promote_value };

        while (pr.converted_iri != target_entry.datatype_iri && result_entry.promote_fptr != nullptr) {
            pr = result_entry.promote_fptr(pr.converted_value);
            result_entry = datatypes::registry::DatatypeRegistry::get_entry(pr.converted_iri).value();
        }

        if (pr.converted_iri != target_entry.datatype_iri) {
            throw std::runtime_error{ "datatype mismatch and not in same promotion hierarchy" };
        }

        return std::make_pair(result_entry, pr.converted_value);
    };

    if (lhs_entry.promote_rank > rhs_entry.promote_rank) {
        // promote lhs
        auto const [promoted_entry, promoted_value] = promote_impl(lhs_entry, lhs_value, rhs_entry);

        lhs_entry = promoted_entry;
        lhs_value = promoted_value;
    } else if (lhs_entry.promote_rank < rhs_entry.promote_rank) {
        // promote rhs
        auto const [promoted_entry, promoted_value] = promote_impl(rhs_entry, rhs_value, lhs_entry);

        rhs_entry = promoted_entry;
        rhs_value = promoted_value;
    } else {
        throw std::runtime_error{ "datatype mismatch and not in same promotion hierarchy" };
    }
}

/**
 * @brief uses type promotion and subtype substitution to convert lhs and rhs to the same type
 *
 * @param lhs_entry the datatype entry for the type of lhs
 * @param lhs_value the value of lhs
 * @param rhs_entry the datatype entry for the type of rhs
 * @param rhs_value the value of rhs
 * @return a tuple consiting of: the result type, the final value of lhs, the final value of rhs
 */
std::tuple<datatypes::registry::DatatypeRegistry::DatatypeEntry, std::any, std::any> equalize(
        datatypes::registry::DatatypeRegistry::DatatypeEntry lhs_entry,
        std::any lhs_value,
        datatypes::registry::DatatypeRegistry::DatatypeEntry rhs_entry,
        std::any rhs_value) {

    while (lhs_entry.datatype_iri != rhs_entry.datatype_iri) {
        if (lhs_entry.subtype_rank == rhs_entry.subtype_rank) {
            try {
                // try to promote, if it doesn't work we are not in the same promotion hierarchy
                promote(lhs_entry, lhs_value, rhs_entry, rhs_value);
                break;
            } catch (std::runtime_error const &e) {
                // cannot go one subtype level up, and promotion did not work
                // => the types are not covertible into a common type
                if (lhs_entry.subtype_rank == 0) {
                    throw e;
                }
            }
        }

        // either bring types up to the same level, or decrease the rank of both by 1
        substitute(lhs_entry, lhs_value, rhs_entry, rhs_value);
    }

    return std::make_tuple(lhs_entry, lhs_value, rhs_value);
}

template<typename OpSelect>
Literal Literal::numeric_binop_impl(OpSelect op_select, Literal const &other, NodeStorage &node_storage) const {
    auto const this_type = this->handle_.type();
    auto const other_type = other.handle_.type();

    if (this_type != other_type || this_type != RDFNodeType::Literal) {
        throw std::runtime_error{ "node type mismatch, expected literal" };
    }

    auto const [result_entry, lhs_val, rhs_val] = [&]() {
        auto const this_lit_type = this->handle_.literal_backend().datatype_id;
        auto const other_lit_type = other.handle_.literal_backend().datatype_id;

        auto this_entry = datatypes::registry::DatatypeRegistry::get_entry(this->datatype().identifier()).value();

        if (this_lit_type == other_lit_type) {
            return std::make_tuple(this_entry, this->value(), other.value());
        } else {
            auto other_entry = datatypes::registry::DatatypeRegistry::get_entry(other.datatype().identifier()).value();
            return equalize(this_entry, this->value(), other_entry, other.value());
        }
    }();

    if (!result_entry.numeric_ops.has_value()) {
        throw std::runtime_error{ "result datatype not numeric" };
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

    auto const entry = datatypes::registry::DatatypeRegistry::get_entry(this->datatype().identifier()).value();

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