#include "Literal.hpp"

#include <sstream>

#include <rdf4cpp/rdf/IRI.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/LiteralBackend.hpp>

namespace rdf4cpp::rdf {

Literal::Literal() noexcept
    : Node(NodeBackendHandle{{}, storage::node::identifier::RDFNodeType::Literal, {}}) {}

Literal::Literal(Node::NodeBackendHandle handle)
    : Node(handle) {}

Literal::Literal(std::string_view lexical_form, Node::NodeStorage &node_storage)
    : Literal{make_simple_unchecked(lexical_form, node_storage)} {}

Literal::Literal(std::string_view lexical_form, const IRI &datatype, Node::NodeStorage &node_storage)
    : Literal(make(lexical_form, datatype, node_storage)) {}

Literal::Literal(std::string_view lexical_form, std::string_view lang, Node::NodeStorage &node_storage)
    : Literal{make_lang_tagged_unchecked(lexical_form, lang, node_storage)} {}

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
    auto const quoted_lexical_into_stream = [](std::ostream &out, std::string_view const lexical) {
        // TODO: escape everything that needs to be escaped in N-Tripels/N-Quads

        out << "\"";
        for (auto const character : lexical) {
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
    };

    const auto &literal = handle_.literal_backend();
    std::ostringstream oss;

    quoted_lexical_into_stream(oss, literal.lexical_form);

    if (literal.datatype_id == NodeID::rdf_langstring_iri.first) {
        if (!literal.language_tag.empty()) {
            oss << "@" << literal.language_tag;
        }
    } else {
        auto const &dtype_iri = NodeStorage::find_iri_backend_view(NodeBackendHandle{literal.datatype_id, storage::node::identifier::RDFNodeType::IRI, backend_handle().node_storage_id()});
        oss << "^^" << dtype_iri.n_string();
    }

    return oss.str();
}
bool Literal::is_literal() const { return true; }
bool Literal::is_variable() const { return false; }
bool Literal::is_blank_node() const { return false; }
bool Literal::is_iri() const { return false; }
bool Literal::is_numeric() const {
    using namespace datatypes::registry;

    return this->handle_.node_id().literal_type().is_numeric()
           || DatatypeRegistry::get_numerical_ops(this->get_datatype_id()) != nullptr;
}

std::ostream &operator<<(std::ostream &os, const Literal &literal) {
    os << static_cast<std::string>(literal);
    return os;
}
std::any Literal::value() const {
    using namespace datatypes;

    auto const datatype = this->get_datatype_id();

    if (datatype == rdf::LangString::datatype_id) {
        auto const &lit = this->handle_.literal_backend();

        return registry::LangStringRepr{
            .lexical_form = std::string{lit.lexical_form},
            .language_tag = std::string{lit.language_tag}};
    } else if (auto const factory = registry::DatatypeRegistry::get_factory(datatype); factory != nullptr) {
        return factory(this->lexical_form());
    } else {
        return {};
    }
}

Literal Literal::make_simple_unchecked(std::string_view lexical_form, Node::NodeStorage &node_storage) {
    return Literal{NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::LiteralBackendView{
                                             .datatype_id = storage::node::identifier::NodeID::xsd_string_iri.first,
                                             .lexical_form = lexical_form,
                                             .language_tag = ""}),
                                     storage::node::identifier::RDFNodeType::Literal,
                                     node_storage.id()}};
}

Literal Literal::make_typed_unchecked(std::string_view lexical_form, IRI const &datatype, Node::NodeStorage &node_storage) {
    return Literal{NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::LiteralBackendView{
                                             .datatype_id = datatype.to_node_storage(node_storage).backend_handle().node_id(),
                                             .lexical_form = lexical_form,
                                             .language_tag = ""}),
                                     storage::node::identifier::RDFNodeType::Literal,
                                     node_storage.id()}};
}

Literal Literal::make_lang_tagged_unchecked(std::string_view lexical_form, std::string_view lang, Node::NodeStorage &node_storage) {
    return Literal{NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::LiteralBackendView{
                                             .datatype_id = storage::node::identifier::NodeID::rdf_langstring_iri.first,
                                             .lexical_form = lexical_form,
                                             .language_tag = lang}),
                                     storage::node::identifier::RDFNodeType::Literal,
                                     node_storage.id()}};
}

Literal Literal::make(std::string_view lexical_form, const IRI &datatype, Node::NodeStorage &node_storage) {
    using namespace datatypes::registry;

    DatatypeIDView const datatype_identifier = datatype.to_datatype_id();

    if (datatype_identifier == datatypes::rdf::LangString::datatype_id) {
        // see: https://www.w3.org/TR/rdf11-concepts/#section-Graph-Literal
        throw std::invalid_argument{"cannot construct rdf:langString without a language tag, please call one of the other constructors"};
    }

    if (auto const *entry = DatatypeRegistry::get_entry(datatype_identifier); entry != nullptr) {
        // exists => canonize
        auto const cpp_value = entry->factory_fptr(lexical_form);
        auto const canonical_lexical_form = entry->to_string_fptr(cpp_value);

        return Literal::make_typed_unchecked(canonical_lexical_form, datatype, node_storage);
    } else {
        // doesn't exist in the registry no way to canonicalize
        return Literal::make_typed_unchecked(lexical_form, datatype, node_storage);
    }
}

template<typename OpSelect>
Literal Literal::numeric_binop_impl(OpSelect op_select, Literal const &other, NodeStorage &node_storage) const {
    using namespace datatypes::registry;

    if (this->null() || other.null() || this->is_fixed_not_numeric() || other.is_fixed_not_numeric()) {
        return Literal{};
    }

    auto const this_datatype = this->get_datatype_id();
    auto const this_entry = DatatypeRegistry::get_entry(this_datatype);
    assert(this_entry != nullptr);

    if (!this_entry->numeric_ops.has_value()) {
        return Literal{};  // not numeric
    }

    auto const other_datatype = other.get_datatype_id();

    if (this_datatype == other_datatype && this_entry->numeric_ops->is_impl()) {
        DatatypeRegistry::NumericOpResult const op_res = op_select(this_entry->numeric_ops->get_impl())(this->value(),
                                                                                                        other.value());

        if (!op_res.result_value.has_value()) {
            return Literal{};
        }

        auto const to_string_fptr = [&]() {
            if (op_res.result_type_id == this_datatype) [[likely]] {
                return this_entry->to_string_fptr;
            } else [[unlikely]] {
                return DatatypeRegistry::get_to_string(op_res.result_type_id);
            }
        }();

        assert(to_string_fptr != nullptr);

        return Literal{to_string_fptr(*op_res.result_value),
                       IRI::from_datatype_id(op_res.result_type_id, node_storage),
                       node_storage};
    } else {
        auto const other_entry = DatatypeRegistry::get_entry(other_datatype);
        assert(other_entry != nullptr);

        if (!other_entry->numeric_ops.has_value()) {
            return Literal{};  // not numeric
        }

        auto const equalizer = DatatypeRegistry::get_common_numeric_op_type_conversion(*this_entry,
                                                                                       *other_entry);

        if (!equalizer.has_value()) {
            return Literal{};  // not convertible
        }

        auto const [equalized_entry, equalized_id] = [&]() {
            if (equalizer->target_type_id == this_datatype) {
                return std::make_pair(this_entry, this_datatype);
            } else if (equalizer->target_type_id == other_datatype) {
                return std::make_pair(other_entry, other_datatype);
            } else {
                return std::make_pair(DatatypeRegistry::get_entry(equalizer->target_type_id), equalizer->target_type_id);
            }
        }();

        assert(equalized_entry != nullptr);
        assert(equalized_entry->numeric_ops.has_value());
        assert(equalized_entry->numeric_ops->is_impl());

        DatatypeRegistry::NumericOpResult const op_res = op_select(equalized_entry->numeric_ops->get_impl())(equalizer->convert_lhs(this->value()),
                                                                                                             equalizer->convert_rhs(other.value()));

        if (!op_res.result_value.has_value()) {
            return Literal{};
        }

        auto const to_string_fptr = [&op_res, equalized_id = std::ref(equalized_id), equalized_entry = equalized_entry]() {
            if (op_res.result_type_id == equalized_id.get()) [[likely]] {
                return equalized_entry->to_string_fptr;
            } else [[unlikely]] {
                return DatatypeRegistry::get_to_string(op_res.result_type_id);
            }
        }();

        assert(to_string_fptr != nullptr);

        return Literal{to_string_fptr(*op_res.result_value),
                       IRI::from_datatype_id(op_res.result_type_id, node_storage),
                       node_storage};
    }
}

template<typename OpSelect>
Literal Literal::numeric_unop_impl(OpSelect op_select, NodeStorage &node_storage) const {
    using namespace datatypes::registry;

    if (this->null()) {
        return Literal{};
    }

    auto const this_datatype = this->get_datatype_id();
    auto const this_entry = DatatypeRegistry::get_entry(this_datatype);
    assert(this_entry != nullptr);

    if (!this_entry->numeric_ops.has_value()) {
        return Literal{};  // this_datatype not numeric
    }

    auto const [operand_entry, value] = [&]() {
        if (this_entry->numeric_ops->is_stub()) {
            auto const impl_converter = DatatypeRegistry::get_numeric_op_impl_conversion(*this_entry);
            auto const target_num_ops = DatatypeRegistry::get_entry(impl_converter.target_type_id);

            return std::make_pair(target_num_ops, impl_converter.convert(this->value()));
        } else {
            return std::make_pair(this_entry, this->value());
        }
    }();

    assert(operand_entry != nullptr);
    assert(operand_entry->numeric_ops.has_value());
    assert(operand_entry->numeric_ops->is_impl());

    DatatypeRegistry::NumericOpResult const op_res = op_select(operand_entry->numeric_ops->get_impl())(value);

    auto const to_string_fptr = [&op_res, operand_entry = operand_entry]() {
        if (op_res.result_type_id == DatatypeIDView{operand_entry->datatype_iri}) [[likely]] {
            return operand_entry->to_string_fptr;
        } else [[unlikely]] {
            return DatatypeRegistry::get_to_string(op_res.result_type_id);
        }
    }();

    assert(to_string_fptr != nullptr);

    return Literal{to_string_fptr(*op_res.result_value),
                   IRI::from_datatype_id(op_res.result_type_id, node_storage)};
}

std::partial_ordering Literal::compare_impl(Literal const &other, std::strong_ordering *out_alternative_ordering) const {
    using datatypes::registry::DatatypeRegistry;

    if (this->handle_.null() || other.handle_.null()) {
        if (this->handle_ == other.handle_) {
            return std::partial_ordering::equivalent;
        } else {
            if (out_alternative_ordering != nullptr) {
                // ordering extensions (for e.g. ORDER BY) require that null nodes
                // are always the smallest node
                *out_alternative_ordering = this->handle_.null()
                                             ? std::strong_ordering::less
                                             : std::strong_ordering::greater;
            }
            return std::partial_ordering::unordered;
        }
    }

    auto const this_datatype = this->get_datatype_id();
    auto const other_datatype = other.get_datatype_id();

    std::strong_ordering const datatype_cmp_res = this_datatype <=> other_datatype;

    auto const this_entry = DatatypeRegistry::get_entry(this_datatype);

    if (datatype_cmp_res == std::strong_ordering::equal) {
        if (out_alternative_ordering != nullptr) {
            // types equal, fallback to lexical form ordering
            *out_alternative_ordering = this->lexical_form() <=> other.lexical_form();
        }

        if (this_entry == nullptr || this_entry->compare_fptr == nullptr) {
            return std::partial_ordering::unordered;
        }

        return this_entry->compare_fptr(this->value(), other.value());
    } else {
        if (out_alternative_ordering != nullptr) {
            // types are different, the only useful alternative ordering is the type ordering
            *out_alternative_ordering = datatype_cmp_res;
        }

        auto const other_entry = DatatypeRegistry::get_entry(other_datatype);

        if (this_entry == nullptr || this_entry->compare_fptr == nullptr || other_entry == nullptr || other_entry->compare_fptr == nullptr) {
            return std::partial_ordering::unordered;
        }

        auto const equalizer = DatatypeRegistry::get_common_type_conversion(this_entry->conversion_table,
                                                                            other_entry->conversion_table);

        if (!equalizer.has_value()) {
            return std::partial_ordering::unordered; // not convertible to common type
        }

        auto const equalized_compare_fptr = [&]() {
            if (equalizer->target_type_id == this_datatype) {
                return this_entry->compare_fptr;
            }  else if (equalizer->target_type_id == other_datatype) {
                return other_entry->compare_fptr;
            } else {
                return DatatypeRegistry::get_compare(equalizer->target_type_id);
            }
        }();

        assert(equalized_compare_fptr != nullptr);

        return equalized_compare_fptr(equalizer->convert_lhs(this->value()),
                                      equalizer->convert_rhs(other.value()));
    }
}

std::partial_ordering Literal::compare(Literal const &other) const {
    return this->compare_impl(other);
}

std::partial_ordering Literal::operator<=>(Literal const &other) const {
    return this->compare(other);
}

std::weak_ordering Literal::compare_with_extensions(Literal const &other) const {
    // default to equivalent; as required by compare_impl
    // see doc for compare_impl
    std::strong_ordering alternative_cmp_res = std::strong_ordering::equivalent;
    auto const cmp_res = this->compare_impl(other, &alternative_cmp_res);

    if (cmp_res == std::partial_ordering::equivalent || cmp_res == std::partial_ordering::unordered) {
        // return alternative ordering instead
        return alternative_cmp_res;
    } else if (cmp_res == std::partial_ordering::less) {
        return std::weak_ordering::less;
    } else { // cmp_res == std::partial_ordering::greater
        return std::weak_ordering::greater;
    }
}

util::TriBool Literal::eq(Literal const &other) const {
    return util::partial_weak_ordering_eq(this->compare(other), std::weak_ordering::equivalent);
}

util::TriBool Literal::operator==(Literal const &other) const {
    return this->eq(other);
}

util::TriBool Literal::ne(Literal const &other) const {
    return !util::partial_weak_ordering_eq(this->compare(other), std::weak_ordering::equivalent);
}

util::TriBool Literal::operator!=(Literal const &other) const {
    return this->ne(other);
}

util::TriBool Literal::lt(Literal const &other) const {
    return util::partial_weak_ordering_eq(this->compare(other), std::weak_ordering::less);
}

util::TriBool Literal::operator<(Literal const &other) const {
    return this->lt(other);
}

util::TriBool Literal::le(Literal const &other) const {
    return !util::partial_weak_ordering_eq(this->compare(other), std::weak_ordering::greater);
}

util::TriBool Literal::operator<=(Literal const &other) const {
    return this->le(other);
}

util::TriBool Literal::gt(Literal const &other) const {
    return util::partial_weak_ordering_eq(this->compare(other), std::weak_ordering::greater);
}

util::TriBool Literal::operator>(Literal const &other) const {
    return this->gt(other);
}

util::TriBool Literal::ge(Literal const &other) const {
    return !util::partial_weak_ordering_eq(this->compare(other), std::weak_ordering::less);
}

util::TriBool Literal::operator>=(Literal const &other) const {
    return this->ge(other);
}

bool Literal::eq_with_extensions(Literal const &other) const {
    return this->compare_with_extensions(other) == std::weak_ordering::equivalent;
}

bool Literal::ne_with_extensions(Literal const &other) const {
    return this->compare_with_extensions(other) != std::weak_ordering::equivalent;
}

bool Literal::lt_with_extensions(Literal const &other) const {
    return this->compare_with_extensions(other) == std::weak_ordering::less;
}

bool Literal::le_with_extensions(Literal const &other) const {
    return this->compare_with_extensions(other) != std::weak_ordering::greater;
}

bool Literal::gt_with_extensions(Literal const &other) const {
    return this->compare_with_extensions(other) == std::weak_ordering::greater;
}

bool Literal::ge_with_extensions(Literal const &other) const {
    return this->compare_with_extensions(other) != std::weak_ordering::less;
}

datatypes::registry::DatatypeIDView Literal::get_datatype_id() const noexcept {
    auto const lit_type = this->handle_.node_id().literal_type();

    if (lit_type.is_fixed()) {
        return datatypes::registry::DatatypeIDView{lit_type};
    } else {
        return datatypes::registry::DatatypeIDView{this->datatype().identifier()};
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

util::TriBool Literal::ebv() const noexcept {
    if (this->null()) {
        return util::TriBool::Err;
    }

    auto const ebv = datatypes::registry::DatatypeRegistry::get_ebv(this->get_datatype_id());

    if (ebv == nullptr) {
        return util::TriBool::Err;
    }

    return ebv(this->value()) ? util::TriBool::True : util::TriBool::False;
}

Literal Literal::ebv_as_literal(NodeStorage &node_storage) const {
    auto const ebv = this->ebv();

    if (ebv == util::TriBool::Err) {
        return Literal{};
    }

    return Literal::make<datatypes::xsd::Boolean>(ebv == util::TriBool::True, node_storage);
}

Literal Literal::logical_and(Literal const &other, Node::NodeStorage &node_storage) const {
    auto const res = this->ebv() && other.ebv();

    if (res == util::TriBool::Err) {
        return Literal{};
    }

    return Literal::make<datatypes::xsd::Boolean>(res, node_storage);
}

Literal Literal::operator&&(Literal const &other) const {
    return this->logical_and(other);
}

Literal Literal::logical_or(Literal const &other, Node::NodeStorage &node_storage) const {
    auto const res = this->ebv() || other.ebv();

    if (res == util::TriBool::Err) {
        return Literal{};
    }

    return Literal::make<datatypes::xsd::Boolean>(res, node_storage);
}

Literal Literal::operator||(Literal const &other) const {
    return this->logical_or(other);
}

Literal Literal::logical_not(Node::NodeStorage &node_storage) const {
    auto const ebv = this->ebv();

    if (ebv == util::TriBool::Err) {
        return Literal{};
    }

    return Literal::make<datatypes::xsd::Boolean>(ebv == util::TriBool::False, node_storage);
}

Literal Literal::operator!() const {
    return this->logical_not();
}
}  // namespace rdf4cpp::rdf