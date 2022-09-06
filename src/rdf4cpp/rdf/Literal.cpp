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
bool Literal::is_numeric() const {
    using namespace datatypes::registry;

    return this->handle_.node_id().literal_type().is_numeric()
           || DatatypeRegistry::get_numerical_ops(this->datatype().to_datatype_iri()) != nullptr;
}

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
    if (this->null() || other.null()) {
        return this->null() == other.null();
    } else if (this->datatype() != other.datatype()) {
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
    using namespace datatypes::registry;

    DatatypeRegistry::factory_fptr_t factory = DatatypeRegistry::get_factory(this->get_datatype_iri());
    if (factory != nullptr)
        return factory(lexical_form());
    else
        return {};
}

Literal Literal::make(std::string_view lexical_form, const IRI &datatype, Node::NodeStorage &node_storage) {
    using namespace datatypes::registry;

    // retrieving the datatype.identifier() requires a lookup in the backend -> cache
    DatatypeIRIView const datatype_identifier = datatype.to_datatype_iri();
    auto const factory_func = DatatypeRegistry::get_factory(datatype_identifier);

    if (factory_func) {  // this is a know datatype -> canonize the string representation
        auto const native_type = factory_func(lexical_form);
        // if factory_func exists, to_string_func must exist, too
        auto const to_string_func = DatatypeRegistry::get_to_string(datatype_identifier);

        return Literal(NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::LiteralBackendView{
                                                 .datatype_id = datatype.to_node_storage(node_storage).backend_handle().node_id(),
                                                 .lexical_form = to_string_func(native_type),
                                                 .language_tag = ""}),
                                         storage::node::identifier::RDFNodeType::Literal,
                                         node_storage.id()});
    } else {  // datatype is not registered, so we cannot parse the lexical_form nor canonize it
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
    using namespace datatypes::registry;

    if (this->null() || other.null() || this->is_fixed_not_numeric() || other.is_fixed_not_numeric()) {
        return Literal{};
    }

    auto const this_datatype = this->get_datatype_iri();
    auto const this_entry = DatatypeRegistry::get_entry(this_datatype);
    assert(this_entry != nullptr);

    if (!this_entry->numeric_ops.has_value()) {
        return Literal{};  // not numeric
    }

    auto const other_datatype = other.get_datatype_iri();

    if (this_datatype == other_datatype) {
        DatatypeRegistry::NumericOpResult const op_res = op_select(*this_entry->numeric_ops)(this->value(),
                                                                                             other.value());

        auto const to_string_fptr = [&]() {
            if (op_res.result_type_iri == this_datatype) [[likely]] {
                return this_entry->to_string_fptr;
            } else [[unlikely]] {
                return DatatypeRegistry::get_to_string(op_res.result_type_iri);
            }
        }();

        assert(to_string_fptr != nullptr);

        return Literal{to_string_fptr(op_res.result_value),
                       IRI::from_datatype_iri(op_res.result_type_iri, node_storage), // todo translate
                       node_storage};
    } else {
        auto const other_entry = DatatypeRegistry::get_entry(other_datatype);
        assert(other_entry != nullptr);

        if (!other_entry->numeric_ops.has_value()) {
            return Literal{};  // not numeric
        }

        auto const equalizer = DatatypeRegistry::get_common_type_conversion(this_entry->conversion_table,
                                                                            other_entry->conversion_table);

        if (!equalizer.has_value()) {
            return Literal{};  // not convertible
        }

        auto const [equalized_entry, equalized_iri] = [&]() {
            if (equalizer->target_type_iri == this_datatype) {
                return std::make_pair(this_entry, this_datatype);
            } else if (equalizer->target_type_iri == other_datatype) {
                return std::make_pair(other_entry, other_datatype);
            } else {
                return std::make_pair(DatatypeRegistry::get_entry(equalizer->target_type_iri), equalizer->target_type_iri);
            }
        }();

        assert(equalized_entry != nullptr);
        assert(equalized_entry->numeric_ops.has_value());

        DatatypeRegistry::NumericOpResult const op_res = op_select(*equalized_entry->numeric_ops)(equalizer->convert_lhs(this->value()),
                                                                                                  equalizer->convert_rhs(other.value()));

        auto const to_string_fptr = op_res.result_type_iri == equalized_iri
                                            ? equalized_entry->to_string_fptr
                                            : DatatypeRegistry::get_to_string(op_res.result_type_iri);

        assert(to_string_fptr != nullptr);

        return Literal{to_string_fptr(op_res.result_value),
                       IRI::from_datatype_iri(op_res.result_type_iri, node_storage),
                       node_storage};
    }
}

template<typename OpSelect>
Literal Literal::numeric_unop_impl(OpSelect op_select, NodeStorage &node_storage) const {
    using namespace datatypes::registry;

    if (this->null()) {
        return Literal{};
    }

    auto const this_datatype = this->get_datatype_iri();
    auto const this_entry = DatatypeRegistry::get_entry(this_datatype);
    assert(this_entry != nullptr);

    if (!this_entry->numeric_ops.has_value()) {
        return Literal{};  // this_datatype not numeric
    }

    DatatypeRegistry::NumericOpResult const op_res = op_select(*this_entry->numeric_ops)(this->value());

    auto const to_string_fptr = op_res.result_type_iri == this_datatype
                                        ? this_entry->to_string_fptr
                                        : DatatypeRegistry::get_to_string(op_res.result_type_iri);

    assert(to_string_fptr != nullptr);

    return Literal{to_string_fptr(op_res.result_value),
                   IRI::from_datatype_iri(op_res.result_type_iri, node_storage),
                   node_storage};
}

Literal::TriStateBool Literal::get_ebv_impl() const {
    if (this->null()) {
        return TriStateBool::Err;
    }

    auto const ebv = datatypes::registry::DatatypeRegistry::get_ebv(this->get_datatype_iri());

    if (ebv == nullptr) {
        return TriStateBool::Err;
    }

    return ebv(this->value()) ? TriStateBool::True : TriStateBool::False;
}

Literal Literal::logical_binop_impl(std::array<std::array<TriStateBool, 3>, 3> const &logic_table, Literal const &other, NodeStorage &node_storage) const {
    auto const this_ebv = this->get_ebv_impl();
    auto const other_ebv = other.get_ebv_impl();

    auto const res = logic_table[static_cast<size_t>(this_ebv)][static_cast<size_t>(other_ebv)];

    if (res == TriStateBool::Err) {
        return Literal{};
    }

    return Literal::make<datatypes::xsd::Boolean>(res == TriStateBool::True, node_storage);
}

datatypes::registry::DatatypeIRIView Literal::get_datatype_iri() const noexcept {
    auto const lit_type = this->handle_.node_id().literal_type();

    if (lit_type.is_fixed()) {
        return datatypes::registry::DatatypeIRIView{lit_type};
    } else {
        return datatypes::registry::DatatypeIRIView{this->datatype().identifier()};
    }
}

bool Literal::is_fixed_not_numeric() const noexcept {
    using namespace datatypes::registry;

    auto const lit_type = this->handle_.node_id().literal_type();
    return lit_type.is_fixed() && !lit_type.is_numeric();
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

Literal Literal::effective_boolean_value(NodeStorage &node_storage) const {
    auto const ebv = this->get_ebv_impl();

    if (ebv == TriStateBool::Err) {
        return Literal{};
    }

    return Literal::make<datatypes::xsd::Boolean>(ebv == TriStateBool::True, node_storage);
}

Literal Literal::logical_and(Literal const &other, Node::NodeStorage &node_storage) const {
    constexpr std::array<std::array<TriStateBool, 3>, 3> and_logic_table{
            /* lhs \ rhs               Err                  False                True */
            /* Err       */ std::array{TriStateBool::Err,   TriStateBool::False, TriStateBool::Err},
            /* False     */ std::array{TriStateBool::False, TriStateBool::False, TriStateBool::False},
            /* True      */ std::array{TriStateBool::Err,   TriStateBool::False, TriStateBool::True}};

    return this->logical_binop_impl(and_logic_table, other, node_storage);
}

Literal Literal::operator&&(Literal const &other) const {
    return this->logical_and(other);
}

Literal Literal::logical_or(Literal const &other, Node::NodeStorage &node_storage) const {
    constexpr std::array<std::array<TriStateBool, 3>, 3> or_logic_table{
            /* lhs \ rhs               Err                 False                True */
            /* Err       */ std::array{TriStateBool::Err,  TriStateBool::Err,   TriStateBool::True},
            /* False     */ std::array{TriStateBool::Err,  TriStateBool::False, TriStateBool::True},
            /* True      */ std::array{TriStateBool::True, TriStateBool::True,  TriStateBool::True}};

    return this->logical_binop_impl(or_logic_table, other, node_storage);
}

Literal Literal::operator||(Literal const &other) const {
    return this->logical_or(other);
}

Literal Literal::logical_not(Node::NodeStorage &node_storage) const {
    TriStateBool const ebv = this->get_ebv_impl();

    if (ebv == TriStateBool::Err) {
        return Literal{};
    }

    return Literal::make<datatypes::xsd::Boolean>(ebv == TriStateBool::False, node_storage);
}

Literal Literal::operator!() const {
    return this->logical_not();
}

}  // namespace rdf4cpp::rdf