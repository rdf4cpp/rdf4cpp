#include "Literal.hpp"

#include <sstream>
#include <random>

#include <rdf4cpp/rdf/IRI.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/LiteralBackend.hpp>
#include <rdf4cpp/rdf/util/Utf8.hpp>
#include <rdf4cpp/rdf/util/CaseInsensitiveCharTraits.hpp>

namespace rdf4cpp::rdf {

Literal::Literal(Node::NodeBackendHandle handle) noexcept
    : Node{handle} {}

Literal::Literal() noexcept
    : Node{NodeBackendHandle{{}, storage::node::identifier::RDFNodeType::Literal, {}}} {}

Literal Literal::make_null() noexcept {
    return Literal{};
}

Literal Literal::make_simple_unchecked(std::string_view lexical_form, Node::NodeStorage &node_storage) noexcept {
    return Literal{NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::LiteralBackendView{
                                             .literal = storage::node::view::LexicalFormBackendView{
                                                     .datatype_id = storage::node::identifier::NodeID::xsd_string_iri.first,
                                                     .lexical_form = lexical_form,
                                                     .language_tag = ""}}),
                                     storage::node::identifier::RDFNodeType::Literal,
                                     node_storage.id()}};
}

Literal Literal::make_noninlined_typed_unchecked(std::string_view lexical_form, IRI const &datatype, Node::NodeStorage &node_storage) noexcept {
    return Literal{NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::LiteralBackendView{
                                             .literal = storage::node::view::LexicalFormBackendView{
                                                     .datatype_id = datatype.to_node_storage(node_storage).backend_handle().node_id(),
                                                     .lexical_form = lexical_form,
                                                     .language_tag = ""}}),
                                     storage::node::identifier::RDFNodeType::Literal,
                                     node_storage.id()}};
}

Literal Literal::make_lang_tagged_unchecked(std::string_view lexical_form, std::string_view lang, Node::NodeStorage &node_storage) noexcept {
    return Literal{NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::LiteralBackendView{
                                             .literal = storage::node::view::LexicalFormBackendView{
                                                     .datatype_id = storage::node::identifier::NodeID::rdf_langstring_iri.first,
                                                     .lexical_form = lexical_form,
                                                     .language_tag = lang}}),
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

Literal Literal::make_string_like_copy_lang_tag(std::string_view str, Literal const &lang_tag_src, Node::NodeStorage &node_storage) noexcept {
    if (lang_tag_src.datatype_eq<datatypes::rdf::LangString>()) {
        return Literal::make_lang_tagged_unchecked(str, lang_tag_src.language_tag(), node_storage);
    }

    assert(lang_tag_src.datatype_eq<datatypes::xsd::String>());
    return Literal::make_simple_unchecked(str, node_storage);
}

bool Literal::dynamic_datatype_eq_impl(std::string_view datatype) const noexcept {
    assert(!this->is_fixed());
    return this->datatype().identifier() == datatype;
}

Literal Literal::make_simple(std::string_view lexical_form, Node::NodeStorage &node_storage) {
    return Literal::make_simple_unchecked(lexical_form, node_storage);
}

Literal Literal::make_lang_tagged(std::string_view lexical_form, std::string_view lang_tag, Node::NodeStorage &node_storage) {
    std::string lowercase_lang_tag;
    std::transform(lang_tag.begin(), lang_tag.end(), std::back_inserter(lowercase_lang_tag), [](char const ch) {
        return static_cast<char>(std::tolower(ch));
    });

    return Literal::make_lang_tagged_unchecked(lexical_form, lowercase_lang_tag, node_storage);
}

Literal Literal::make_typed(std::string_view lexical_form, IRI const &datatype, Node::NodeStorage &node_storage) {
    using namespace datatypes::registry;

    DatatypeIDView const datatype_identifier{datatype};

    if (datatype_identifier == datatypes::rdf::LangString::datatype_id) {
        // see: https://www.w3.org/TR/rdf11-concepts/#section-Graph-Literal
        throw std::invalid_argument{"cannot construct rdf:langString without a language tag, please call one of the other factory functions"};
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

Literal Literal::make_boolean(util::TriBool const b, Node::NodeStorage &node_storage) {
    if (b == util::TriBool::Err) {
        return Literal{};
    }

    return Literal::make_typed_from_value<datatypes::xsd::Boolean>(b == util::TriBool::True, node_storage);
}

Literal Literal::generate_random_double(Node::NodeStorage &node_storage) {
    struct RngState {
        std::default_random_engine rng{std::random_device{}()};
        std::uniform_real_distribution<datatypes::xsd::Double::cpp_type> dist{0.0, 1.0};
    };

    static thread_local RngState state;
    return Literal::make_typed_from_value<datatypes::xsd::Double>(state.dist(state.rng), node_storage);
}

Literal Literal::to_node_storage(Node::NodeStorage &node_storage) const noexcept {
    if (handle_.node_storage_id() == node_storage.id()) {
        return *this;
    }

    auto literal_view = NodeStorage::find_literal_backend_view(backend_handle());
    auto const node_id = std::visit(storage::util::Overloaded{
                                            [&](storage::node::view::LexicalFormBackendView &lexical) {
                                                // exchange the datatype in literal_view for one managed by the new node_storage (the IRI of the datatype must live within the same NodeStorage as the Literal it is used for)
                                                auto dtype_iri_view = NodeStorage::find_iri_backend_view(NodeBackendHandle{lexical.datatype_id, storage::node::identifier::RDFNodeType::IRI, backend_handle().node_storage_id()});
                                                lexical.datatype_id = node_storage.find_or_make_id(dtype_iri_view);
                                                // find or make the requested node
                                                return node_storage.find_or_make_id(literal_view);
                                            },
                                            [&](storage::node::view::AnyBackendView const &) {
                                                return node_storage.find_or_make_id(literal_view);
                                            }},
                                    literal_view.literal);

    return Literal{NodeBackendHandle{node_id, storage::node::identifier::RDFNodeType::Literal, node_storage.id()}};
}

bool Literal::datatype_eq(IRI const &datatype) const noexcept {
    return this->datatype_id() == datatypes::registry::DatatypeIDView{datatype};
}

bool Literal::datatype_eq(Literal const &other) const noexcept {
    if (auto const this_type = this->handle_.node_id().literal_type(); this_type.is_fixed()) {
        if (auto const other_type = other.handle_.node_id().literal_type(); other_type.is_fixed()) {
            return this_type == other_type;
        }

        return false;
    }

    return this->datatype() == other.datatype();
}

Literal Literal::as_datatype_eq(IRI const &datatype, NodeStorage &node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    return Literal::make_boolean(this->datatype_eq(datatype), node_storage);
}

Literal Literal::as_datatype_eq(Literal const &other, NodeStorage &node_storage) const noexcept {
    if (this->null() || other.null()) {
        return Literal{};
    }

    return Literal::make_boolean(this->datatype_eq(other), node_storage);
}

IRI Literal::datatype() const noexcept {
    if (this->is_fixed()) {
        return IRI{NodeBackendHandle::datatype_iri_handle_for_fixed_lit_handle(handle_)};
    }

    auto const lit_backend = handle_.literal_backend();
    return std::visit(storage::util::Overloaded{
                              [&](storage::node::view::LexicalFormBackendView const &lexical) {
                                  return IRI{NodeBackendHandle{lexical.datatype_id,
                                                               storage::node::identifier::RDFNodeType::IRI,
                                                               handle_.node_storage_id()}};
                              },
                              [&](storage::node::view::AnyBackendView const &any) {
                                  assert(false);
                                  return IRI{NodeBackendHandle{static_cast<NodeID>(any.datatype.to_underlying()),
                                                               storage::node::identifier::RDFNodeType::IRI,
                                                               handle_.node_storage_id()}};
                              }},
                      lit_backend.literal);
}

util::CowString Literal::lexical_form() const noexcept {
    if (this->is_inlined()) {
        auto const *entry = datatypes::registry::DatatypeRegistry::get_entry(this->datatype_id());
        assert(entry != nullptr);
        assert(entry->inlining_ops.has_value());

        auto const inlined_value = this->handle_.node_id().literal_id().value;
        return util::CowString{util::ownership_tag::owned, entry->to_canonical_string_fptr(entry->inlining_ops->from_inlined_fptr(inlined_value))};
    }

    auto const lit_backend = handle_.literal_backend();
    return std::visit(storage::util::Overloaded{
                              [&](storage::node::view::LexicalFormBackendView const &lexical) {
                                  return util::CowString{util::ownership_tag::borrowed, lexical.lexical_form};
                              },
                              [&](storage::node::view::AnyBackendView const &any) {
                                  auto const to_string = datatypes::registry::DatatypeRegistry::get_to_canonical_string(this->datatype_id());
                                  assert(to_string != nullptr);
                                  assert(false); // TODO
                                  // to_string(any.value);
                                  return util::CowString{util::ownership_tag::owned, ""};
                              }},
                      lit_backend.literal);
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
        return util::CowString{util::ownership_tag::borrowed, std::get<storage::node::view::LexicalFormBackendView>(this->handle_.literal_backend().literal).lexical_form};
    }

    if (this->is_inlined()) {
        assert(entry->inlining_ops.has_value());

        auto const inlined_value = this->handle_.node_id().literal_id().value;
        return util::CowString{util::ownership_tag::owned, entry->to_simplified_string_fptr(entry->inlining_ops->from_inlined_fptr(inlined_value))};
    }

    return std::visit(storage::util::Overloaded{
                              [&](storage::node::view::LexicalFormBackendView const &lexical) {
                                  auto const value = entry->factory_fptr(lexical.lexical_form);
                                  return util::CowString{util::ownership_tag::owned, entry->to_simplified_string_fptr(value)};
                              },
                              [&](storage::node::view::AnyBackendView const &any) {
                                  assert(false);
                                  //return util::CowString{util::ownership_tag::owned, entry->to_simplified_string_fptr(any.value)}; TODO
                                  return util::CowString{util::ownership_tag::owned, ""};
                              }},
                      this->handle_.literal_backend().literal);
}

Literal Literal::as_simplified_lexical_form(NodeStorage &node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    return Literal::make_simple_unchecked(this->simplified_lexical_form(), node_storage);
}

std::string_view Literal::language_tag() const noexcept {
    if (this->datatype_eq<datatypes::rdf::LangString>()) {
        return std::get<storage::node::view::LexicalFormBackendView>(handle_.literal_backend().literal).language_tag;
    }

    return "";
}

Literal Literal::as_language_tag(Node::NodeStorage &node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    return Literal::make_simple_unchecked(this->language_tag(), node_storage);
}

util::TriBool Literal::language_tag_eq(std::string_view const lang_tag) const noexcept {
    if (!this->datatype_eq<datatypes::rdf::LangString>()) {
        return util::TriBool::Err;
    }

    return this->language_tag() == lang_tag;
}

util::TriBool Literal::language_tag_eq(Literal const &other) const noexcept {
    if (!this->datatype_eq<datatypes::rdf::LangString>() || !other.datatype_eq<datatypes::rdf::LangString>()) {
        return util::TriBool::Err;
    }

    return this->language_tag() == other.language_tag();
}

Literal Literal::as_language_tag_eq(std::string_view const lang_tag, Node::NodeStorage &node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    return Literal::make_boolean(this->language_tag_eq(lang_tag), node_storage);
}

Literal Literal::as_language_tag_eq(Literal const &other, Node::NodeStorage &node_storage) const noexcept {
    if (this->null() || other.null()) {
        return Literal{};
    }

    return Literal::make_boolean(this->language_tag_eq(other), node_storage);
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
    } else if (this->datatype_eq<datatypes::rdf::LangString>()) {
        auto const value = this->value<datatypes::rdf::LangString>();

        quoted_lexical_into_stream(oss, value.lexical_form);
        oss << '@' << value.language_tag;
    } else {
        auto const &literal = handle_.literal_backend();

        std::visit(storage::util::Overloaded{
                           [&](storage::node::view::LexicalFormBackendView const &lexical) {
                               auto const &dtype_iri = NodeStorage::find_iri_backend_view(NodeBackendHandle{lexical.datatype_id,
                                                                                                            storage::node::identifier::RDFNodeType::IRI,
                                                                                                            handle_.node_storage_id()});
                               quoted_lexical_into_stream(oss, lexical.lexical_form);
                               oss << "^^" << dtype_iri.n_string();
                           },
                           [&](storage::node::view::AnyBackendView const &any) {
                               auto const to_string = datatypes::registry::DatatypeRegistry::get_to_canonical_string(this->datatype_id());
                               assert(to_string != nullptr);

                               auto const &dtype_iri = NodeStorage::find_iri_backend_view(NodeBackendHandle::datatype_iri_handle_for_fixed_lit_handle(handle_));

                               //oss << to_string(any.value); TODO
                               oss << "^^" << dtype_iri.n_string();
                           }},
                   literal.literal);
    }

    return oss.str();
}
bool Literal::is_literal() const noexcept { return true; }
bool Literal::is_variable() const noexcept { return false; }
bool Literal::is_blank_node() const noexcept { return false; }
bool Literal::is_iri() const noexcept { return false; }
bool Literal::is_numeric() const noexcept {
    using namespace datatypes::registry;

    if (this->null()) {
        return false;
    }

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
        auto const &lex = std::get<storage::node::view::LexicalFormBackendView>(lit.literal);

        return registry::LangStringRepr{
            .lexical_form = lex.lexical_form,
            .language_tag = lex.language_tag};
    }

    if (auto const factory = registry::DatatypeRegistry::get_factory(datatype); factory != nullptr) {
        return factory(this->lexical_form());
    }

    return {};
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
        return this->to_node_storage(node_storage);
    }

    if (this_dtid == String::datatype_id) {
        // string -> any
        try {
            return Literal::make_typed(this->lexical_form(), target, node_storage);
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
        return this->as_ebv(node_storage);
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

    if (this->handle_ == other.handle_) {
        return std::partial_ordering::equivalent;
    }

    if (this->handle_.null() || other.handle_.null()) {
        if (out_alternative_ordering != nullptr) {
            // ordering extensions (for e.g. ORDER BY) require that null nodes
            // are always the smallest node
            *out_alternative_ordering = this->handle_.null()
                                                ? std::strong_ordering::less
                                                : std::strong_ordering::greater;
        }
        return std::partial_ordering::unordered;
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

Literal Literal::as_eq(Literal const &other, NodeStorage &node_storage) const noexcept {
    return Literal::make_boolean(this->eq(other), node_storage);
}

util::TriBool Literal::operator==(Literal const &other) const noexcept {
    return this->eq(other);
}

util::TriBool Literal::ne(Literal const &other) const noexcept {
    return !util::partial_weak_ordering_eq(this->compare(other), std::weak_ordering::equivalent);
}

Literal Literal::as_ne(Literal const &other, NodeStorage &node_storage) const noexcept {
    return Literal::make_boolean(this->ne(other), node_storage);
}

util::TriBool Literal::operator!=(Literal const &other) const noexcept {
    return this->ne(other);
}

util::TriBool Literal::lt(Literal const &other) const noexcept {
    return util::partial_weak_ordering_eq(this->compare(other), std::weak_ordering::less);
}

Literal Literal::as_lt(Literal const &other, NodeStorage &node_storage) const noexcept {
    return Literal::make_boolean(this->lt(other), node_storage);
}

util::TriBool Literal::operator<(Literal const &other) const noexcept {
    return this->lt(other);
}

util::TriBool Literal::le(Literal const &other) const noexcept {
    return !util::partial_weak_ordering_eq(this->compare(other), std::weak_ordering::greater);
}

Literal Literal::as_le(Literal const &other, NodeStorage &node_storage) const noexcept {
    return Literal::make_boolean(this->le(other), node_storage);
}

util::TriBool Literal::operator<=(Literal const &other) const noexcept {
    return this->le(other);
}

util::TriBool Literal::gt(Literal const &other) const noexcept {
    return util::partial_weak_ordering_eq(this->compare(other), std::weak_ordering::greater);
}

Literal Literal::as_gt(Literal const &other, NodeStorage &node_storage) const noexcept {
    return Literal::make_boolean(this->gt(other), node_storage);
}

util::TriBool Literal::operator>(Literal const &other) const noexcept {
    return this->gt(other);
}

util::TriBool Literal::ge(Literal const &other) const noexcept {
    return !util::partial_weak_ordering_eq(this->compare(other), std::weak_ordering::less);
}

Literal Literal::as_ge(Literal const &other, NodeStorage &node_storage) const noexcept {
    return Literal::make_boolean(this->ge(other), node_storage);
}

util::TriBool Literal::operator>=(Literal const &other) const noexcept {
    return this->ge(other);
}

bool Literal::eq_with_extensions(Literal const &other) const noexcept {
    return this->compare_with_extensions(other) == std::weak_ordering::equivalent;
}

Literal Literal::as_eq_with_extensions(Literal const &other, NodeStorage &node_storage) const noexcept {
    return Literal::make_boolean(this->eq_with_extensions(other), node_storage);
}

bool Literal::ne_with_extensions(Literal const &other) const noexcept {
    return this->compare_with_extensions(other) != std::weak_ordering::equivalent;
}

Literal Literal::as_ne_with_extensions(Literal const &other, NodeStorage &node_storage) const noexcept {
    return Literal::make_boolean(this->ne_with_extensions(other), node_storage);
}

bool Literal::lt_with_extensions(Literal const &other) const noexcept {
    return this->compare_with_extensions(other) == std::weak_ordering::less;
}

Literal Literal::as_lt_with_extensions(Literal const &other, NodeStorage &node_storage) const noexcept {
    return Literal::make_boolean(this->lt_with_extensions(other), node_storage);
}

bool Literal::le_with_extensions(Literal const &other) const noexcept {
    return this->compare_with_extensions(other) != std::weak_ordering::greater;
}

Literal Literal::as_le_with_extensions(Literal const &other, NodeStorage &node_storage) const noexcept {
    return Literal::make_boolean(this->le_with_extensions(other), node_storage);
}

bool Literal::gt_with_extensions(Literal const &other) const noexcept {
    return this->compare_with_extensions(other) == std::weak_ordering::greater;
}

Literal Literal::as_gt_with_extensions(Literal const &other, NodeStorage &node_storage) const noexcept {
    return Literal::make_boolean(this->gt_with_extensions(other), node_storage);
}

bool Literal::ge_with_extensions(Literal const &other) const noexcept {
    return this->compare_with_extensions(other) != std::weak_ordering::less;
}

Literal Literal::as_ge_with_extensions(Literal const &other, NodeStorage &node_storage) const noexcept {
    return Literal::make_boolean(this->ge_with_extensions(other), node_storage);
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

bool Literal::is_string_like() const noexcept {
    auto const type = this->handle_.node_id().literal_type();
    if (!type.is_fixed()) {
        return false;
    }

    return type == datatypes::xsd::String::fixed_id || type == datatypes::rdf::LangString::fixed_id;
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

Literal Literal::as_ebv(NodeStorage &node_storage) const noexcept {
    return Literal::make_boolean(this->ebv(), node_storage);
}

Literal Literal::logical_and(Literal const &other, Node::NodeStorage &node_storage) const noexcept {
    return Literal::make_boolean(this->ebv() && other.ebv(), node_storage);;
}

Literal Literal::operator&&(Literal const &other) const noexcept {
    return this->logical_and(other);
}

Literal Literal::logical_or(Literal const &other, Node::NodeStorage &node_storage) const noexcept {
    return Literal::make_boolean(this->ebv() || other.ebv(), node_storage);
}

Literal Literal::operator||(Literal const &other) const noexcept {
    return this->logical_or(other);
}

Literal Literal::logical_not(Node::NodeStorage &node_storage) const noexcept {
    return Literal::make_boolean(!this->ebv(), node_storage);
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
    if (this->null()) {
        return Literal{};
    }

    auto const len = this->strlen();
    if (!len.has_value()) {
        return Literal{};
    }

    return Literal::make_typed_from_value<datatypes::xsd::Integer>(datatypes::xsd::Integer::cpp_type{*len}, node_storage);
}

util::TriBool Literal::language_tag_matches_range(std::string_view const lang_range) const noexcept {
    if (!this->is_string_like()) {
        return util::TriBool::Err;
    }

    return lang_matches(this->language_tag(), lang_range);
}

Literal Literal::as_language_tag_matches_range(std::string_view lang_range, Node::NodeStorage &node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    auto const res = this->language_tag_matches_range(lang_range);
    return Literal::make_boolean(res, node_storage);
}

Literal Literal::as_language_tag_matches_range(Literal const &lang_range, Node::NodeStorage &node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    if (!lang_range.datatype_eq<datatypes::xsd::String>()) {
        return Literal{};
    }

    auto const res = this->language_tag_matches_range(lang_range.lexical_form());
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

util::TriBool Literal::regex_matches(regex::Regex const &pattern) const noexcept {
    if (!this->is_string_like()) {
        return util::TriBool::Err;
    }

    return pattern.regex_search(this->lexical_form());
}

Literal Literal::as_regex_matches(regex::Regex const &pattern, Node::NodeStorage &node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    auto const res = this->regex_matches(pattern);
    return Literal::make_boolean(res, node_storage);
}

Literal Literal::as_regex_matches(Literal const &pattern, Literal const &flags, Node::NodeStorage &node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    if (!this->is_string_like() || !pattern.is_string_like() || !flags.is_string_like()) {
        return Literal{};
    }

    if (pattern.datatype_eq<datatypes::rdf::LangString>() && this->language_tag() != pattern.language_tag()) {
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

    auto const res = this->regex_matches(*re);
    return Literal::make_boolean(res, node_storage);
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

    if (pattern.datatype_eq<datatypes::rdf::LangString>() && this->language_tag() != pattern.language_tag()) {
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
    return s.view().find(needle) != std::string_view::npos;
}

Literal Literal::as_contains(std::string_view const needle, Node::NodeStorage &node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    auto const res = this->contains(needle);
    return Literal::make_boolean(res, node_storage);
}

Literal Literal::as_contains(Literal const &needle, Node::NodeStorage &node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    if (needle.datatype_eq<datatypes::rdf::LangString>() && this->language_tag() != needle.language_tag()) {
        return Literal{};
    }

    auto const res = this->contains(needle.lexical_form());
    return Literal::make_boolean(res, node_storage);
}

Literal Literal::substr_before(std::string_view const needle, Node::NodeStorage &node_storage) const noexcept {
    if (!this->is_string_like()) {
        return Literal{};
    }

    if (needle.empty()) {
        return Literal::make_string_like_copy_lang_tag("", *this, node_storage);
    }

    auto const s = this->lexical_form();
    auto const pos = s.view().find(needle);

    if (pos == std::string_view::npos) {
        return Literal::make_simple_unchecked("", node_storage);
    }

    auto const substr = s.view().substr(0, pos);
    return Literal::make_string_like_copy_lang_tag(substr, *this, node_storage);
}

Literal Literal::substr_before(Literal const &needle, Node::NodeStorage &node_storage) const noexcept {
    if (needle.datatype_eq<datatypes::rdf::LangString>() && this->language_tag() != needle.language_tag()) {
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
    auto const pos = s.view().find(needle);

    if (pos == std::string_view::npos) {
        return Literal::make_simple_unchecked("", node_storage);
    }

    auto const substr = s.view().substr(pos + needle.size());
    return Literal::make_string_like_copy_lang_tag(substr, *this, node_storage);
}

Literal Literal::substr_after(Literal const &needle, Node::NodeStorage &node_storage) const noexcept {
    if (needle.datatype_eq<datatypes::rdf::LangString>() && this->language_tag() != needle.language_tag()) {
        return Literal{};
    }

    return this->substr_after(needle.lexical_form(), node_storage);
}

util::TriBool Literal::str_starts_with(std::string_view const needle) const noexcept {
    if (!this->is_string_like()) {
        return util::TriBool::Err;
    }

    auto const s = this->lexical_form();
    return s.view().starts_with(needle);
}

Literal Literal::as_str_starts_with(std::string_view const needle, Node::NodeStorage &node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    auto const res = this->str_starts_with(needle);
    return Literal::make_boolean(res, node_storage);
}

Literal Literal::as_str_starts_with(Literal const &needle, Node::NodeStorage &node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    if (!needle.is_string_like()) {
        return Literal{};
    }

    if (needle.datatype_eq<datatypes::rdf::LangString>() && this->language_tag() != needle.language_tag()) {
        return Literal{};
    }

    auto const res = this->str_starts_with(needle.lexical_form());
    return Literal::make_boolean(res, node_storage);
}

util::TriBool Literal::str_ends_with(std::string_view const needle) const noexcept {
    if (!this->is_string_like()) {
        return util::TriBool::Err;
    }

    auto const s = this->lexical_form();
    return s.view().ends_with(needle);
}

Literal Literal::as_str_ends_with(std::string_view const needle, Node::NodeStorage &node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    auto const res = this->str_ends_with(needle);
    return Literal::make_boolean(res, node_storage);
}

Literal Literal::as_str_ends_with(Literal const &needle, Node::NodeStorage &node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    if (!needle.is_string_like()) {
        return Literal{};
    }

    if (needle.datatype_eq<datatypes::rdf::LangString>() && this->language_tag() != needle.language_tag()) {
        return Literal{};
    }

    auto const res = this->str_ends_with(needle.lexical_form());
    return Literal::make_boolean(res, node_storage);
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

    std::ostringstream combined;
    combined << this->lexical_form() << other.lexical_form();

    if (this->datatype_eq<datatypes::rdf::LangString>() && other.datatype_eq<datatypes::rdf::LangString>()) {
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

    auto const substr = s.view().substr(start, len);
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

bool lang_matches(std::string_view const lang_tag, std::string_view const lang_range) noexcept {
    if (lang_range.empty()) {
        return lang_tag.empty();
    }

    if (lang_range == "*") {
        return !lang_tag.empty();
    }

    auto const lang_ci = util::CiStringView{lang_tag.data(), lang_tag.size()};
    auto const lang_range_ci = util::CiStringView{lang_range.data(), lang_range.size()};

    // case-insensitive comparison
    return lang_ci.starts_with(lang_range_ci) && (lang_ci.size() == lang_range_ci.size() || lang_ci[lang_range_ci.size()] == '-');
}

Literal lang_matches(Literal const &lang_tag, Literal const &lang_range, storage::node::NodeStorage &node_storage) noexcept {
    if (lang_tag.null() || lang_range.null()) {
        return Literal{};
    }

    if (!lang_tag.datatype_eq<datatypes::xsd::String>() || !lang_range.datatype_eq<datatypes::xsd::String>()) {
        return Literal{};
    }

    auto const res = lang_matches(lang_tag.lexical_form(), lang_range.lexical_form());
    return Literal::make_boolean(res, node_storage);
}

inline namespace shorthands {

Literal operator""_xsd_string(char const *str, size_t const len) {
    return Literal::make_simple(std::string_view{str, len});
}

Literal operator""_xsd_double(long double d) {
    return Literal::make_typed_from_value<datatypes::xsd::Double>(static_cast<datatypes::xsd::Double::cpp_type>(d));
}

Literal operator""_xsd_float(long double d) {
    return Literal::make_typed_from_value<datatypes::xsd::Float>(static_cast<datatypes::xsd::Float::cpp_type>(d));
}

Literal operator""_xsd_decimal(char const *str, size_t const len) {
    return Literal::make_typed<datatypes::xsd::Decimal>(std::string_view{str, len});
}

Literal operator""_xsd_integer(unsigned long long int i) {
    return Literal::make_typed_from_value<datatypes::xsd::Integer>(i);
}

Literal operator""_xsd_byte(unsigned long long int i) {
    return Literal::make_typed_from_value<datatypes::xsd::Byte>(static_cast<datatypes::xsd::Byte::cpp_type>(i));
}

Literal operator""_xsd_ubyte(unsigned long long int i) {
    return Literal::make_typed_from_value<datatypes::xsd::UnsignedByte>(static_cast<datatypes::xsd::UnsignedByte::cpp_type>(i));
}

Literal operator""_xsd_short(unsigned long long int i) {
    return Literal::make_typed_from_value<datatypes::xsd::Short>(static_cast<datatypes::xsd::Short::cpp_type>(i));
}

Literal operator""_xsd_ushort(unsigned long long int i) {
    return Literal::make_typed_from_value<datatypes::xsd::UnsignedShort>(static_cast<datatypes::xsd::UnsignedShort::cpp_type>(i));
}

Literal operator""_xsd_int(unsigned long long int i) {
    return Literal::make_typed_from_value<datatypes::xsd::Int>(static_cast<datatypes::xsd::Int::cpp_type>(i));
}

Literal operator""_xsd_uint(unsigned long long int i) {
    return Literal::make_typed_from_value<datatypes::xsd::UnsignedInt>(static_cast<datatypes::xsd::UnsignedInt::cpp_type>(i));
}

Literal operator""_xsd_long(unsigned long long int i) {
    return Literal::make_typed_from_value<datatypes::xsd::Long>(static_cast<datatypes::xsd::Long::cpp_type>(i));
}

Literal operator""_xsd_ulong(unsigned long long int i) {
    return Literal::make_typed_from_value<datatypes::xsd::UnsignedLong>(static_cast<datatypes::xsd::UnsignedLong::cpp_type>(i));
}

}  // namespace shorthands
}  // namespace rdf4cpp::rdf
