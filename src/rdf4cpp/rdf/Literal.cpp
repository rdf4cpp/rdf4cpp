#include "Literal.hpp"

#include <cwchar>
#include <sstream>

#include <utf8cpp/utf8.h>

#include <rdf4cpp/rdf/IRI.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/LiteralBackend.hpp>

namespace rdf4cpp::rdf {

Literal::Literal() noexcept
    : Node(NodeBackendHandle{{}, storage::node::identifier::RDFNodeType::Literal, {}}) {}

Literal::Literal(Node::NodeBackendHandle handle) noexcept
    : Node{handle} {}

Literal::Literal(std::string_view lexical_form, Node::NodeStorage &node_storage)
    : Literal{make_simple_unchecked(lexical_form, node_storage)} {}

Literal::Literal(std::string_view lexical_form, const IRI &datatype, Node::NodeStorage &node_storage)
    : Literal{make(lexical_form, datatype, node_storage)} {}

Literal::Literal(std::string_view lexical_form, std::string_view lang, Node::NodeStorage &node_storage)
    : Literal{make_lang_tagged_unchecked(lexical_form, lang, node_storage)} {}

IRI Literal::datatype() const noexcept {
    NodeBackendHandle iri_handle{handle_.literal_backend().datatype_id, storage::node::identifier::RDFNodeType::IRI, backend_handle().node_storage_id()};
    return IRI{iri_handle};
}

std::string_view Literal::lexical_form() const noexcept {
    return handle_.literal_backend().lexical_form;
}

Literal Literal::as_lexical_form(NodeStorage &node_storage) const {
    return Literal::make<datatypes::xsd::String>(std::string{this->lexical_form()}, node_storage);
}

std::string_view Literal::language_tag() const noexcept {
    return handle_.literal_backend().language_tag;
}

Literal::operator std::string() const noexcept {
    // TODO: escape non-standard chars correctly
    auto const quoted_lexical_into_stream = [](std::ostream &out, std::string_view const lexical) noexcept {
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
bool Literal::is_literal() const noexcept { return true; }
bool Literal::is_variable() const noexcept { return false; }
bool Literal::is_blank_node() const noexcept { return false; }
bool Literal::is_iri() const noexcept { return false; }
bool Literal::is_numeric() const noexcept {
    using namespace datatypes::registry;

    return this->handle_.node_id().literal_type().is_numeric()
           || DatatypeRegistry::get_numerical_ops(this->datatype_id()) != nullptr;
}

std::ostream &operator<<(std::ostream &os, const Literal &literal) {
    os << static_cast<std::string>(literal);
    return os;
}
std::any Literal::value() const {
    using namespace datatypes;

    auto const datatype = this->datatype_id();

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

Literal Literal::make_simple_unchecked(std::string_view lexical_form, Node::NodeStorage &node_storage) noexcept {
    return Literal{NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::LiteralBackendView{
                                             .datatype_id = storage::node::identifier::NodeID::xsd_string_iri.first,
                                             .lexical_form = lexical_form,
                                             .language_tag = ""}),
                                     storage::node::identifier::RDFNodeType::Literal,
                                     node_storage.id()}};
}

Literal Literal::make_typed_unchecked(std::string_view lexical_form, IRI const &datatype, Node::NodeStorage &node_storage) noexcept {
    return Literal{NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::LiteralBackendView{
                                             .datatype_id = datatype.to_node_storage(node_storage).backend_handle().node_id(),
                                             .lexical_form = lexical_form,
                                             .language_tag = ""}),
                                     storage::node::identifier::RDFNodeType::Literal,
                                     node_storage.id()}};
}

Literal Literal::make_lang_tagged_unchecked(std::string_view lexical_form, std::string_view lang, Node::NodeStorage &node_storage) noexcept {
    return Literal{NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::LiteralBackendView{
                                             .datatype_id = storage::node::identifier::NodeID::rdf_langstring_iri.first,
                                             .lexical_form = lexical_form,
                                             .language_tag = lang}),
                                     storage::node::identifier::RDFNodeType::Literal,
                                     node_storage.id()}};
}

Literal Literal::make_string_like_copy_lang_tag(std::string_view str, Literal const &lang_tag_src, Node::NodeStorage &node_storage) noexcept {
    auto const lt_dt = lang_tag_src.datatype_id();

    if (lt_dt == datatypes::rdf::LangString::datatype_id) {
        return Literal::make_lang_tagged_unchecked(str, lang_tag_src.language_tag(), node_storage);
    }

    assert(lt_dt == datatypes::xsd::String::datatype_id);
    return Literal::make_simple_unchecked(str, node_storage);
}

Literal Literal::make(std::string_view lexical_form, const IRI &datatype, Node::NodeStorage &node_storage) {
    using namespace datatypes::registry;

    DatatypeIDView const datatype_identifier{datatype};

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

Literal Literal::make_boolean(util::TriBool const b, Node::NodeStorage &node_storage) {
    if (b == util::TriBool::Err) {
        return Literal{};
    }

    return Literal::make<datatypes::xsd::Boolean>(b == util::TriBool::True, node_storage);
}

Literal Literal::cast(IRI const &target, Node::NodeStorage &node_storage) const noexcept {
    using namespace datatypes::registry;
    using namespace datatypes::xsd;

    if (this->null()) {
        return Literal{};
    }

    auto const this_dtid = this->datatype_id();
    DatatypeIDView const target_dtid{target};

    if (this_dtid == target_dtid) {
        return static_cast<Literal>(this->to_node_storage(node_storage));
    }

    if (target_dtid == Boolean::datatype_id) {
        // any -> bool
        return this->ebv_as_literal(node_storage);
    }

    if (target_dtid == String::datatype_id) {
        // any -> string
        return this->as_lexical_form(node_storage);
    }

    if (this_dtid == String::datatype_id) {
        // string -> any
        try {
            return Literal::make(this->lexical_form(), target, node_storage);
        } catch (std::runtime_error const &) {
            return Literal{};
        }
    }

    auto const *target_e = DatatypeRegistry::get_entry(target_dtid);
    if (target_e == nullptr) {
        // target not registered
        return Literal{};
    }

    if (this_dtid == Boolean::datatype_id && target_e->numeric_ops.has_value()) {
        // bool -> numeric
        if (target_e->numeric_ops->is_impl()) {
            auto const value = this->template value<Boolean>() ? target_e->numeric_ops->get_impl().one_value_fptr()
                                                               : target_e->numeric_ops->get_impl().zero_value_fptr();

            return Literal::make_typed_unchecked(target_e->to_string_fptr(value), target, node_storage);
        } else {
            auto const &impl_converter = DatatypeRegistry::get_numeric_op_impl_conversion(*target_e);
            auto const *target_num_impl = DatatypeRegistry::get_numerical_ops(impl_converter.target_type_id);
            assert(target_num_impl != nullptr);

            // perform conversion as impl numeric type
            auto const value = this->template value<Boolean>() ? target_num_impl->get_impl().one_value_fptr()
                                                               : target_num_impl->get_impl().zero_value_fptr();

            // downcast to target
            auto const target_value = impl_converter.inverted_convert(value);

            if (!target_value.has_value()) {
                // not representable as target type
                return Literal{};
            }

            return Literal::make_typed_unchecked(target_e->to_string_fptr(*target_value), target, node_storage);
        }
    }

    auto const *this_e = DatatypeRegistry::get_entry(this_dtid);
    if (this_e == nullptr) {
        // this datatype not registered
        return Literal{};
    }

    if (auto const common_conversion = DatatypeRegistry::get_common_type_conversion(this_e->conversion_table, target_e->conversion_table); common_conversion.has_value()) {
        // general cast
        // TODO: if performance is bad split into separate cases for up-, down- and cross-casting to avoid one set of std::any wrapping and unwrapping for the former 2

        auto const common_type_value = common_conversion->convert_lhs(this->value()); // upcast to common
        auto const target_value = common_conversion->inverted_convert_rhs(common_type_value); // downcast to target
        if (!target_value.has_value()) {
            // downcast failed
            return Literal{};
        }
        return Literal::make_typed_unchecked(target_e->to_string_fptr(*target_value), target, node_storage);
    }

    // no conversion found
    return Literal{};
}

template<typename OpSelect>
    requires std::is_nothrow_invocable_r_v<datatypes::registry::DatatypeRegistry::binop_fptr_t, OpSelect, datatypes::registry::DatatypeRegistry::NumericOpsImpl const &>
Literal Literal::numeric_binop_impl(OpSelect op_select, Literal const &other, NodeStorage &node_storage) const noexcept {
    using namespace datatypes::registry;

    if (this->null() || other.null() || this->is_fixed_not_numeric() || other.is_fixed_not_numeric()) {
        return Literal{};
    }

    auto const this_datatype = this->datatype_id();
    auto const this_entry = DatatypeRegistry::get_entry(this_datatype);
    assert(this_entry != nullptr);

    if (!this_entry->numeric_ops.has_value()) {
        return Literal{};  // not numeric
    }

    auto const other_datatype = other.datatype_id();

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

        return Literal::make_typed_unchecked(to_string_fptr(*op_res.result_value),
                                             IRI{op_res.result_type_id, node_storage},
                                             node_storage);
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

        return Literal::make_typed_unchecked(to_string_fptr(*op_res.result_value),
                                             IRI{op_res.result_type_id, node_storage},
                                             node_storage);
    }
}

template<typename OpSelect>
    requires std::is_nothrow_invocable_r_v<datatypes::registry::DatatypeRegistry::unop_fptr_t, OpSelect, datatypes::registry::DatatypeRegistry::NumericOpsImpl const &>
Literal Literal::numeric_unop_impl(OpSelect op_select, NodeStorage &node_storage) const noexcept {
    using namespace datatypes::registry;

    if (this->null()) {
        return Literal{};
    }

    auto const this_datatype = this->datatype_id();
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

    return Literal::make_typed_unchecked(to_string_fptr(*op_res.result_value),
                                         IRI{op_res.result_type_id, node_storage},
                                         node_storage);
}

std::partial_ordering Literal::compare_impl(Literal const &other, std::strong_ordering *out_alternative_ordering) const noexcept {
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

    auto const this_datatype = this->datatype_id();
    auto const other_datatype = other.datatype_id();

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

std::partial_ordering Literal::compare(Literal const &other) const noexcept {
    return this->compare_impl(other);
}

std::partial_ordering Literal::operator<=>(Literal const &other) const noexcept {
    return this->compare(other);
}

std::weak_ordering Literal::compare_with_extensions(Literal const &other) const noexcept {
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

util::TriBool Literal::eq(Literal const &other) const noexcept {
    return util::partial_weak_ordering_eq(this->compare(other), std::weak_ordering::equivalent);
}

util::TriBool Literal::operator==(Literal const &other) const noexcept {
    return this->eq(other);
}

util::TriBool Literal::ne(Literal const &other) const noexcept {
    return !util::partial_weak_ordering_eq(this->compare(other), std::weak_ordering::equivalent);
}

util::TriBool Literal::operator!=(Literal const &other) const noexcept {
    return this->ne(other);
}

util::TriBool Literal::lt(Literal const &other) const noexcept {
    return util::partial_weak_ordering_eq(this->compare(other), std::weak_ordering::less);
}

util::TriBool Literal::operator<(Literal const &other) const noexcept {
    return this->lt(other);
}

util::TriBool Literal::le(Literal const &other) const noexcept {
    return !util::partial_weak_ordering_eq(this->compare(other), std::weak_ordering::greater);
}

util::TriBool Literal::operator<=(Literal const &other) const noexcept {
    return this->le(other);
}

util::TriBool Literal::gt(Literal const &other) const noexcept {
    return util::partial_weak_ordering_eq(this->compare(other), std::weak_ordering::greater);
}

util::TriBool Literal::operator>(Literal const &other) const noexcept {
    return this->gt(other);
}

util::TriBool Literal::ge(Literal const &other) const noexcept {
    return !util::partial_weak_ordering_eq(this->compare(other), std::weak_ordering::less);
}

util::TriBool Literal::operator>=(Literal const &other) const noexcept {
    return this->ge(other);
}

bool Literal::eq_with_extensions(Literal const &other) const noexcept {
    return this->compare_with_extensions(other) == std::weak_ordering::equivalent;
}

bool Literal::ne_with_extensions(Literal const &other) const noexcept {
    return this->compare_with_extensions(other) != std::weak_ordering::equivalent;
}

bool Literal::lt_with_extensions(Literal const &other) const noexcept {
    return this->compare_with_extensions(other) == std::weak_ordering::less;
}

bool Literal::le_with_extensions(Literal const &other) const noexcept {
    return this->compare_with_extensions(other) != std::weak_ordering::greater;
}

bool Literal::gt_with_extensions(Literal const &other) const noexcept {
    return this->compare_with_extensions(other) == std::weak_ordering::greater;
}

bool Literal::ge_with_extensions(Literal const &other) const noexcept {
    return this->compare_with_extensions(other) != std::weak_ordering::less;
}

datatypes::registry::DatatypeIDView Literal::datatype_id() const noexcept {
    assert(!this->null());
    auto const lit_type = this->handle_.node_id().literal_type();

    if (lit_type.is_fixed()) {
        return datatypes::registry::DatatypeIDView{lit_type};
    } else {
        return datatypes::registry::DatatypeIDView{this->datatype().identifier()};
    }
}

bool Literal::is_fixed_not_numeric() const noexcept {
    using namespace datatypes::registry;

    assert(!this->null());
    auto const lit_type = this->handle_.node_id().literal_type();
    return lit_type.is_fixed() && !lit_type.is_numeric();
}

bool Literal::is_string_like() const noexcept {
    if (this->null()) {
        return false;
    }

    auto const dt = this->datatype_id();
    return dt == datatypes::xsd::String::datatype_id || dt == datatypes::rdf::LangString::datatype_id;
}

Literal Literal::add(Literal const &other, Node::NodeStorage &node_storage) const noexcept {
    return this->numeric_binop_impl([](auto const &num_ops) noexcept {
        return num_ops.add_fptr;
    }, other, node_storage);
}

Literal Literal::operator+(Literal const &other) const noexcept {
    return this->add(other);
}

Literal Literal::sub(Literal const &other, Node::NodeStorage &node_storage) const noexcept {
    return this->numeric_binop_impl([](auto const &num_ops) noexcept {
        return num_ops.sub_fptr;
    }, other, node_storage);
}

Literal Literal::operator-(Literal const &other) const noexcept {
    return this->sub(other);
}

Literal Literal::mul(Literal const &other, Node::NodeStorage &node_storage) const noexcept {
    return this->numeric_binop_impl([](auto const &num_ops) noexcept {
        return num_ops.mul_fptr;
    }, other, node_storage);
}

Literal Literal::operator*(Literal const &other) const noexcept {
    return this->mul(other);
}

Literal Literal::div(Literal const &other, Node::NodeStorage &node_storage) const noexcept {
    return this->numeric_binop_impl([](auto const &num_ops) noexcept {
        return num_ops.div_fptr;
    }, other, node_storage);
}

Literal Literal::operator/(Literal const &other) const noexcept {
    return this->div(other);
}

Literal Literal::pos(Node::NodeStorage &node_storage) const noexcept {
    return this->numeric_unop_impl([](auto const &num_ops) noexcept {
        return num_ops.pos_fptr;
    }, node_storage);
}

Literal Literal::operator+() const noexcept {
    return this->pos();
}

Literal Literal::neg(Node::NodeStorage &node_storage) const noexcept {
    return this->numeric_unop_impl([](auto const &num_ops) noexcept {
        return num_ops.neg_fptr;
    }, node_storage);
}

Literal Literal::operator-() const noexcept {
    return this->neg();
}

Literal Literal::abs(Node::NodeStorage &node_storage) const noexcept {
    return this->numeric_unop_impl([](auto const &num_ops) noexcept {
        return num_ops.abs_fptr;
    }, node_storage);
}

Literal Literal::round(Node::NodeStorage &node_storage) const noexcept {
    return this->numeric_unop_impl([](auto const &num_ops) noexcept {
        return num_ops.round_fptr;
    }, node_storage);
}

Literal Literal::floor(Node::NodeStorage &node_storage) const noexcept {
    return this->numeric_unop_impl([](auto const &num_ops) noexcept {
        return num_ops.floor_fptr;
    }, node_storage);
}

Literal Literal::ceil(NodeStorage &node_storage) const noexcept {
    return this->numeric_unop_impl([](auto const &num_ops) noexcept {
        return num_ops.ceil_fptr;
    }, node_storage);
}

util::TriBool Literal::ebv() const noexcept {
    if (this->null()) {
        return util::TriBool::Err;
    }

    auto const ebv = datatypes::registry::DatatypeRegistry::get_ebv(this->datatype_id());

    if (ebv == nullptr) {
        return util::TriBool::Err;
    }

    return ebv(this->value()) ? util::TriBool::True : util::TriBool::False;
}

Literal Literal::ebv_as_literal(NodeStorage &node_storage) const noexcept {
    auto const ebv = this->ebv();

    if (ebv == util::TriBool::Err) {
        return Literal{};
    }

    return Literal::make<datatypes::xsd::Boolean>(ebv == util::TriBool::True, node_storage);
}

Literal Literal::logical_and(Literal const &other, Node::NodeStorage &node_storage) const noexcept {
    auto const res = this->ebv() && other.ebv();

    if (res == util::TriBool::Err) {
        return Literal{};
    }

    return Literal::make<datatypes::xsd::Boolean>(res, node_storage);
}

Literal Literal::operator&&(Literal const &other) const noexcept {
    return this->logical_and(other);
}

Literal Literal::logical_or(Literal const &other, Node::NodeStorage &node_storage) const noexcept {
    auto const res = this->ebv() || other.ebv();

    if (res == util::TriBool::Err) {
        return Literal{};
    }

    return Literal::make<datatypes::xsd::Boolean>(res, node_storage);
}

Literal Literal::operator||(Literal const &other) const noexcept {
    return this->logical_or(other);
}

Literal Literal::logical_not(Node::NodeStorage &node_storage) const noexcept {
    auto const ebv = this->ebv();

    if (ebv == util::TriBool::Err) {
        return Literal{};
    }

    return Literal::make<datatypes::xsd::Boolean>(ebv == util::TriBool::False, node_storage);
}

Literal Literal::operator!() const noexcept {
    return this->logical_not();
}

std::optional<size_t> Literal::strlen() const noexcept {
    if (!this->is_string_like()) {
        return std::nullopt;
    }

    auto const lf = this->lexical_form();
    try {
        return utf8::distance(lf.begin(), lf.end());
    } catch (utf8::exception const &) {
        return std::nullopt;
    }
}

Literal Literal::as_strlen(Node::NodeStorage &node_storage) const noexcept {
    auto const len = this->strlen();
    if (!len.has_value()) {
        return Literal{};
    }

    return Literal::make<datatypes::xsd::Integer>(datatypes::xsd::Integer::cpp_type{*len}, node_storage);
}

util::TriBool Literal::lang_matches(std::string_view const lang_range) const noexcept {
    if (!this->is_string_like()) {
        return util::TriBool::Err;
    }

    auto const lang = this->language_tag();

    if (lang_range.empty()) {
        return lang.empty();
    }

    if (lang_range == "*") {
        return !lang.empty();
    }

    assert(false);
    // TODO: comment in when other PR merged
    /*auto const lang_ci = datatypes::registry::util::LangTagView{lang.data(), lang.size()};
    auto const lang_range_ci = datatypes::registry::util::LangTagView{lang_range.data(), lang_range.size()};

    return lang_ci.starts_with(lang_range_ci) && (lang_ci.size() == lang_range_ci.size() || lang_ci[lang_range_ci.size()] == '-');*/
}

Literal Literal::lang_matches(Literal const &lang_range, Node::NodeStorage &node_storage) const noexcept {
    if (lang_range.datatype_id() != datatypes::xsd::String::datatype_id) {
        return Literal{};
    }

    auto const res = this->lang_matches(lang_range.lexical_form());
    return Literal::make_boolean(res, node_storage);
}

static regex::Regex::flag_type translate_regex_flags(std::string_view const xpath_flags) {
    return std::accumulate(xpath_flags.begin(), xpath_flags.end(), regex::RegexFlags::none(), [](auto facc, char c) {
        switch (c) {
            case 's':
                return facc | regex::RegexFlag::DotAll;
            case 'i':
                return facc | regex::RegexFlag::CaseInsensitive;
            case 'q':
                return facc | regex::RegexFlag::Literal;
            default:
                throw std::runtime_error{std::string{"Encountered unsupported regex flag: "} + c};
        }
    });
}

util::TriBool Literal::regex_match(regex::Regex const &pattern) const noexcept {
    if (!this->is_string_like()) {
        return util::TriBool::Err;
    }

    return pattern.regex_search(this->lexical_form());
}

Literal Literal::regex_match(Literal const &pattern, Literal const &flags, Node::NodeStorage &node_storage) const noexcept {
    if (!this->is_string_like() || !pattern.is_string_like() || !flags.is_string_like()) {
        return Literal{};
    }

    if (pattern.datatype_id() == datatypes::rdf::LangString::datatype_id && this->language_tag() != pattern.language_tag()) {
        return Literal{};
    }

    auto const re = [&]() noexcept -> std::optional<regex::Regex> {
        try {
            auto const regex_flags = translate_regex_flags(flags.lexical_form());
            return regex::Regex{pattern.lexical_form(), regex_flags};
        } catch (std::runtime_error const &) {
            return std::nullopt;
        }
    }();

    if (!re.has_value()) {
        return Literal{};
    }

    return Literal::make_boolean(this->regex_match(*re), node_storage);
}

Literal Literal::regex_replace(regex::RegexReplacer const &replacer, Node::NodeStorage &node_storage) const noexcept {
    if (!this->is_string_like()) {
        return Literal{};
    }

    std::string lf{this->lexical_form()};
    replacer.regex_replace(lf);

    return Literal::make_string_like_copy_lang_tag(lf, *this, node_storage);
}

Literal Literal::regex_replace(Literal const &pattern, Literal const &replacement, Literal const &flags, Node::NodeStorage &node_storage) const noexcept {
    if (!this->is_string_like() || !pattern.is_string_like() || !replacement.is_string_like() || !flags.is_string_like()) {
        return Literal{};
    }

    if (pattern.datatype_id() == datatypes::rdf::LangString::datatype_id && this->language_tag() != pattern.language_tag()) {
        return Literal{};
    }

    auto const re = [&]() noexcept -> std::optional<regex::Regex> {
        try {
            auto const regex_flags = translate_regex_flags(flags.lexical_form());
            return regex::Regex{pattern.lexical_form(), regex_flags};
        } catch (std::runtime_error const &) {
            return std::nullopt;
        }
    }();

    if (!re.has_value()) {
        return Literal{};
    }

    auto const repl = [&]() noexcept -> std::optional<regex::RegexReplacer> {
        try {
            return re->make_replacer(replacement.lexical_form());
        } catch (std::runtime_error const &) {
            return std::nullopt;
        }
    }();

    if (!repl.has_value()) {
        return Literal{};
    }

    return this->regex_replace(*repl, node_storage);
}

util::TriBool Literal::contains(std::string_view const needle) const noexcept {
    if (!this->is_string_like()) {
        return util::TriBool::Err;
    }

    auto const s = this->lexical_form();
    return s.find(needle) != std::string_view::npos;
}

Literal Literal::contains(Literal const &needle, Node::NodeStorage &node_storage) const noexcept {
    if (needle.datatype_id() == datatypes::rdf::LangString::datatype_id && this->language_tag() != needle.language_tag()) {
        return Literal{};
    }

    return Literal::make_boolean(this->contains(needle.lexical_form()), node_storage);
}

Literal Literal::substr_before(std::string_view const needle, Node::NodeStorage &node_storage) const noexcept {
    if (!this->is_string_like()) {
        return Literal{};
    }

    if (needle.empty()) {
        return Literal::make_string_like_copy_lang_tag("", *this, node_storage);
    }

    auto const s = this->lexical_form();
    auto const pos = s.find(needle);

    if (pos == std::string_view::npos) {
        return Literal::make_simple_unchecked("", node_storage);
    }

    auto const substr = s.substr(0, pos);
    return Literal::make_string_like_copy_lang_tag(substr, *this, node_storage);
}

Literal Literal::substr_before(Literal const &needle, Node::NodeStorage &node_storage) const noexcept {
    if (needle.datatype_id() == datatypes::rdf::LangString::datatype_id && this->language_tag() != needle.language_tag()) {
        return Literal{};
    }

    return this->substr_before(needle.lexical_form(), node_storage);
}

Literal Literal::substr_after(std::string_view const needle, Node::NodeStorage &node_storage) const noexcept {
    if (!this->is_string_like()) {
        return Literal{};
    }

    if (needle.empty()) {
        return *this;
    }

    auto const s = this->lexical_form();
    auto const pos = s.find(needle);

    if (pos == std::string_view::npos) {
        return Literal::make_simple_unchecked("", node_storage);
    }

    auto const substr = s.substr(pos + needle.size());
    return Literal::make_string_like_copy_lang_tag(substr, *this, node_storage);
}

Literal Literal::substr_after(Literal const &needle, Node::NodeStorage &node_storage) const noexcept {
    if (needle.datatype_id() == datatypes::rdf::LangString::datatype_id && this->language_tag() != needle.language_tag()) {
        return Literal{};
    }

    return this->substr_after(needle.lexical_form(), node_storage);
}

util::TriBool Literal::str_starts_with(std::string_view const needle) const noexcept {
    if (!this->is_string_like()) {
        return util::TriBool::Err;
    }

    auto const s = this->lexical_form();
    return s.starts_with(needle);
}

Literal Literal::str_starts_with(Literal const &needle, Node::NodeStorage &node_storage) const noexcept {
    if (!needle.is_string_like()) {
        return Literal{};
    }

    if (needle.datatype_id() == datatypes::rdf::LangString::datatype_id && this->language_tag() != needle.language_tag()) {
        return Literal{};
    }

    return Literal::make_boolean(this->str_starts_with(needle.lexical_form()), node_storage);
}

util::TriBool Literal::str_ends_with(std::string_view const needle) const noexcept {
    if (!this->is_string_like()) {
        return util::TriBool::Err;
    }

    auto const s = this->lexical_form();
    return s.ends_with(needle);
}

Literal Literal::str_ends_with(Literal const &needle, Node::NodeStorage &node_storage) const noexcept {
    if (!needle.is_string_like()) {
        return Literal{};
    }

    if (needle.datatype_id() == datatypes::rdf::LangString::datatype_id && this->language_tag() != needle.language_tag()) {
        return Literal{};
    }

    return Literal::make_boolean(this->str_ends_with(needle.lexical_form()), node_storage);
}

Literal Literal::uppercase(Node::NodeStorage &node_storage) const noexcept {
    if (!this->is_string_like()) {
        return Literal{};
    }

    auto const s = this->lexical_form();
    std::string upper;
    upper.reserve(s.size());
    std::transform(s.begin(), s.end(), std::back_inserter(upper), [](char const ch) { return std::toupper(ch); });

    return Literal::make_string_like_copy_lang_tag(upper, *this, node_storage);
}

Literal Literal::lowercase(Node::NodeStorage &node_storage) const noexcept {
    if (!this->is_string_like()) {
        return Literal{};
    }

    auto const s = this->lexical_form();
    std::string lower;
    lower.reserve(s.size());
    std::transform(s.begin(), s.end(), std::back_inserter(lower), [](char const ch) { return std::tolower(ch); });

    return Literal::make_string_like_copy_lang_tag(lower, *this, node_storage);
}

Literal Literal::concat(Literal const &other, Node::NodeStorage &node_storage) const noexcept {
    if (this->null() || other.null()) {
        return Literal{};
    }

    auto const this_dt = this->datatype_id();
    auto const other_dt = other.datatype_id();

    std::ostringstream combined;
    combined << this->lexical_form() << other.lexical_form();

    if (this_dt == datatypes::rdf::LangString::datatype_id && other_dt == datatypes::rdf::LangString::datatype_id) {
        if (auto const lang = this->language_tag(); lang == other.language_tag())  {
            return Literal::make_lang_tagged_unchecked(combined.view(), lang, node_storage);
        }
    }

    return Literal::make_simple_unchecked(combined.view(), node_storage);
}

Literal Literal::substr(size_t start, size_t len, Node::NodeStorage &node_storage) const noexcept {
    if (!this->is_string_like()) {
        return Literal{};
    }

    auto const s = this->lexical_form();

    if (start > s.size()) {
        return Literal::make_string_like_copy_lang_tag("", *this, node_storage);
    }

    auto const substr = s.substr(start, len);
    return Literal::make_string_like_copy_lang_tag(substr, *this, node_storage);
}

Literal Literal::substr(Literal const &start, Literal const &len, Node::NodeStorage &node_storage) const noexcept {
    using namespace datatypes;

    if (!this->is_string_like()) {
        return Literal{};
    }

    auto const start_double = start.cast<xsd::Double>(node_storage);
    if (start_double.null()) {
        return Literal{};
    }

    auto const len_double = len.cast<xsd::Double>(node_storage);
    if (len_double.null()) {
        return Literal{};
    }

    auto const start_val = start_double.value<xsd::Double>();
    if (std::isnan(start_val) || (std::isinf(start_val) && start_val > 0)) [[unlikely]] {
        return Literal::make_string_like_copy_lang_tag("", *this, node_storage);
    }

    auto const len_val = len_double.value<xsd::Double>();
    if (std::isnan(len_val) || len_val <= 0) {
        return Literal::make_string_like_copy_lang_tag("", *this, node_storage);
    }

    auto const start_ix = static_cast<size_t>(std::round(std::max(0.0, start_val - 1.0)));

    if (std::isinf(len_val)) {
        return this->substr(start_ix);
    }

    auto const len_ix = static_cast<size_t>(std::round(len_val) - (start_val < 1.0 ? std::round(1.0 - start_val) : 0.0));

    return this->substr(start_ix, len_ix, node_storage);
}

inline namespace literals {

Literal operator""_xsd_string(char const *str, size_t const len) {
    return Literal{std::string_view{str, len}};
}

Literal operator""_xsd_integer(unsigned long long int i) {
    return Literal::make<datatypes::xsd::Integer>(i);
}

Literal operator""_xsd_int(unsigned long long int i) {
    return Literal::make<datatypes::xsd::Int>(i);
}

Literal operator""_xsd_long(unsigned long long int i) {
    return Literal::make<datatypes::xsd::Integer>(i);
}

Literal operator""_xsd_double(long double d) {
    return Literal::make<datatypes::xsd::Double>(static_cast<datatypes::xsd::Double::cpp_type>(d));
}

Literal operator""_xsd_float(long double d) {
    return Literal::make<datatypes::xsd::Float>(static_cast<datatypes::xsd::Float::cpp_type>(d));
}

Literal operator""_xsd_decimal(char const *str, size_t const len) {
    return Literal::make(std::string_view{str, len}, IRI{datatypes::xsd::Decimal::identifier});
}

}  // namespace literals
}  // namespace rdf4cpp::rdf
