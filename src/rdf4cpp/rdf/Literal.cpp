#include "Literal.hpp"

#include <sstream>

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

Literal Literal::to_node_storage(Node::NodeStorage &node_storage) const noexcept {
    if (this->backend_handle().node_storage_id() == node_storage.id()) {
        return *this;
    }

    auto literal_view = NodeStorage::find_literal_backend_view(backend_handle());
    auto const dtype_iri_view = NodeStorage::find_iri_backend_view(NodeBackendHandle{literal_view.datatype_id, storage::node::identifier::RDFNodeType::IRI, this->backend_handle().node_storage_id()});

    literal_view.datatype_id = node_storage.find_or_make_id(dtype_iri_view);

    auto const new_lit_id = node_storage.find_or_make_id(literal_view);
    return Literal{NodeBackendHandle{new_lit_id, storage::node::identifier::RDFNodeType::Literal, node_storage.id()}};
}

IRI Literal::datatype() const noexcept {
    if (this->is_fixed()) {
        return IRI{NodeBackendHandle::datatype_iri_handle_for_fixed_lit_handle(handle_)};
    }

    return IRI{NodeBackendHandle{handle_.literal_backend().datatype_id,
                                 storage::node::identifier::RDFNodeType::IRI,
                                 handle_.node_storage_id()}};
}

util::CowString Literal::lexical_form() const noexcept {
    if (this->is_inlined()) {
        auto const *entry = datatypes::registry::DatatypeRegistry::get_entry(this->datatype_id());
        assert(entry != nullptr);
        assert(entry->inlining_ops.has_value());

        auto const inlined_value = this->handle_.node_id().literal_id().value;
        return util::CowString{util::ownership_tag::owned, entry->to_canonical_string_fptr(entry->inlining_ops->from_inlined_fptr(inlined_value))};
    }

    return util::CowString{util::ownership_tag::borrowed, handle_.literal_backend().lexical_form};
}

Literal Literal::as_lexical_form(NodeStorage &node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    return Literal::make_simple_unchecked(this->lexical_form(), node_storage);
}

util::CowString Literal::simplified_lexical_form() const noexcept {
    auto const *entry = datatypes::registry::DatatypeRegistry::get_entry(this->datatype_id());
    if (entry == nullptr) {
        return util::CowString{util::ownership_tag::borrowed, this->handle_.literal_backend().lexical_form};
    }

    if (this->is_inlined()) {
        assert(entry->inlining_ops.has_value());

        auto const inlined_value = this->handle_.node_id().literal_id().value;
        return util::CowString{util::ownership_tag::owned, entry->to_simplified_string_fptr(entry->inlining_ops->from_inlined_fptr(inlined_value))};
    }

    auto const value = entry->factory_fptr(this->handle_.literal_backend().lexical_form);
    return util::CowString{util::ownership_tag::owned, entry->to_simplified_string_fptr(value)};
}

Literal Literal::as_simplified_lexical_form(NodeStorage &node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    return Literal::make_simple_unchecked(this->simplified_lexical_form(), node_storage);
}

std::string_view Literal::language_tag() const noexcept {
    if (this->datatype_id() == datatypes::rdf::LangString::datatype_id) {
        return handle_.literal_backend().language_tag;
    }

    return "";
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

    std::ostringstream oss;

    if (this->is_inlined()) {
        quoted_lexical_into_stream(oss, this->lexical_form());

        // rdf:langString is not inlined, therefore can only have datatype not lang tag
        auto const &dtype_iri = NodeStorage::find_iri_backend_view(NodeBackendHandle::datatype_iri_handle_for_fixed_lit_handle(handle_));

        oss << "^^" << dtype_iri.n_string();
    } else {
        auto const &literal = handle_.literal_backend();
        quoted_lexical_into_stream(oss, literal.lexical_form);

        if (this->datatype_id() == datatypes::rdf::LangString::datatype_id) {
            if (!literal.language_tag.empty()) {
                oss << "@" << literal.language_tag;
            }
        } else {
            auto const &dtype_iri = NodeStorage::find_iri_backend_view(NodeBackendHandle{literal.datatype_id,
                                                                                         storage::node::identifier::RDFNodeType::IRI,
                                                                                         handle_.node_storage_id()});
            oss << "^^" << dtype_iri.n_string();
        }
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
std::any Literal::value() const noexcept {
    using namespace datatypes;

    auto const datatype = this->datatype_id();

    if (this->is_inlined()) {
        auto const ops = registry::DatatypeRegistry::get_inlining_ops(datatype);
        assert(ops != nullptr);

        auto const inlined_value = this->handle_.node_id().literal_id().value;
        return ops->from_inlined_fptr(inlined_value);
    }

    if (datatype == rdf::LangString::datatype_id) {
        auto const &lit = this->handle_.literal_backend();

        return registry::LangStringRepr{
            .lexical_form = lit.lexical_form,
            .language_tag = lit.language_tag};
    }

    if (auto const factory = registry::DatatypeRegistry::get_factory(datatype); factory != nullptr) {
        return factory(this->lexical_form());
    }

    return {};
}

Literal Literal::make_simple_unchecked(std::string_view lexical_form, Node::NodeStorage &node_storage) noexcept {
    return Literal{NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::LiteralBackendView{
                                             .datatype_id = storage::node::identifier::NodeID::xsd_string_iri.first,
                                             .lexical_form = lexical_form,
                                             .language_tag = ""}),
                                     storage::node::identifier::RDFNodeType::Literal,
                                     node_storage.id()}};
}

Literal Literal::make_noninlined_typed_unchecked(std::string_view lexical_form, IRI const &datatype, Node::NodeStorage &node_storage) noexcept {
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

Literal Literal::make_inlined_typed_unchecked(uint64_t inlined_value, storage::node::identifier::LiteralType fixed_id, Node::NodeStorage &node_storage) noexcept {
    using namespace storage::node::identifier;

    assert(inlined_value >> LiteralID::width == 0);
    assert(fixed_id != LiteralType::other());

    return Literal{NodeBackendHandle{NodeID{LiteralID{inlined_value}, fixed_id},
                                     RDFNodeType::Literal,
                                     node_storage.id(),
                                     true}};
}

Literal Literal::make_typed_unchecked(std::any const &value, datatypes::registry::DatatypeIDView datatype, datatypes::registry::DatatypeRegistry::DatatypeEntry const &entry, Node::NodeStorage &node_storage) noexcept {
    if (entry.inlining_ops.has_value()) {
        if (auto const maybe_inlined = entry.inlining_ops->try_into_inlined_fptr(value); maybe_inlined.has_value()) {
            return Literal::make_inlined_typed_unchecked(*maybe_inlined, datatype.get_fixed(), node_storage);
        }
    }

    return Literal::make_noninlined_typed_unchecked(entry.to_canonical_string_fptr(value),
                                                    IRI{datatype, node_storage},
                                                    node_storage);
}

Literal Literal::make(std::string_view lexical_form, IRI const &datatype, Node::NodeStorage &node_storage) {
    using namespace datatypes::registry;

    DatatypeIDView const datatype_identifier{datatype};

    if (datatype_identifier == datatypes::rdf::LangString::datatype_id) {
        // see: https://www.w3.org/TR/rdf11-concepts/#section-Graph-Literal
        throw std::invalid_argument{"cannot construct rdf:langString without a language tag, please call one of the other constructors"};
    }

    if (auto const *entry = DatatypeRegistry::get_entry(datatype_identifier); entry != nullptr) {
        // exists => canonize

        auto const cpp_value = entry->factory_fptr(lexical_form);
        return Literal::make_typed_unchecked(cpp_value, datatype_identifier, *entry, node_storage);
    } else {
        // doesn't exist in the registry no way to canonicalize
        return Literal::make_noninlined_typed_unchecked(lexical_form, datatype, node_storage);
    }
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

    if (this_dtid == String::datatype_id) {
        // string -> any
        try {
            return Literal::make(this->lexical_form(), target, node_storage);
        } catch (std::runtime_error const &) {
            return Literal{};
        }
    }

    if (target_dtid == String::datatype_id) {
        // any -> string
        return this->as_simplified_lexical_form(node_storage);
    }

    if (target_dtid == Boolean::datatype_id) {
        // any -> bool
        return this->ebv_as_literal(node_storage);
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

            return Literal::make_typed_unchecked(value, target_dtid, *target_e, node_storage);
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

            return Literal::make_typed_unchecked(*target_value, target_dtid, *target_e, node_storage);
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
        return Literal::make_typed_unchecked(*target_value, target_dtid, *target_e, node_storage);
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
    auto const *this_entry = DatatypeRegistry::get_entry(this_datatype);
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

        auto const *result_entry = [&]() {
            if (op_res.result_type_id == this_datatype) [[likely]] {
                return this_entry;
            } else [[unlikely]] {
                return DatatypeRegistry::get_entry(op_res.result_type_id);
            }
        }();

        assert(result_entry != nullptr);
        return Literal::make_typed_unchecked(*op_res.result_value, op_res.result_type_id, *result_entry, node_storage);
    } else {
        auto const *other_entry = DatatypeRegistry::get_entry(other_datatype);
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

        auto const *result_entry = [&, equalized_id = std::ref(equalized_id), equalized_entry = equalized_entry]() {
            if (op_res.result_type_id == equalized_id.get()) [[likely]] {
                return equalized_entry;
            } else [[unlikely]] {
                return DatatypeRegistry::get_entry(op_res.result_type_id);
            }
        }();

        assert(result_entry != nullptr);
        return Literal::make_typed_unchecked(*op_res.result_value, op_res.result_type_id, *result_entry, node_storage);
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

    auto const *result_entry = [&op_res, operand_entry = operand_entry]() {
        if (op_res.result_type_id == DatatypeIDView{operand_entry->datatype_iri}) [[likely]] {
            return operand_entry;
        } else [[unlikely]] {
            return DatatypeRegistry::get_entry(op_res.result_type_id);
        }
    }();

    assert(result_entry != nullptr);
    return Literal::make_typed_unchecked(*op_res.result_value, op_res.result_type_id, *result_entry, node_storage);
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

bool Literal::is_fixed() const noexcept {
    assert(!this->null());
    auto const lit_type = this->handle_.node_id().literal_type();
    return lit_type.is_fixed();
}

bool Literal::is_fixed_not_numeric() const noexcept {
    using namespace datatypes::registry;

    assert(!this->null());
    auto const lit_type = this->handle_.node_id().literal_type();
    return lit_type.is_fixed() && !lit_type.is_numeric();
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



}  // namespace rdf4cpp::rdf
