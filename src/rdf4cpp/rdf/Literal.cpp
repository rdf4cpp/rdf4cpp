#include "Literal.hpp"

#include <algorithm>
#include <execution>
#include <random>
#include <ranges>
#include <sstream>

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <uni_algo/all.h>

#include <rdf4cpp/rdf/IRI.hpp>
#include <rdf4cpp/rdf/datatypes/registry/util/DateTimeUtils.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/FallbackLiteralBackend.hpp>
#include <rdf4cpp/rdf/util/CaseInsensitiveCharTraits.hpp>

#include <openssl/evp.h>

namespace rdf4cpp::rdf {
static bool lexical_form_needs_escape_non_simd(std::string_view const lexical_form) noexcept {
    // https://www.w3.org/TR/n-triples/#grammar-production-STRING_LITERAL_QUOTE
    auto const it = std::find_if(std::execution::unseq, lexical_form.begin(), lexical_form.end(), [](char const ch) noexcept {
        return ch == '"' || ch == '\\' || ch == '\n' || ch == '\r';
    });

    return it != lexical_form.end();
}
} // namespace rdf4cpp::rdf

#ifdef __AVX2__
#include <immintrin.h>

namespace rdf4cpp::rdf {
bool Literal::lexical_form_needs_escape(std::string_view const lexical_form) noexcept {
    // https://www.w3.org/TR/n-triples/#grammar-production-STRING_LITERAL_QUOTE
    __m256i const masks[4]{_mm256_set1_epi8('"'),
                           _mm256_set1_epi8('\\'),
                           _mm256_set1_epi8('\n'),
                           _mm256_set1_epi8('\r')};

    for (size_t bix = 0; bix < lexical_form.size() / 32; ++bix) {
        __m256i const chars = _mm256_loadu_si256(reinterpret_cast<__m256i const *>(lexical_form.data() + (bix * 32)));
        for (auto const &mask : masks) {
            auto const eq = _mm256_cmpeq_epi8(mask, chars);

            if (_mm256_movemask_epi8(eq) > 0) {
                return true;
            }
        }
    }

    auto const rest = lexical_form.size() % 32;
    return lexical_form_needs_escape_non_simd(lexical_form.substr(lexical_form.size() - rest));
}
} // rdf4cpp::rdf
#else
namespace rdf4cpp::rdf {
bool Literal::lexical_form_needs_escape(std::string_view const lexical_form) noexcept {
    return lexical_form_needs_escape_non_simd(lexical_form);
}
} // namespace rdf4cpp::rdf
#endif

namespace rdf4cpp::rdf {

Literal::Literal(Node::NodeBackendHandle handle) noexcept
    : Node{handle} {}

Literal::Literal() noexcept
    : Node{NodeBackendHandle{{}, storage::node::identifier::RDFNodeType::Literal, {}}} {}

Literal Literal::make_null() noexcept {
    return Literal{};
}

Literal Literal::make_simple_unchecked(std::string_view lexical_form, bool needs_escape, NodeStorage &node_storage) noexcept {
    return Literal{NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::LexicalFormLiteralBackendView{
                                             .datatype_id = storage::node::identifier::NodeID::xsd_string_iri.first,
                                             .lexical_form = lexical_form,
                                             .language_tag = "",
                                             .needs_escape = needs_escape}),
                                     storage::node::identifier::RDFNodeType::Literal,
                                     node_storage.id()}};
}

Literal Literal::make_noninlined_typed_unchecked(std::string_view lexical_form, bool needs_escape, IRI const &datatype, NodeStorage &node_storage) noexcept {
    return Literal{NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::LexicalFormLiteralBackendView{
                                             .datatype_id = datatype.to_node_storage(node_storage).backend_handle().node_id(),
                                             .lexical_form = lexical_form,
                                             .language_tag = "",
                                             .needs_escape = needs_escape}),
                                     storage::node::identifier::RDFNodeType::Literal,
                                     node_storage.id()}};
}

Literal Literal::make_noninlined_special_unchecked(std::any &&value, storage::node::identifier::LiteralType fixed_id, NodeStorage &node_storage) noexcept {
    return Literal{NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::ValueLiteralBackendView{
                                             .datatype = fixed_id,
                                             .value = std::move(value)}),
                                     storage::node::identifier::RDFNodeType::Literal, node_storage.id()}};
}

Literal Literal::make_lang_tagged_unchecked(std::string_view lexical_form, bool needs_escape, std::string_view lang, NodeStorage &node_storage) noexcept {
    auto node_id = node_storage.find_or_make_id(storage::node::view::LexicalFormLiteralBackendView{
            .datatype_id = storage::node::identifier::NodeID::rdf_langstring_iri.first,
            .lexical_form = lexical_form,
            .language_tag = lang,
            .needs_escape = needs_escape});

    bool inlined = false;
    auto lang_tag_i = datatypes::registry::DatatypeRegistry::LangTagInlines::try_tag_to_inlined(lang);  // check if the lang_tag can be inlined
    if (lang_tag_i.has_value()) {
        auto inlined_id = datatypes::registry::DatatypeRegistry::LangTagInlines::try_into_inlined(node_id.literal_id(), lang_tag_i.value());  // check if we have enough space
        if (inlined_id.has_value()) {
            node_id = NodeID{inlined_id.value(), node_id.literal_type()};
            inlined = true;
        }
    }

    return Literal{NodeBackendHandle{node_id,
                                     storage::node::identifier::RDFNodeType::Literal,
                                     node_storage.id(), inlined}};
}

Literal Literal::make_inlined_typed_unchecked(storage::node::identifier::LiteralID inlined_value, storage::node::identifier::LiteralType fixed_id, NodeStorage const &node_storage) noexcept {
    using namespace storage::node::identifier;

    assert(fixed_id != LiteralType::other());

    return Literal{NodeBackendHandle{NodeID{inlined_value, fixed_id},
                                     RDFNodeType::Literal,
                                     node_storage.id(),
                                     true}};
}

Literal Literal::make_typed_unchecked(std::any &&value, datatypes::registry::DatatypeIDView datatype, datatypes::registry::DatatypeRegistry::DatatypeEntry const &entry, Node::NodeStorage &node_storage) noexcept {
    if (entry.inlining_ops.has_value()) {
        if (auto const maybe_inlined = entry.inlining_ops->try_into_inlined_fptr(value); maybe_inlined.has_value()) {
            return Literal::make_inlined_typed_unchecked(*maybe_inlined, datatype.get_fixed(), node_storage);
        }
    }

    if (datatype.is_fixed()) {
        if (auto const fixed_id = datatype.get_fixed(); node_storage.has_specialized_storage_for(fixed_id)) {
            return Literal::make_noninlined_special_unchecked(std::move(value), fixed_id, node_storage);
        }
    }

    return Literal::make_noninlined_typed_unchecked(entry.to_canonical_string_fptr(value),
                                                    false,
                                                    IRI{datatype, node_storage},
                                                    node_storage);
}

Literal Literal::make_string_like_copy_lang_tag(std::string_view str, Literal const &lang_tag_src, Node::NodeStorage &node_storage) noexcept {
    auto const needs_escape = lexical_form_needs_escape(str);

    if (lang_tag_src.datatype_eq<datatypes::rdf::LangString>()) {
        return Literal::make_lang_tagged_unchecked(str, needs_escape, lang_tag_src.language_tag(), node_storage);
    }

    assert(lang_tag_src.datatype_eq<datatypes::xsd::String>());
    return Literal::make_simple_unchecked(str, needs_escape, node_storage);
}

Literal Literal::lang_tagged_get_de_inlined() const noexcept {
    auto [_, id] = rdf4cpp::rdf::datatypes::registry::DatatypeRegistry::LangTagInlines::from_inlined(handle_.node_id().literal_id());
    return Literal{NodeBackendHandle{NodeID{id, this->handle_.node_id().literal_type()},
                                     handle_.type(),
                                     handle_.node_storage_id()}};
}

bool Literal::dynamic_datatype_eq_impl(std::string_view datatype) const noexcept {
    assert(!this->is_fixed());
    return this->datatype().identifier() == datatype;
}

Literal Literal::make_simple(std::string_view lexical_form, Node::NodeStorage &node_storage) {
    if (!una::is_valid_utf8(lexical_form)) {
        throw std::runtime_error{"Invalid UTF-8 in lexical form of literal"};
    }

    auto const needs_escape = lexical_form_needs_escape(lexical_form);
    return Literal::make_simple_unchecked(lexical_form, needs_escape, node_storage);
}

Literal Literal::make_simple_normalize(std::string_view lexical_form, Node::NodeStorage &node_storage) {
    auto const lex = una::norm::to_nfc_utf8(lexical_form);
    auto const needs_escape = lexical_form_needs_escape(lex);
    return Literal::make_simple_unchecked(lex, needs_escape, node_storage);
}

Literal Literal::make_lang_tagged(std::string_view lexical_form, std::string_view lang_tag,
                                  Node::NodeStorage &node_storage) {
    if (!una::is_valid_utf8(lexical_form)) [[unlikely]] {
        throw std::runtime_error{"Invalid UTF-8 in lexical form of literal"};
    }

    auto const lowercase_lang_tag = una::cases::to_lowercase_utf8(lang_tag);
    auto const needs_escape = lexical_form_needs_escape(lexical_form);
    return Literal::make_lang_tagged_unchecked(lexical_form, needs_escape, lowercase_lang_tag, node_storage);
}

Literal Literal::make_lang_tagged_normalize(std::string_view lexical_form, std::string_view lang_tag,
                                            Node::NodeStorage &node_storage) {
    auto const lowercase_lang_tag = una::cases::to_lowercase_utf8(lang_tag);
    auto const lex = una::norm::to_nfc_utf8(lexical_form);
    auto const needs_escape = lexical_form_needs_escape(lex);
    return Literal::make_lang_tagged_unchecked(lex, needs_escape, lowercase_lang_tag, node_storage);
}

Literal Literal::make_typed(std::string_view lexical_form, IRI const &datatype, Node::NodeStorage &node_storage) {
    using namespace datatypes::registry;

    DatatypeIDView const datatype_identifier{datatype};

    if (datatype_identifier == datatypes::rdf::LangString::datatype_id) {
        // see: https://www.w3.org/TR/rdf11-concepts/#section-Graph-Literal
        throw std::invalid_argument{
                "cannot construct rdf:langString without a language tag, please call one of the other factory functions"};
    }

    if (datatype_identifier == datatypes::xsd::String::datatype_id) {
        return Literal::make_simple(lexical_form, node_storage);
    }

    if (auto const *entry = DatatypeRegistry::get_entry(datatype_identifier); entry != nullptr) {
        // exists => canonize

        auto cpp_value = entry->factory_fptr(lexical_form);
        return Literal::make_typed_unchecked(std::move(cpp_value), datatype_identifier, *entry, node_storage);
    } else {
        // doesn't exist in the registry no way to canonicalize
        auto const needs_escape = lexical_form_needs_escape(lexical_form);
        return Literal::make_noninlined_typed_unchecked(lexical_form, needs_escape, datatype, node_storage);
    }
}

Literal Literal::make_boolean(util::TriBool const b, Node::NodeStorage &node_storage) {
    if (b == util::TriBool::Err) {
        return Literal{};
    }

    return Literal::make_typed_from_value<datatypes::xsd::Boolean>(b == util::TriBool::True, node_storage);
}

Literal Literal::make_string_uuid(Node::NodeStorage &node_storage) {
    boost::uuids::random_generator_mt19937 gen{};
    boost::uuids::uuid u = gen();
    std::string s = boost::uuids::to_string(u);
    return make_simple(s, node_storage);
}

Literal Literal::generate_random_double(Node::NodeStorage &node_storage) {
    struct RngState {
        std::default_random_engine rng{std::random_device{}()};
        std::uniform_real_distribution<datatypes::xsd::Double::cpp_type> dist{0.0, 1.0};
    };

    static thread_local RngState state;
    return Literal::make_typed_from_value<datatypes::xsd::Double>(state.dist(state.rng), node_storage);
}

Literal Literal::to_node_storage(NodeStorage &node_storage) const noexcept {
    using datatypes::registry::DatatypeRegistry;

    if (handle_.node_storage_id() == node_storage.id()) {
        return *this;
    }

    if (this->is_inlined()) {
        if (this->datatype_eq<datatypes::rdf::LangString>()) {
            auto const data = this->lang_tagged_get_de_inlined().backend_handle().literal_backend().get_lexical();
            return Literal::make_lang_tagged_unchecked(data.lexical_form,
                                                       data.needs_escape,
                                                       data.language_tag,
                                                       node_storage);
        }

        auto const node_id = this->handle_.node_id();
        return Literal::make_inlined_typed_unchecked(node_id.literal_id(),
                                                     node_id.literal_type(),
                                                     node_storage);
    }

    auto literal_view = handle_.literal_backend();
    auto node_id = literal_view.visit(
            [&](storage::node::view::LexicalFormLiteralBackendView &lexical_backend) noexcept {
                if (auto const dt_id = storage::node::identifier::iri_node_id_to_literal_type(lexical_backend.datatype_id);
                    dt_id.is_fixed() && node_storage.has_specialized_storage_for(dt_id)) {

                    // This node storage doesn't have specialized storage for the given type but the target node storage does.
                    // Need to send value over.
                    // This doesn't work for rdf:langString, but it shouldn't have a specialized storage anyway.
                    assert(!this->datatype_eq<datatypes::rdf::LangString>());

                    auto const from_string = DatatypeRegistry::get_factory(dt_id);
                    auto value = from_string(lexical_backend.lexical_form);

                    return node_storage.find_or_make_id(storage::node::view::ValueLiteralBackendView{.datatype = dt_id,
                                                                                                     .value = std::move(value)});
                }

                // send over IRI corresponding to this datatype
                auto const dtype_iri_view = NodeStorage::find_iri_backend_view(NodeBackendHandle{lexical_backend.datatype_id,
                                                                                                 storage::node::identifier::RDFNodeType::IRI,
                                                                                                 handle_.node_storage_id()});
                lexical_backend.datatype_id = node_storage.find_or_make_id(dtype_iri_view);

                // find or make the requested node
                return node_storage.find_or_make_id(literal_view);
            },
            [&node_storage, &literal_view](storage::node::view::ValueLiteralBackendView const &value_backend) noexcept {
                // no need to send over datatype IRI, as this having a specialized storage requires
                // that the datatype is fixed, so it must already be present

                if (!node_storage.has_specialized_storage_for(value_backend.datatype)) {
                    // target node storage is not specialized for this datatype, need to convert to lexical form
                    auto const to_string = DatatypeRegistry::get_to_canonical_string(datatypes::registry::DatatypeIDView{value_backend.datatype});
                    assert(to_string != nullptr);
                    std::string const str = to_string(value_backend.value);

                    return node_storage.find_or_make_id(storage::node::view::LexicalFormLiteralBackendView{
                            .datatype_id = storage::node::identifier::literal_type_to_iri_node_id(value_backend.datatype),
                            .lexical_form = str,
                            .language_tag = "",
                            .needs_escape = false});
                }

                // target node storage is also specialized for this datatype, directly send it over
                return node_storage.find_or_make_id(literal_view);
            });

    return Literal{NodeBackendHandle{node_id, storage::node::identifier::RDFNodeType::Literal, node_storage.id()}};
}

Literal Literal::try_get_in_node_storage(NodeStorage const &node_storage) const noexcept {
    using datatypes::registry::DatatypeRegistry;

    if (handle_.node_storage_id() == node_storage.id()) {
        return *this;
    }

    if (this->is_inlined()) {
        if (this->datatype_eq<datatypes::rdf::LangString>()) {
            // special case for rdf:langString because part of it is in the backend
            auto const literal_view = this->lang_tagged_get_de_inlined().handle_.literal_backend();

            auto const tmp_id = node_storage.find_id(literal_view);
            if (tmp_id == NodeID{}) {
                return Literal{};
            }

            auto const [node_id, inlined] = [tmp_id, &literal_view]() noexcept {
                if (auto const lang_tag_i = DatatypeRegistry::LangTagInlines::try_tag_to_inlined(literal_view.get_lexical().language_tag);
                    lang_tag_i.has_value()) {

                    if (auto const inlined_id = DatatypeRegistry::LangTagInlines::try_into_inlined(tmp_id.literal_id(), *lang_tag_i);
                        inlined_id.has_value()) {

                        return std::make_pair(NodeID{*inlined_id, tmp_id.literal_type()}, true);
                    }
                }

                return std::make_pair(tmp_id, false);
            }();

            return Literal{NodeBackendHandle{node_id,
                                             storage::node::identifier::RDFNodeType::Literal,
                                             node_storage.id(),
                                             inlined}};
        }

        auto const node_id = this->handle_.node_id();
        return Literal::make_inlined_typed_unchecked(node_id.literal_id(),
                                                     node_id.literal_type(),
                                                     node_storage);
    }

    auto literal_view = handle_.literal_backend();
    auto const node_id = literal_view.visit(
            [&](storage::node::view::LexicalFormLiteralBackendView &lexical_backend) noexcept {
                if (auto const dt_id = storage::node::identifier::iri_node_id_to_literal_type(lexical_backend.datatype_id);
                    dt_id.is_fixed() && node_storage.has_specialized_storage_for(dt_id)) {

                    // This node storage doesn't have specialized storage for the given type but the target node storage does.
                    // Need to send value over.
                    // This doesn't work for rdf:langString, but it shouldn't have a specialized storage anyway.
                    assert(!this->datatype_eq<datatypes::rdf::LangString>());

                    auto const from_string = DatatypeRegistry::get_factory(dt_id);
                    auto value = from_string(lexical_backend.lexical_form);

                    return node_storage.find_id(storage::node::view::ValueLiteralBackendView{.datatype = dt_id,
                                                                                             .value = std::move(value)});
                }

                // Default case.
                // This node storage doesn't have specialized storage for the value and the new one also doesn't
                auto const dtype_iri_view = NodeStorage::find_iri_backend_view(NodeBackendHandle{lexical_backend.datatype_id,
                                                                                                 storage::node::identifier::RDFNodeType::IRI,
                                                                                                 handle_.node_storage_id()});
                lexical_backend.datatype_id = node_storage.find_id(dtype_iri_view);
                if (lexical_backend.datatype_id == NodeID{}) {
                    // datatype IRI not present, therefore literal cannot be present
                    return NodeID{};
                }

                return node_storage.find_id(literal_view);
            },
            [&node_storage, &literal_view](storage::node::view::ValueLiteralBackendView const &value_backend) noexcept {
                // no need to send over datatype IRI, as this having a specialized storage requires
                // that the datatype is fixed, so it must already be present

                if (!node_storage.has_specialized_storage_for(value_backend.datatype)) {
                    // target node storage is not specialized for this datatype, need to convert to lexical form
                    auto const to_string = DatatypeRegistry::get_to_canonical_string(datatypes::registry::DatatypeIDView{value_backend.datatype});
                    assert(to_string != nullptr);
                    std::string const str = to_string(value_backend.value);

                    return node_storage.find_id(storage::node::view::LexicalFormLiteralBackendView{
                            .datatype_id = storage::node::identifier::literal_type_to_iri_node_id(value_backend.datatype),
                            .lexical_form = str,
                            .language_tag = "",
                            .needs_escape = false});
                }

                // target node storage is also specialized for this datatype, directly try to get it
                return node_storage.find_id(literal_view);
            });

    if (node_id == NodeID{}) {
        return Literal{};
    }

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
        return IRI{storage::node::identifier::datatype_iri_handle_for_fixed_lit_handle(handle_)};
    }

    auto const lexical = handle_.literal_backend().get_lexical();
    return IRI{NodeBackendHandle{lexical.datatype_id,
                                 storage::node::identifier::RDFNodeType::IRI,
                                 handle_.node_storage_id()}};
}

util::CowString Literal::lexical_form() const noexcept {
    if (this->is_inlined()) {
        if (this->datatype_eq<datatypes::rdf::LangString>()) {
            return this->lang_tagged_get_de_inlined().lexical_form();
        }

        auto const *entry = datatypes::registry::DatatypeRegistry::get_entry(this->datatype_id());
        assert(entry != nullptr);
        assert(entry->inlining_ops.has_value());

        auto const inlined_value = this->handle_.node_id().literal_id();
        return util::CowString{util::ownership_tag::owned, entry->to_canonical_string_fptr(entry->inlining_ops->from_inlined_fptr(inlined_value))};
    }

    return handle_.literal_backend().visit(
            [](storage::node::view::LexicalFormLiteralBackendView const &lexical_backend) noexcept {
                return util::CowString{util::ownership_tag::borrowed, lexical_backend.lexical_form};
            },
            [](storage::node::view::ValueLiteralBackendView const &value_backend) noexcept {
                auto const to_string = datatypes::registry::DatatypeRegistry::get_to_canonical_string(value_backend.datatype);
                assert(to_string != nullptr);

                return util::CowString{util::ownership_tag::owned, to_string(value_backend.value)};
            });
}

Literal Literal::as_lexical_form(NodeStorage &node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    auto const lex = this->lexical_form();
    auto const needs_escape = lexical_form_needs_escape(lex);
    return Literal::make_simple_unchecked(lex, needs_escape, node_storage);
}

util::CowString Literal::simplified_lexical_form() const noexcept {
    auto const *entry = datatypes::registry::DatatypeRegistry::get_entry(this->datatype_id());
    if (entry == nullptr) {
        return util::CowString{util::ownership_tag::borrowed, handle_.literal_backend().get_lexical().lexical_form};
    }

    if (this->is_inlined()) {
        if (this->datatype_eq<datatypes::rdf::LangString>()) {
            return this->lang_tagged_get_de_inlined().simplified_lexical_form();
        }

        assert(entry->inlining_ops.has_value());

        auto const inlined_value = handle_.node_id().literal_id();
        return util::CowString{util::ownership_tag::owned, entry->to_simplified_string_fptr(entry->inlining_ops->from_inlined_fptr(inlined_value))};
    }

    return handle_.literal_backend().visit(
            [entry](storage::node::view::LexicalFormLiteralBackendView const &lexical_backend) noexcept {
                auto const value = entry->factory_fptr(lexical_backend.lexical_form);
                return util::CowString{util::ownership_tag::owned, entry->to_simplified_string_fptr(value)};
            },
            [entry](storage::node::view::ValueLiteralBackendView const &value_backend) noexcept {
                return util::CowString{util::ownership_tag::owned, entry->to_simplified_string_fptr(value_backend.value)};
            });
}

Literal Literal::as_simplified_lexical_form(NodeStorage &node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    auto const lex = this->simplified_lexical_form();
    auto const needs_escape = lexical_form_needs_escape(lex);
    return Literal::make_simple_unchecked(lex, needs_escape, node_storage);
}

std::string_view Literal::language_tag() const noexcept {
    if (this->datatype_eq<datatypes::rdf::LangString>()) {
        if (this->is_inlined()) {
            auto [tag, _] = rdf4cpp::rdf::datatypes::registry::DatatypeRegistry::LangTagInlines::from_inlined(this->handle_.node_id().literal_id());
            return rdf4cpp::rdf::datatypes::registry::DatatypeRegistry::LangTagInlines::inlined_to_tag(tag);
        }
        return handle_.literal_backend().get_lexical().language_tag;
    }

    return "";
}

Literal Literal::as_language_tag(Node::NodeStorage &node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    return Literal::make_simple_unchecked(this->language_tag(), false, node_storage);
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

// https://www.w3.org/TR/n-triples/#grammar-production-STRING_LITERAL_QUOTE
#define TRY_WRITE_QUOTED_LEXICAL_FORM(lexical) \
    TRY_WRITE("\"");                           \
    for (char const ch : lexical) {            \
        switch (ch) {                          \
            case '"': {                        \
                TRY_WRITE(R"(\")");            \
                break;                         \
            }                                  \
            case '\\': {                       \
                TRY_WRITE(R"(\\)");            \
                break;                         \
            }                                  \
            case '\n': {                       \
                TRY_WRITE(R"(\n)");            \
                break;                         \
            }                                  \
            case '\r': {                       \
                TRY_WRITE(R"(\r)");            \
                break;                         \
            }                                  \
            [[likely]] default: {              \
                TRY_WRITE(&ch, 1);             \
                break;                         \
            }                                  \
        }                                      \
    }                                          \
    TRY_WRITE("\"");

bool Literal::serialize(char **buf, size_t *buf_size, FlushFunc const flush, void *data) const noexcept {
    if (this->null()) {
        TRY_WRITE("null");
        return true;
    }

    if (this->datatype_eq<datatypes::xsd::String>()) {
        auto const value = this->backend_handle().literal_backend().get_lexical();

        if (value.needs_escape) [[unlikely]] {
            TRY_WRITE_QUOTED_LEXICAL_FORM(value.lexical_form);
        } else {
            TRY_WRITE("\"");
            TRY_WRITE(value.lexical_form);
            TRY_WRITE("\"");
        }

        return true;
    } else if (this->datatype_eq<datatypes::rdf::LangString>()) {
        auto const value = this->lang_tagged_get_de_inlined().backend_handle().literal_backend().get_lexical();

        if (value.needs_escape) [[unlikely]] {
            TRY_WRITE_QUOTED_LEXICAL_FORM(value.lexical_form);
        } else {
            TRY_WRITE("\"");
            TRY_WRITE(value.lexical_form);
            TRY_WRITE("\"");
        }

        TRY_WRITE("@");
        TRY_WRITE(value.language_tag);
        return true;
    } else if (this->is_inlined()) {
        assert(!this->datatype_eq<datatypes::rdf::LangString>());
        // Notes:
        // 1. inlined values are assumed to not require escaping
        // 2. This is a known datatype because it is inlined => the registry contains the datatype IRI

        auto const *entry = datatypes::registry::DatatypeRegistry::get_entry(this->datatype_id());
        assert(entry != nullptr);
        assert(entry->inlining_ops.has_value());

        auto const inlined_value = this->backend_handle().node_id().literal_id();
        auto const lexical_form = entry->to_canonical_string_fptr(entry->inlining_ops->from_inlined_fptr(inlined_value));
        auto const &datatype_iri = entry->datatype_iri;

        TRY_WRITE("\"");
        TRY_WRITE(lexical_form);
        TRY_WRITE("\"^^<");
        TRY_WRITE(datatype_iri);
        TRY_WRITE(">");
        return true;
    } else {
        using storage::node::NodeStorage;
        using storage::node::identifier::NodeBackendHandle;

        return this->backend_handle().literal_backend().visit(
                [&](storage::node::view::LexicalFormLiteralBackendView const &lexical_backend) noexcept {
                    auto const &dtype_iri = NodeStorage::find_iri_backend_view(NodeBackendHandle{lexical_backend.datatype_id,
                                                                                                 storage::node::identifier::RDFNodeType::IRI,
                                                                                                 this->backend_handle().node_storage_id()});

                    if (lexical_backend.needs_escape) [[unlikely]] {
                        TRY_WRITE_QUOTED_LEXICAL_FORM(lexical_backend.lexical_form);
                    } else {
                        TRY_WRITE("\"");
                        TRY_WRITE(lexical_backend.lexical_form);
                        TRY_WRITE("\"");
                    }

                    TRY_WRITE("^^<");
                    TRY_WRITE(dtype_iri.identifier);
                    TRY_WRITE(">");
                    return true;
                },
                [&](storage::node::view::ValueLiteralBackendView const &value_backend) noexcept {
                    // Notes:
                    // 1. non-string storage values are assumed to not require escaping
                    // 2. This is a known datatype because it is stored in a value backend => the registry contains the datatype IRI

                    auto const *entry = datatypes::registry::DatatypeRegistry::get_entry(this->datatype_id());
                    assert(entry != nullptr);

                    auto const lexical_form = entry->to_canonical_string_fptr(value_backend.value);
                    auto const &datatype_iri = entry->datatype_iri;

                    TRY_WRITE("\"");
                    TRY_WRITE(lexical_form);
                    TRY_WRITE("\"^^<");
                    TRY_WRITE(datatype_iri);
                    TRY_WRITE(">");
                    return true;
                });
    }
}

#undef TRY_WRITE_QUOTED_LEXICAL_FORM

Literal::operator std::string() const noexcept {
    assert(false);
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
    assert(false);
}

std::any Literal::value() const noexcept {
    using namespace datatypes;

    auto const datatype = this->datatype_id();

    if (this->is_inlined()) {
        if (datatype == rdf::LangString::datatype_id) {
            return this->lang_tagged_get_de_inlined().value();
        }

        auto const ops = registry::DatatypeRegistry::get_inlining_ops(datatype);
        assert(ops != nullptr);

        auto const inlined_value = this->handle_.node_id().literal_id();
        return ops->from_inlined_fptr(inlined_value);
    }

    auto const backend = handle_.literal_backend();

    if (datatype == rdf::LangString::datatype_id) {
        auto const &lex = backend.get_lexical();

        return std::any{registry::LangStringRepr{
                .lexical_form = lex.lexical_form,
                .language_tag = lex.language_tag}};
    }

    if (datatype == xsd::String::datatype_id) {
        auto const &lex = backend.get_lexical();
        return std::any{lex.lexical_form};
    }

    return backend.visit(
            [&datatype](storage::node::view::LexicalFormLiteralBackendView const &lexical_backend) noexcept {
                if (auto const factory = registry::DatatypeRegistry::get_factory(datatype); factory != nullptr) {
                    return factory(lexical_backend.lexical_form);
                }

                return std::any{};
            },
            [&datatype](storage::node::view::ValueLiteralBackendView const &value_backend) noexcept {
                assert(value_backend.datatype == datatype);
                (void)datatype;

                return value_backend.value;
            });
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
        } catch (...) {
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
            auto value = this->template value<Boolean>() ? target_e->numeric_ops->get_impl().one_value_fptr()
                                                         : target_e->numeric_ops->get_impl().zero_value_fptr();

            return Literal::make_typed_unchecked(std::move(value), target_dtid, *target_e, node_storage);
        } else {
            auto const &impl_converter = DatatypeRegistry::get_numeric_op_impl_conversion(*target_e);
            auto const *target_num_impl = DatatypeRegistry::get_numerical_ops(impl_converter.target_type_id);
            assert(target_num_impl != nullptr);

            // perform conversion as impl numeric type
            auto const value = this->template value<Boolean>() ? target_num_impl->get_impl().one_value_fptr()
                                                               : target_num_impl->get_impl().zero_value_fptr();

            // downcast to target
            auto target_value = impl_converter.inverted_convert(value);

            if (!target_value.has_value()) {
                // not representable as target type
                return Literal{};
            }

            return Literal::make_typed_unchecked(std::move(*target_value), target_dtid, *target_e, node_storage);
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
        auto target_value = common_conversion->inverted_convert_rhs(common_type_value); // downcast to target
        if (!target_value.has_value()) {
            // downcast failed
            return Literal{};
        }
        return Literal::make_typed_unchecked(std::move(*target_value), target_dtid, *target_e, node_storage);
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
        DatatypeRegistry::NumericOpResult op_res = op_select(this_entry->numeric_ops->get_impl())(this->value(),
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
        return Literal::make_typed_unchecked(std::move(*op_res.result_value), op_res.result_type_id, *result_entry, node_storage);
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

        DatatypeRegistry::NumericOpResult op_res = op_select(equalized_entry->numeric_ops->get_impl())(equalizer->convert_lhs(this->value()),
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
        return Literal::make_typed_unchecked(std::move(*op_res.result_value), op_res.result_type_id, *result_entry, node_storage);
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

    auto const [operand_entry, value] = [&]() noexcept {
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

    DatatypeRegistry::NumericOpResult op_res = op_select(operand_entry->numeric_ops->get_impl())(value);

    auto const *result_entry = [&op_res, operand_entry = operand_entry]() {
        if (op_res.result_type_id == DatatypeIDView{operand_entry->datatype_iri}) [[likely]] {
            return operand_entry;
        } else [[unlikely]] {
            return DatatypeRegistry::get_entry(op_res.result_type_id);
        }
    }();

    assert(result_entry != nullptr);
    return Literal::make_typed_unchecked(std::move(*op_res.result_value), op_res.result_type_id, *result_entry, node_storage);
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

    // TODO: this does not cover custom types deriving from xsd:string or xsd:langString
    return type == datatypes::xsd::String::fixed_id || type == datatypes::rdf::LangString::fixed_id;
}

Literal Literal::add(Literal const &other, Node::NodeStorage &node_storage) const noexcept {
    // DayTimeDuration + DayTimeDuration
    {
        auto this_v = this->cast_to_supertype_value<datatypes::xsd::DayTimeDuration>();
        auto other_v = other.cast_to_supertype_value<datatypes::xsd::DayTimeDuration>();
        if (this_v.has_value() && other_v.has_value()) {
            auto r = datatypes::registry::util::to_checked(*this_v) + datatypes::registry::util::to_checked(*other_v);
            if (r.count().is_invalid())
                return Literal{};

            return make_typed_from_value<datatypes::xsd::DayTimeDuration>(datatypes::registry::util::from_checked(r));
        }
    }
    // YearMonthDuration + YearMonthDuration
    {
        auto this_v = this->cast_to_supertype_value<datatypes::xsd::YearMonthDuration>();
        auto other_v = other.cast_to_supertype_value<datatypes::xsd::YearMonthDuration>();
        if (this_v.has_value() && other_v.has_value()) {
            auto r = datatypes::registry::util::to_checked(*this_v) + datatypes::registry::util::to_checked(*other_v);
            if (r.count().is_invalid())
                return Literal{};

            return make_typed_from_value<datatypes::xsd::YearMonthDuration>(datatypes::registry::util::from_checked(r));
        }
    }
    // DateTime(&Subclasses) + Duration(&Subclasses)
    {
        auto this_dt = this->cast_to_supertype_value<datatypes::xsd::DateTime>();
        auto other_dt = other.cast_to_supertype_value<datatypes::xsd::Duration>();
        if (this_dt.has_value() && other_dt.has_value()) {
            if (this->datatype_eq<datatypes::xsd::Time>() && other.datatype_eq<datatypes::xsd::YearMonthDuration>())
                return Literal{};

            auto tp = datatypes::registry::util::add_duration_to_date_time(this_dt->first, *other_dt);
            if (tp.time_since_epoch().count().is_invalid())
                return Literal{};

            auto r = datatypes::registry::util::from_checked(tp);

            if (this->datatype_eq<datatypes::xsd::Date>())
                return make_typed_from_value<datatypes::xsd::Date>(std::make_pair(std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(r)}, this_dt->second));

            if (this->datatype_eq<datatypes::xsd::Time>())
                return make_typed_from_value<datatypes::xsd::Time>(std::make_pair(r - std::chrono::floor<std::chrono::days>(r), this_dt->second));

            return make_typed_from_value<datatypes::xsd::DateTime>(std::make_pair(r, this_dt->second));
        }
    }

    return this->numeric_binop_impl(
            [](auto const &num_ops) noexcept {
                return num_ops.add_fptr;
            },
            other, node_storage);
}

Literal Literal::operator+(Literal const &other) const noexcept {
    return this->add(other);
}

Literal &Literal::add_assign(const Literal &other, NodeStorage &node_storage) noexcept {
    auto result = this->add(other, node_storage);
    this->handle_ = result.handle_;
    return *this;
}

Literal &Literal::operator+=(const Literal &other) noexcept {
    return this->add_assign(other);
}

Literal Literal::sub(Literal const &other, Node::NodeStorage &node_storage) const noexcept {
    // DateTime(&Subclasses) - DateTime(&Subclasses)
    if (this->datatype_id() == other.datatype_id()) {
        auto this_dt = this->cast_to_supertype_value<datatypes::xsd::DateTime>();
        auto other_dt = other.cast_to_supertype_value<datatypes::xsd::DateTime>();
        if (this_dt.has_value() && other_dt.has_value()) {
            datatypes::registry::util::CheckedZonedTime const this_tp{this_dt->second.has_value() ? *this_dt->second : util::Timezone{},
                                                                      datatypes::registry::util::to_checked(this_dt->first)};

            datatypes::registry::util::CheckedZonedTime const other_tp{other_dt->second.has_value() ? *other_dt->second : util::Timezone{},
                                                                       datatypes::registry::util::to_checked(other_dt->first)};

            auto d = this_tp.get_sys_time() - other_tp.get_sys_time();
            if (d.count().is_invalid())
                return Literal{};

            return make_typed_from_value<datatypes::xsd::DayTimeDuration>(datatypes::registry::util::from_checked(d));
        }
    }
    // DateTime(&Subclasses) - Duration(&Subclasses)
    {
        auto this_dt = this->cast_to_supertype_value<datatypes::xsd::DateTime>();
        auto other_dt = other.cast_to_supertype_value<datatypes::xsd::Duration>();
        if (this_dt.has_value() && other_dt.has_value()) {
            auto tp = datatypes::registry::util::add_duration_to_date_time(this_dt->first, std::make_pair(-other_dt->first, -other_dt->second));
            if (tp.time_since_epoch().count().is_invalid())
                return Literal{};

            auto r = datatypes::registry::util::from_checked(tp);
            if (this->datatype_eq<datatypes::xsd::Date>())
                return make_typed_from_value<datatypes::xsd::Date>(std::make_pair(std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(r)}, this_dt->second));

            if (this->datatype_eq<datatypes::xsd::Time>())
                return make_typed_from_value<datatypes::xsd::Time>(std::make_pair(r - std::chrono::floor<std::chrono::days>(r), this_dt->second));

            return make_typed_from_value<datatypes::xsd::DateTime>(std::make_pair(r, this_dt->second));
        }
    }
    // DayTimeDuration - DayTimeDuration
    {
        auto this_v = this->cast_to_supertype_value<datatypes::xsd::DayTimeDuration>();
        auto other_v = other.cast_to_supertype_value<datatypes::xsd::DayTimeDuration>();
        if (this_v.has_value() && other_v.has_value()) {
            auto r = datatypes::registry::util::to_checked(*this_v) - datatypes::registry::util::to_checked(*other_v);
            if (r.count().is_invalid())
                return Literal{};

            return make_typed_from_value<datatypes::xsd::DayTimeDuration>(datatypes::registry::util::from_checked(r));
        }
    }
    // YearMonthDuration - YearMonthDuration
    {
        auto this_v = this->cast_to_supertype_value<datatypes::xsd::YearMonthDuration>();
        auto other_v = other.cast_to_supertype_value<datatypes::xsd::YearMonthDuration>();
        if (this_v.has_value() && other_v.has_value()) {
            auto r = datatypes::registry::util::to_checked(*this_v) - datatypes::registry::util::to_checked(*other_v);
            if (r.count().is_invalid())
                return Literal{};

            return make_typed_from_value<datatypes::xsd::YearMonthDuration>(datatypes::registry::util::from_checked(r));
        }
    }

    return this->numeric_binop_impl(
            [](auto const &num_ops) noexcept {
                return num_ops.sub_fptr;
            },
            other, node_storage);
}

Literal Literal::operator-(Literal const &other) const noexcept {
    return this->sub(other);
}

Literal &Literal::sub_assign(const Literal &other, NodeStorage &node_storage) noexcept {
    auto result = this->sub(other, node_storage);
    this->handle_ = result.handle_;
    return *this;
}

Literal &Literal::operator-=(const Literal &other) noexcept {
    return this->sub_assign(other);
}

Literal Literal::mul(Literal const &other, Node::NodeStorage &node_storage) const noexcept {
    // YearMonthDuration * double
    {
        auto this_v = this->cast_to_supertype_value<datatypes::xsd::YearMonthDuration>();
        auto other_v = other.cast_to_supertype_value<datatypes::xsd::Double>();
        if (this_v.has_value() && other_v.has_value()) {
            auto r = std::round(static_cast<double>(this_v->count()) * *other_v);
            if (!datatypes::registry::util::fits_into<int64_t>(r))
                return Literal{};

            return make_typed_from_value<datatypes::xsd::YearMonthDuration>(std::chrono::months{static_cast<int64_t>(r)});
        }
    }
    // DayTimeDuration * double
    {
        auto this_v = this->cast_to_supertype_value<datatypes::xsd::DayTimeDuration>();
        auto other_v = other.cast_to_supertype_value<datatypes::xsd::Double>();
        if (this_v.has_value() && other_v.has_value()) {
            auto r = std::round(static_cast<double>(this_v->count()) * *other_v);
            if (!datatypes::registry::util::fits_into<int64_t>(r))
                return Literal{};

            return make_typed_from_value<datatypes::xsd::DayTimeDuration>(std::chrono::milliseconds{static_cast<int64_t>(r)});
        }
    }

    return this->numeric_binop_impl(
            [](auto const &num_ops) noexcept {
                return num_ops.mul_fptr;
            },
            other, node_storage);
}

Literal Literal::operator*(Literal const &other) const noexcept {
    return this->mul(other);
}

Literal &Literal::mul_assign(const Literal &other, NodeStorage &node_storage) noexcept {
    auto result = this->mul(other, node_storage);
    this->handle_ = result.handle_;
    return *this;
}

Literal &Literal::operator*=(const Literal &other) noexcept {
    return this->mul_assign(other);
}

Literal Literal::div(Literal const &other, Node::NodeStorage &node_storage) const noexcept {
    // YearMonthDuration / double
    {
        auto this_v = this->cast_to_supertype_value<datatypes::xsd::YearMonthDuration>();
        auto other_v = other.cast_to_supertype_value<datatypes::xsd::Double>();
        if (this_v.has_value() && other_v.has_value()) {
            auto r = static_cast<int64_t>(std::round(static_cast<double>(this_v->count()) / *other_v));
            if (!datatypes::registry::util::fits_into<int64_t>(r))
                return Literal{};

            return make_typed_from_value<datatypes::xsd::YearMonthDuration>(std::chrono::months{r});
        }
    }
    // DayTimeDuration / double
    {
        auto this_v = this->cast_to_supertype_value<datatypes::xsd::DayTimeDuration>();
        auto other_v = other.cast_to_supertype_value<datatypes::xsd::Double>();
        if (this_v.has_value() && other_v.has_value()) {
            auto r = static_cast<int64_t>(std::round(static_cast<double>(this_v->count()) / *other_v));
            if (!datatypes::registry::util::fits_into<int64_t>(r))
                return Literal{};

            return make_typed_from_value<datatypes::xsd::DayTimeDuration>(std::chrono::milliseconds{r});
        }
    }
    // YearMonthDuration / YearMonthDuration
    {
        auto this_v = this->cast_to_supertype_value<datatypes::xsd::YearMonthDuration>();
        auto other_v = other.cast_to_supertype_value<datatypes::xsd::YearMonthDuration>();
        if (this_v.has_value() && other_v.has_value()) {
            auto r = util::BigDecimal<>{this_v->count(), 0}.div_checked(util::BigDecimal<>{other_v->count(), 0}, 1000);
            if (!r.has_value())
                return Literal{};

            return make_typed_from_value<datatypes::xsd::Decimal>(*r);
        }
    }
    // DayTimeDuration / DayTimeDuration
    {
        auto this_v = this->cast_to_supertype_value<datatypes::xsd::DayTimeDuration>();
        auto other_v = other.cast_to_supertype_value<datatypes::xsd::DayTimeDuration>();
        if (this_v.has_value() && other_v.has_value()) {
            auto r = util::BigDecimal<>{this_v->count(), 0}.div_checked(util::BigDecimal<>{other_v->count(), 0}, 1000);
            if (!r.has_value())
                return Literal{};

            return make_typed_from_value<datatypes::xsd::Decimal>(*r);
        }
    }

    return this->numeric_binop_impl(
            [](auto const &num_ops) noexcept {
                return num_ops.div_fptr;
            },
            other, node_storage);
}

Literal Literal::operator/(Literal const &other) const noexcept {
    return this->div(other);
}

Literal &Literal::div_assign(const Literal &other, NodeStorage &node_storage) noexcept {
    auto result = this->div(other, node_storage);
    this->handle_ = result.handle_;
    return *this;
}

Literal &Literal::operator/=(const Literal &other) noexcept {
    return this->div_assign(other);
}

Literal Literal::pos(Node::NodeStorage &node_storage) const noexcept {
    return this->numeric_unop_impl(
            [](auto const &num_ops) noexcept {
                return num_ops.pos_fptr;
            },
            node_storage);
}

Literal Literal::operator+() const noexcept {
    return this->pos();
}

Literal Literal::neg(Node::NodeStorage &node_storage) const noexcept {
    return this->numeric_unop_impl(
            [](auto const &num_ops) noexcept {
                return num_ops.neg_fptr;
            },
            node_storage);
}

Literal Literal::operator-() const noexcept {
    return this->neg();
}

Literal Literal::abs(Node::NodeStorage &node_storage) const noexcept {
    return this->numeric_unop_impl(
            [](auto const &num_ops) noexcept {
                return num_ops.abs_fptr;
            },
            node_storage);
}

Literal Literal::round(Node::NodeStorage &node_storage) const noexcept {
    return this->numeric_unop_impl(
            [](auto const &num_ops) noexcept {
                return num_ops.round_fptr;
            },
            node_storage);
}

Literal Literal::floor(Node::NodeStorage &node_storage) const noexcept {
    return this->numeric_unop_impl(
            [](auto const &num_ops) noexcept {
                return num_ops.floor_fptr;
            },
            node_storage);
}

Literal Literal::ceil(NodeStorage &node_storage) const noexcept {
    return this->numeric_unop_impl(
            [](auto const &num_ops) noexcept {
                return num_ops.ceil_fptr;
            },
            node_storage);
}

util::TriBool Literal::ebv() const noexcept {
    if (this->null()) {
        return util::TriBool::Err;
    }

    auto const ebv = datatypes::registry::DatatypeRegistry::get_ebv(this->datatype_id());

    if (ebv == nullptr) {
        return util::TriBool::Err;
    }

    return ebv(this->value());
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
    auto u = lf.view() | una::views::utf8;
    return std::distance(u.begin(), u.end());
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

    auto r = una::casesens::search_utf8(s.view(), needle);
    return static_cast<bool>(r);
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

    if (!needle.is_string_like())
        return Literal::make_boolean(true, node_storage);

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

    const auto r = una::casesens::search_utf8(s.view(), needle);

    if (!r)
        return Literal::make_simple_unchecked("", false, node_storage);

    auto substr = static_cast<std::string_view>(s.view()).substr(0, r.pos());  // search_utf8 returns byte position, not unicode character position
    return Literal::make_string_like_copy_lang_tag(substr, *this, node_storage);
}

Literal Literal::substr_before(Literal const &needle, Node::NodeStorage &node_storage) const noexcept {
    if (needle.datatype_eq<datatypes::rdf::LangString>() && this->language_tag() != needle.language_tag()) {
        return Literal{};
    }

    if (!needle.is_string_like()) {
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

    const auto r = una::casesens::search_utf8(s.view(), needle);

    if (!r)
        return Literal::make_simple_unchecked("", false, node_storage);

    auto substr = s.view().substr(
            r.pos() + needle.size());  // search_utf8 returns byte position, not unicode character position
    return Literal::make_string_like_copy_lang_tag(substr, *this, node_storage);
}

Literal Literal::substr_after(Literal const &needle, Node::NodeStorage &node_storage) const noexcept {
    if (needle.datatype_eq<datatypes::rdf::LangString>() && this->language_tag() != needle.language_tag()) {
        return Literal{};
    }

    if (!needle.is_string_like()) {
        return Literal{};
    }

    return this->substr_after(needle.lexical_form(), node_storage);
}

util::TriBool Literal::str_starts_with(std::string_view const needle) const noexcept {
    if (!this->is_string_like()) {
        return util::TriBool::Err;
    }

    auto const s = this->lexical_form();

    auto norm_needle = needle | una::ranges::views::utf8;
    auto len = std::ranges::distance(norm_needle);
    // TODO use std::ranges::starts_with as soon as c++ 23 arrives
    return std::ranges::equal(norm_needle, s.view() | una::ranges::views::utf8 | una::views::take(len));
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

util::TriBool Literal::str_ends_with(std::string_view needle) const noexcept {
    if (!this->is_string_like()) {
        return util::TriBool::Err;
    }

    auto const s = this->lexical_form();
    auto norm_needle = needle | una::views::utf8;
    auto norm_this = s.view() | una::views::utf8;
    auto const len_needle = std::ranges::distance(norm_needle);
    auto const len_this = std::ranges::distance(norm_this);

    // TODO use std::ranges::ends_with as soon as c++ 23 arrives
    if (len_needle > len_this)
        return false;

    return std::ranges::equal(norm_needle, norm_this | una::views::drop(len_this - len_needle));
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
    auto const upper = una::cases::to_uppercase_utf8(s.view());

    return Literal::make_string_like_copy_lang_tag(upper, *this, node_storage);
}

Literal Literal::lowercase(Node::NodeStorage &node_storage) const noexcept {
    if (!this->is_string_like()) {
        return Literal{};
    }

    auto const s = this->lexical_form();
    const auto lower = una::cases::to_lowercase_utf8(s.view());

    return Literal::make_string_like_copy_lang_tag(lower, *this, node_storage);
}

Literal Literal::concat(Literal const &other, Node::NodeStorage &node_storage) const noexcept {
    if (this->null() || other.null()) {
        return Literal{};
    }

    auto const this_lex = this->lexical_form();
    auto const other_lex = other.lexical_form();

    std::string combined;
    combined.reserve(this_lex.size() + other_lex.size());
    combined.append(this_lex);
    combined.append(other_lex);

    auto const needs_escape = lexical_form_needs_escape(combined); // TODO not optimal

    if (this->datatype_eq<datatypes::rdf::LangString>() && other.datatype_eq<datatypes::rdf::LangString>()) {
        if (auto const lang = this->language_tag(); lang == other.language_tag()) {
            return Literal::make_lang_tagged_unchecked(combined, needs_escape, lang, node_storage);
        }
    }

    return Literal::make_simple_unchecked(combined, needs_escape, node_storage);
}

Literal Literal::encode_for_uri(std::string_view string, NodeStorage &node_storage) {
    if (!una::is_valid_utf8(string))
        return Literal{};

    std::stringstream stream{};
    stream << std::hex << std::setfill('0');
    // note that ASCII is a subset of UTF-8
    auto is_valid = [](const uint32_t cp) {
        if (cp >= static_cast<uint32_t>(static_cast<unsigned char>('A')) && cp <= static_cast<uint32_t>(static_cast<unsigned char>('Z')))
            return true;
        if (cp >= static_cast<uint32_t>(static_cast<unsigned char>('a')) && cp <= static_cast<uint32_t>(static_cast<unsigned char>('z')))
            return true;
        if (cp >= static_cast<uint32_t>(static_cast<unsigned char>('0')) && cp <= static_cast<uint32_t>(static_cast<unsigned char>('9')))
            return true;
        if (cp == static_cast<uint32_t>(static_cast<unsigned char>('-')) || cp == static_cast<uint32_t>(static_cast<unsigned char>('_')) ||
            cp == static_cast<uint32_t>(static_cast<unsigned char>('.')) || cp == static_cast<uint32_t>(static_cast<unsigned char>('~')))
            return true;
        return false;
    };
    for (const uint32_t cp : string | una::views::utf8) {
        if (is_valid(cp)) {
            stream << std::nouppercase << static_cast<char>(static_cast<unsigned char>(cp));  // all URI allowed characters are ASCII, so this cast is valid
        } else {
            const std::array<uint32_t, 1> data{cp};
            const auto r = data | una::ranges::to_utf8<std::string>();  // at maximum 4 bytes + zero, guaranteed to fit into small string optimization
            for (const char c : r) {
                stream << '%';
                stream << std::uppercase << std::setw(2) << static_cast<unsigned int>(static_cast<unsigned char>(c));
            }
        }
    }
    return make_simple_unchecked(stream.view(), false, node_storage);
}

Literal Literal::encode_for_uri(NodeStorage &node_storage) const {
    if (!this->is_string_like())
        return Literal{};
    const auto s = this->lexical_form();
    return encode_for_uri(s, node_storage);
}

Literal Literal::substr(size_t start, size_t len, Node::NodeStorage &node_storage) const noexcept {
    if (!this->is_string_like()) {
        return Literal{};
    }

    auto const s = this->lexical_form();

    auto substr = s.view() | una::ranges::views::utf8 | una::views::drop(start) | una::views::take(len) | una::ranges::to_utf8<std::string>();

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

Literal Literal::hash_with(const char *alg, NodeStorage &node_storage) const {
    if (this->handle_.node_id().literal_type() != datatypes::xsd::String::fixed_id)
        return Literal{};

    auto s = this->lexical_form();
    auto v = s.view();

    unsigned char hash_buffer[EVP_MAX_MD_SIZE];
    size_t len = 0;

    if (!EVP_Q_digest(nullptr, alg, nullptr, &v[0], v.size(), hash_buffer, &len))
        return Literal{};

    std::stringstream stream{};

    stream << std::hex << std::setfill('0');
    for (size_t i = 0; i < len; ++i)
        stream << std::setw(2) << static_cast<unsigned int>(hash_buffer[i]);

    return Literal::make_simple(stream.view(), node_storage);
}

Literal Literal::md5(NodeStorage &node_storage) const {
    return this->hash_with("MD5", node_storage);
}

Literal Literal::sha1(NodeStorage &node_storage) const {
    return this->hash_with("SHA1", node_storage);
}

Literal Literal::sha256(NodeStorage &node_storage) const {
    return this->hash_with("SHA2-256", node_storage);
}

Literal Literal::sha384(NodeStorage &node_storage) const {
    return this->hash_with("SHA2-384", node_storage);
}

Literal Literal::sha512(NodeStorage &node_storage) const {
    return this->hash_with("SHA2-512", node_storage);
}

Literal Literal::now(NodeStorage &node_storage) noexcept {
    auto n = std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now());
    util::Timezone tz{};
    rdf::util::TimePoint t = tz.to_local(n);
    rdf::util::OptionalTimezone opt = std::nullopt;
    return make_typed_from_value<datatypes::xsd::DateTime>(std::make_pair(t, opt), node_storage);
}

std::optional<std::chrono::year> Literal::year() const noexcept {
    if (!datatype_eq<datatypes::xsd::DateTime>() && !datatype_eq<datatypes::xsd::DateTimeStamp>() && !datatype_eq<datatypes::xsd::Date>()
            && !datatype_eq<datatypes::xsd::GYearMonth>() && !datatype_eq<datatypes::xsd::GYear>())
        return std::nullopt;
    auto casted = this->cast_to_value<datatypes::xsd::DateTime>();
    if (!casted.has_value())
        return std::nullopt;
    return std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(casted->first)}.year();
}

Literal Literal::as_year(NodeStorage &node_storage) const noexcept {
    auto r = this->year();
    if (!r.has_value())
        return Literal{};
    return Literal::make_typed_from_value<datatypes::xsd::Integer>(static_cast<int>(*r), node_storage);
}

std::optional<std::chrono::month> Literal::month() const noexcept {
    if (!datatype_eq<datatypes::xsd::DateTime>() && !datatype_eq<datatypes::xsd::DateTimeStamp>() && !datatype_eq<datatypes::xsd::Date>()
        && !datatype_eq<datatypes::xsd::GYearMonth>() && !datatype_eq<datatypes::xsd::GMonthDay>() && !datatype_eq<datatypes::xsd::GMonth>())
        return std::nullopt;
    auto casted = this->cast_to_value<datatypes::xsd::DateTime>();
    if (!casted.has_value())
        return std::nullopt;
    return std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(casted->first)}.month();
}

Literal Literal::as_month(NodeStorage &node_storage) const noexcept {
    auto r = this->month();
    if (!r.has_value())
        return Literal{};
    return Literal::make_typed_from_value<datatypes::xsd::Integer>(static_cast<unsigned int>(*r), node_storage);
}

std::optional<std::chrono::day> Literal::day() const noexcept {
    if (!datatype_eq<datatypes::xsd::DateTime>() && !datatype_eq<datatypes::xsd::DateTimeStamp>() && !datatype_eq<datatypes::xsd::Date>()
        && !datatype_eq<datatypes::xsd::GMonthDay>() && !datatype_eq<datatypes::xsd::GDay>())
        return std::nullopt;
    auto casted = this->cast_to_value<datatypes::xsd::DateTime>();
    if (!casted.has_value())
        return std::nullopt;
    return std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(casted->first)}.day();
}

Literal Literal::as_day(NodeStorage &node_storage) const noexcept {
    auto r = this->day();
    if (!r.has_value())
        return Literal{};
    return Literal::make_typed_from_value<datatypes::xsd::Integer>(static_cast<unsigned int>(*r), node_storage);
}

std::optional<std::chrono::hours> Literal::hours() const noexcept {
    if (!datatype_eq<datatypes::xsd::DateTime>() && !datatype_eq<datatypes::xsd::DateTimeStamp>() && !datatype_eq<datatypes::xsd::Time>())
        return std::nullopt;
    auto casted = this->cast_to_value<datatypes::xsd::DateTime>();
    if (!casted.has_value())
        return std::nullopt;
    auto day = std::chrono::floor<std::chrono::days>(casted->first);
    auto time = (casted->first - day);
    return std::chrono::hh_mm_ss{time}.hours();
}

Literal Literal::as_hours(NodeStorage &node_storage) const noexcept {
    auto r = this->hours();
    if (!r.has_value())
        return Literal{};
    return Literal::make_typed_from_value<datatypes::xsd::Integer>(r->count(), node_storage);
}

std::optional<std::chrono::minutes> Literal::minutes() const noexcept {
    if (!datatype_eq<datatypes::xsd::DateTime>() && !datatype_eq<datatypes::xsd::DateTimeStamp>() && !datatype_eq<datatypes::xsd::Time>())
        return std::nullopt;
    auto casted = this->cast_to_value<datatypes::xsd::DateTime>();
    if (!casted.has_value())
        return std::nullopt;
    auto day = std::chrono::floor<std::chrono::days>(casted->first);
    auto time = (casted->first - day);
    return std::chrono::hh_mm_ss{time}.minutes();
}

Literal Literal::as_minutes(NodeStorage &node_storage) const noexcept {
    auto r = this->minutes();
    if (!r.has_value())
        return Literal{};
    return Literal::make_typed_from_value<datatypes::xsd::Integer>(r->count(), node_storage);
}

std::optional<std::chrono::milliseconds> Literal::seconds() const noexcept {
    if (!datatype_eq<datatypes::xsd::DateTime>() && !datatype_eq<datatypes::xsd::DateTimeStamp>() && !datatype_eq<datatypes::xsd::Time>())
        return std::nullopt;
    auto casted = this->cast_to_value<datatypes::xsd::DateTime>();
    if (!casted.has_value())
        return std::nullopt;
    auto day = std::chrono::floor<std::chrono::days>(casted->first);
    auto time = std::chrono::hh_mm_ss{casted->first - day};
    return time.seconds() + time.subseconds();
}

Literal Literal::as_seconds(NodeStorage &node_storage) const noexcept {
    auto r = this->seconds();
    if (!r.has_value())
        return Literal{};
    return Literal::make_typed_from_value<datatypes::xsd::Decimal>(rdf::util::BigDecimal<>{r->count(), 3}, node_storage);
}

std::optional<util::Timezone> Literal::timezone() const noexcept {
    auto casted = this->cast_to_value<datatypes::xsd::DateTime>();
    if (!casted.has_value())
        return std::nullopt;
    auto tz = casted->second;
    if (!tz.has_value())
        return std::nullopt;
    return *tz;
}

Literal Literal::as_timezone(NodeStorage &node_storage) const noexcept {
    auto r = this->timezone();
    if (!r.has_value())
        return Literal{};
    return Literal::make_typed_from_value<datatypes::xsd::DayTimeDuration>(r->offset, node_storage);
}

std::optional<std::string> Literal::tz() const noexcept {
    auto casted = this->cast_to_value<datatypes::xsd::DateTime>();
    if (!casted.has_value())
        return std::nullopt;
    auto tz = casted->second;
    if (!tz.has_value())
        return "";
    return tz->to_canonical_string();
}
Literal Literal::as_tz(NodeStorage &node_storage) const noexcept {
    auto r = this->tz();
    if (!r.has_value())
        return Literal{};
    return Literal::make_simple_unchecked(*r, false, node_storage);
}

std::string normalize_unicode(std::string_view utf8) {
    return una::norm::to_nfc_utf8(utf8);
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
