#include "Literal.hpp"

#include <algorithm>
#include <execution>
#include <random>
#include <ranges>
#include <sstream>
#include <array>

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <uni_algo/all.h>

#include <rdf4cpp/writer/BufWriter.hpp>
#include <rdf4cpp/writer/TryWrite.hpp>
#include <rdf4cpp/IRI.hpp>
#include <rdf4cpp/datatypes/registry/util/DateTimeUtils.hpp>
#include <rdf4cpp/util/CaseInsensitiveCharTraits.hpp>
#include <rdf4cpp/writer/Prefixes.hpp>
#include <rdf4cpp/util/CharMatcher.hpp>

#include <openssl/evp.h>

namespace rdf4cpp {
bool Literal::lexical_form_needs_escape(std::string_view const lexical_form) noexcept {
    static constexpr datatypes::registry::util::ConstexprString pattern = "\"\\\n\r";
    return util::char_matcher_detail::contains_any(lexical_form, pattern);
}

template<typename T, typename S>
static std::string run_serialize(S serialize, T const &value) {
    return writer::StringWriter::oneshot([&serialize, &value](writer::StringWriter &w) noexcept {
        return std::invoke(serialize, value, w);
    });
}

Literal::Literal(storage::identifier::NodeBackendHandle handle) noexcept : Node{handle} {
}

Literal::Literal() noexcept : Node{storage::identifier::NodeBackendHandle{{}, storage::identifier::RDFNodeType::Literal, {}}} {
}

Literal Literal::make_null() noexcept {
    return Literal{};
}

Literal Literal::make_simple_unchecked(std::string_view lexical_form, bool needs_escape, storage::DynNodeStoragePtr node_storage) {
    return Literal{storage::identifier::NodeBackendHandle{node_storage.find_or_make_id(storage::view::LexicalFormLiteralBackendView{
                                                                .datatype_id = storage::identifier::NodeBackendID::xsd_string_iri.first,
                                                                .lexical_form = lexical_form,
                                                                .language_tag = "",
                                                                .needs_escape = needs_escape}),
                                                          node_storage}};
}

Literal Literal::make_noninlined_typed_unchecked(std::string_view lexical_form, bool needs_escape, IRI const &datatype, storage::DynNodeStoragePtr node_storage) {
    return Literal{storage::identifier::NodeBackendHandle{node_storage.find_or_make_id(storage::view::LexicalFormLiteralBackendView{
                                                                .datatype_id = datatype.to_node_storage(node_storage).backend_handle().id(),
                                                                .lexical_form = lexical_form,
                                                                .language_tag = "",
                                                                .needs_escape = needs_escape}),
                                                          node_storage}};
}

Literal Literal::make_noninlined_special_unchecked(std::any &&value, storage::identifier::LiteralType fixed_id, storage::DynNodeStoragePtr node_storage) {
    return Literal{storage::identifier::NodeBackendHandle{node_storage.find_or_make_id(storage::view::ValueLiteralBackendView{
                                                                .datatype = fixed_id,
                                                                .value = std::move(value)}),
                                                          node_storage}};
}

Literal Literal::make_lang_tagged_unchecked(std::string_view lexical_form, bool needs_escape, std::string_view lang, storage::DynNodeStoragePtr node_storage) {
    auto node_id = node_storage.find_or_make_id(storage::view::LexicalFormLiteralBackendView{
            .datatype_id = storage::identifier::NodeBackendID::rdf_langstring_iri.first,
            .lexical_form = lexical_form,
            .language_tag = lang,
            .needs_escape = needs_escape});

    return make_lang_tagged_unchecked_from_node_id(lang, node_storage, node_id);
}
Literal Literal::make_lang_tagged_unchecked_from_node_id(std::string_view lang, storage::DynNodeStoragePtr node_storage, storage::identifier::NodeBackendID node_id) noexcept {
    using namespace storage::identifier;

    auto const lang_tag_i = datatypes::registry::DatatypeRegistry::LangTagInlines::try_tag_to_inlined(lang);  // check if the lang_tag can be inlined
    if (!lang_tag_i.has_value()) {
        return Literal{NodeBackendHandle{node_id, node_storage}};
    }

    auto const inlined_id = datatypes::registry::DatatypeRegistry::LangTagInlines::try_into_inlined(node_id.node_id().literal_id(), *lang_tag_i);  // check if we have enough space#
    if (!inlined_id.has_value()) {
        return Literal{NodeBackendHandle{node_id, node_storage}};
    }

    return Literal{NodeBackendHandle{NodeBackendID{NodeID{*inlined_id, node_id.node_id().literal_type()},
                                                   RDFNodeType::Literal,
                                                   true,
                                                   node_id.free_tagging_bits()},
                                     node_storage}};
}

Literal Literal::make_inlined_typed_unchecked(storage::identifier::LiteralID inlined_value, storage::identifier::LiteralType fixed_id, storage::DynNodeStoragePtr node_storage) noexcept {
    using namespace storage::identifier;

    assert(fixed_id != LiteralType::other());

    return Literal{NodeBackendHandle{NodeID{inlined_value, fixed_id},
                                     RDFNodeType::Literal,
                                     node_storage,
                                     true}};
}

Literal Literal::make_typed_unchecked(std::any &&value, datatypes::registry::DatatypeIDView datatype, datatypes::registry::DatatypeRegistry::DatatypeEntry const &entry, storage::DynNodeStoragePtr node_storage) {
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

    auto const lex = run_serialize(entry.serialize_canonical_string_fptr, value);

    return Literal::make_noninlined_typed_unchecked(lex,
                                                    false,
                                                    IRI{datatype, node_storage},
                                                    node_storage);
}

Literal Literal::make_string_like_copy_lang_tag(std::string_view str, Literal const &lang_tag_src, storage::DynNodeStoragePtr node_storage) {
    auto const needs_escape = lexical_form_needs_escape(str);

    if (lang_tag_src.datatype_eq<datatypes::rdf::LangString>()) {
        return Literal::make_lang_tagged_unchecked(str, needs_escape, lang_tag_src.language_tag(), node_storage);
    }

    assert(lang_tag_src.datatype_eq<datatypes::xsd::String>());
    return Literal::make_simple_unchecked(str, needs_escape, node_storage);
}

Literal Literal::lang_tagged_get_de_inlined() const noexcept {
    auto [_, id] = datatypes::registry::DatatypeRegistry::LangTagInlines::from_inlined(handle_.node_id().literal_id());
    return Literal{storage::identifier::NodeBackendHandle{storage::identifier::NodeID{id, this->handle_.node_id().literal_type()},
                                                          handle_.type(),
                                                          handle_.storage(),
                                                          false,
                                                          handle_.free_tagging_bits()}};
}

bool Literal::dynamic_datatype_eq_impl(std::string_view datatype) const noexcept {
    assert(!this->is_fixed());
    return this->datatype().identifier() == datatype;
}

Literal Literal::make_simple(std::string_view lexical_form, storage::DynNodeStoragePtr node_storage) {
    if (!una::is_valid_utf8(lexical_form)) {
        throw InvalidNode{"Invalid UTF-8 in lexical form of literal"};
    }

    auto const needs_escape = lexical_form_needs_escape(lexical_form);
    return Literal::make_simple_unchecked(lexical_form, needs_escape, node_storage);
}

Literal Literal::make_simple_normalize(std::string_view lexical_form, storage::DynNodeStoragePtr node_storage) {
    auto const lex = una::norm::to_nfc_utf8(lexical_form);
    auto const needs_escape = lexical_form_needs_escape(lex);
    return Literal::make_simple_unchecked(lex, needs_escape, node_storage);
}

Literal Literal::make_lang_tagged(std::string_view lexical_form, std::string_view lang_tag,
                                  storage::DynNodeStoragePtr node_storage) {
    if (!una::is_valid_utf8(lexical_form)) [[unlikely]] {
        throw InvalidNode{"Invalid UTF-8 in lexical form of literal"};
    }

    auto const lowercase_lang_tag = una::cases::to_lowercase_utf8(lang_tag);
    auto const needs_escape = lexical_form_needs_escape(lexical_form);
    return Literal::make_lang_tagged_unchecked(lexical_form, needs_escape, lowercase_lang_tag, node_storage);
}

Literal Literal::make_lang_tagged_normalize(std::string_view lexical_form, std::string_view lang_tag,
                                            storage::DynNodeStoragePtr node_storage) {
    auto const lowercase_lang_tag = una::cases::to_lowercase_utf8(lang_tag);
    auto const lex = una::norm::to_nfc_utf8(lexical_form);
    auto const needs_escape = lexical_form_needs_escape(lex);
    return Literal::make_lang_tagged_unchecked(lex, needs_escape, lowercase_lang_tag, node_storage);
}

Literal Literal::make_typed(std::string_view lexical_form, IRI const &datatype, storage::DynNodeStoragePtr node_storage) {
    using namespace datatypes::registry;

    DatatypeIDView const datatype_identifier{datatype};

    if (datatype_identifier == datatypes::rdf::LangString::datatype_id) {
        // see: https://www.w3.org/TR/rdf11-concepts/#section-Graph-Literal
        throw InvalidNode{"cannot construct rdf:langString without a language tag, please call one of the other factory functions"};
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

Literal Literal::make_boolean(TriBool const b, storage::DynNodeStoragePtr node_storage) noexcept {
    if (b == TriBool::Err) {
        return Literal{};
    }

    return Literal::make_typed_from_value<datatypes::xsd::Boolean>(b == TriBool::True, node_storage);
}

Literal Literal::make_string_uuid(storage::DynNodeStoragePtr node_storage) {
    boost::uuids::random_generator_mt19937 gen{};
    boost::uuids::uuid u = gen();
    std::string s = boost::uuids::to_string(u);
    return make_simple(s, node_storage);
}

Literal Literal::generate_random_double(storage::DynNodeStoragePtr node_storage) {
    static thread_local std::default_random_engine rng{std::random_device{}()};
    return Literal::generate_random_double(rng, node_storage);
}

Literal Literal::to_node_storage(storage::DynNodeStoragePtr node_storage) const {
    using datatypes::registry::DatatypeRegistry;

    node_storage = select_node_storage(node_storage);
    if (handle_.storage() == node_storage || null()) {
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
            [&](storage::view::LexicalFormLiteralBackendView &lexical_backend) {
                if (auto const dt_id = storage::identifier::iri_node_id_to_literal_type(lexical_backend.datatype_id);
                    dt_id.is_fixed() && node_storage.has_specialized_storage_for(dt_id)) {

                    // This node storage doesn't have specialized storage for the given type but the target node storage does.
                    // Need to send value over.
                    // This doesn't work for rdf:langString, but it shouldn't have a specialized storage anyway.
                    assert(!this->datatype_eq<datatypes::rdf::LangString>());

                    auto const from_string = DatatypeRegistry::get_factory(dt_id);
                    auto value = from_string(lexical_backend.lexical_form);

                    return node_storage.find_or_make_id(storage::view::ValueLiteralBackendView{.datatype = dt_id,
                                                                                                     .value = std::move(value)});
                }

                // send over IRI corresponding to this datatype
                auto const dtype_iri_view = handle_.storage().find_iri_backend(lexical_backend.datatype_id);
                lexical_backend.datatype_id = node_storage.find_or_make_id(dtype_iri_view);

                // find or make the requested node
                return node_storage.find_or_make_id(literal_view);
            },
            [&node_storage, &literal_view](storage::view::ValueLiteralBackendView const &value_backend) {
                // no need to send over datatype IRI, as this having a specialized storage requires
                // that the datatype is fixed, so it must already be present

                if (!node_storage.has_specialized_storage_for(value_backend.datatype)) {
                    // target node storage is not specialized for this datatype, need to convert to lexical form
                    auto const serialize = DatatypeRegistry::get_serialize_canonical_string(datatypes::registry::DatatypeIDView{value_backend.datatype});
                    assert(serialize != nullptr);

                    auto const lex = run_serialize(serialize, value_backend.value);

                    return node_storage.find_or_make_id(storage::view::LexicalFormLiteralBackendView{
                            .datatype_id = storage::identifier::literal_type_to_iri_node_id(value_backend.datatype),
                            .lexical_form = lex,
                            .language_tag = "",
                            .needs_escape = false});
                }

                // target node storage is also specialized for this datatype, directly send it over
                return node_storage.find_or_make_id(literal_view);
            });

    return Literal{storage::identifier::NodeBackendHandle{node_id, node_storage}};
}

Literal Literal::try_get_in_node_storage(storage::DynNodeStoragePtr node_storage) const noexcept {
    using datatypes::registry::DatatypeRegistry;

    node_storage = select_node_storage(node_storage);
    if (handle_.storage() == node_storage || null()) {
        return *this;
    }

    if (this->is_inlined()) {
        if (this->datatype_eq<datatypes::rdf::LangString>()) {
            // special case for rdf:langString because part of it is in the backend
            auto const literal_view = this->lang_tagged_get_de_inlined().handle_.literal_backend();

            auto const tmp_id = node_storage.find_id(literal_view);
            if (tmp_id.null()) {
                return Literal{};
            }

            auto const lang_tag_i = DatatypeRegistry::LangTagInlines::try_tag_to_inlined(literal_view.get_lexical().language_tag);
            if (!lang_tag_i.has_value()) {
                return Literal{storage::identifier::NodeBackendHandle{tmp_id, node_storage}};
            }

            auto const inlined_id = DatatypeRegistry::LangTagInlines::try_into_inlined(tmp_id.node_id().literal_id(), *lang_tag_i);
            if (!inlined_id.has_value()) {
                return Literal{storage::identifier::NodeBackendHandle{tmp_id, node_storage}};
            }

            return Literal{storage::identifier::NodeBackendHandle{storage::identifier::NodeBackendID{storage::identifier::NodeID{*inlined_id, tmp_id.node_id().literal_type()},
                                                                                                     storage::identifier::RDFNodeType::Literal,
                                                                                                     true,
                                                                                                     tmp_id.free_tagging_bits()},
                                                                  node_storage}};
        }

        auto const node_id = this->handle_.node_id();
        return Literal::make_inlined_typed_unchecked(node_id.literal_id(),
                                                     node_id.literal_type(),
                                                     node_storage);
    }

    auto literal_view = handle_.literal_backend();
    auto const node_id = literal_view.visit(
            [&](storage::view::LexicalFormLiteralBackendView &lexical_backend) noexcept {
                if (auto const dt_id = storage::identifier::iri_node_id_to_literal_type(lexical_backend.datatype_id);
                    dt_id.is_fixed() && node_storage.has_specialized_storage_for(dt_id)) {

                    // This node storage doesn't have specialized storage for the given type but the target node storage does.
                    // Need to send value over.
                    // This doesn't work for rdf:langString, but it shouldn't have a specialized storage anyway.
                    assert(!this->datatype_eq<datatypes::rdf::LangString>());

                    auto const from_string = DatatypeRegistry::get_factory(dt_id);
                    auto value = from_string(lexical_backend.lexical_form);

                    return node_storage.find_id(storage::view::ValueLiteralBackendView{.datatype = dt_id,
                                                                                         .value = std::move(value)});
                }

                // Default case.
                // This node storage doesn't have specialized storage for the value and the new one also doesn't
                auto src_node_storage = handle_.storage();
                auto const dtype_iri_view = src_node_storage.find_iri_backend(lexical_backend.datatype_id);

                lexical_backend.datatype_id = node_storage.find_id(dtype_iri_view);
                if (lexical_backend.datatype_id.null()) {
                    // datatype IRI not present, therefore literal cannot be present
                    return storage::identifier::NodeBackendID{};
                }

                return node_storage.find_id(literal_view);
            },
            [&node_storage, &literal_view](storage::view::ValueLiteralBackendView const &value_backend) noexcept {
                // no need to send over datatype IRI, as this having a specialized storage requires
                // that the datatype is fixed, so it must already be present

                if (!node_storage.has_specialized_storage_for(value_backend.datatype)) {
                    // target node storage is not specialized for this datatype, need to convert to lexical form
                    auto const serialize_canonical = DatatypeRegistry::get_serialize_canonical_string(datatypes::registry::DatatypeIDView{value_backend.datatype});
                    assert(serialize_canonical != nullptr);

                    auto const lex = run_serialize(serialize_canonical, value_backend.value);

                    return node_storage.find_id(storage::view::LexicalFormLiteralBackendView{
                            .datatype_id = storage::identifier::literal_type_to_iri_node_id(value_backend.datatype),
                            .lexical_form = lex,
                            .language_tag = "",
                            .needs_escape = false});
                }

                // target node storage is also specialized for this datatype, directly try to get it
                return node_storage.find_id(literal_view);
            });

    if (node_id.null()) {
        return Literal{};
    }

    return Literal{storage::identifier::NodeBackendHandle{node_id, node_storage}};
}

storage::identifier::NodeBackendID Literal::find_datatype_iri(datatypes::registry::DatatypeIDView id, storage::DynNodeStoragePtr node_storage) noexcept {
    auto nid = IRI::find(id, node_storage);
    return nid.backend_handle().id();
}

Literal Literal::find_simple(std::string_view lexical_form, storage::DynNodeStoragePtr node_storage) noexcept {
    auto esc = lexical_form_needs_escape(lexical_form);
    auto nid = node_storage.find_id(storage::view::LexicalFormLiteralBackendView{
            storage::identifier::NodeBackendID::xsd_string_iri.first, lexical_form, "", esc});
    if (nid.null())
        return Literal{};
    return Literal{storage::identifier::NodeBackendHandle{nid, node_storage}};
}

Literal Literal::find_lang_tagged(std::string_view lexical_form, std::string_view lang_tag, storage::DynNodeStoragePtr node_storage) noexcept {
    auto esc = lexical_form_needs_escape(lexical_form);
    auto nid = node_storage.find_id(storage::view::LexicalFormLiteralBackendView{
            storage::identifier::NodeBackendID::rdf_langstring_iri.first, lexical_form, lang_tag, esc});
    if (nid.null())
        return Literal{};
    return make_lang_tagged_unchecked_from_node_id(lang_tag, node_storage, nid);
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

Literal Literal::as_datatype_eq(IRI const &datatype, storage::DynNodeStoragePtr node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    return Literal::make_boolean(this->datatype_eq(datatype), select_node_storage(node_storage));
}

Literal Literal::as_datatype_eq(Literal const &other, storage::DynNodeStoragePtr node_storage) const noexcept {
    if (this->null() || other.null()) {
        return Literal{};
    }

    return Literal::make_boolean(this->datatype_eq(other), select_node_storage(node_storage));
}

IRI Literal::datatype() const noexcept {
    if (this->is_fixed()) {
        return IRI{storage::identifier::datatype_iri_handle_for_fixed_lit_handle(handle_)};
    }

    auto const lexical = handle_.literal_backend().get_lexical();
    return IRI{storage::identifier::NodeBackendHandle{lexical.datatype_id,
                                                      handle_.storage()}};
}

template<bool simplified, typename C>
auto Literal::serialize_lexical_form_impl(C &&consume) const noexcept {
    if (this->is_inlined()) {
        if (this->datatype_eq<datatypes::rdf::LangString>()) {
            auto const lex = this->lang_tagged_get_de_inlined()
                                     .handle_
                                     .literal_backend()
                                     .get_lexical()
                                     .lexical_form;

            return std::invoke(std::forward<C>(consume), lex);
        }

        auto const *entry = datatypes::registry::DatatypeRegistry::get_entry(this->datatype_id());
        assert(entry != nullptr);
        assert(entry->inlining_ops.has_value());

        auto const inlined_value = this->handle_.node_id().literal_id();
        auto const value = entry->inlining_ops->from_inlined_fptr(inlined_value);

        if constexpr (simplified) {
            return std::invoke(std::forward<C>(consume), value, entry->serialize_simplified_string_fptr);
        } else {
            return std::invoke(std::forward<C>(consume), value, entry->serialize_canonical_string_fptr);
        }
    }

    return handle_.literal_backend().visit(
            [this, &consume](storage::view::LexicalFormLiteralBackendView const &lexical_backend) noexcept {
                if constexpr (simplified) {
                    auto const *entry = datatypes::registry::DatatypeRegistry::get_entry(this->datatype_id());

                    if (entry != nullptr) {
                        auto const value = entry->factory_fptr(lexical_backend.lexical_form);
                        return std::invoke(std::forward<C>(consume), value, entry->serialize_simplified_string_fptr);
                    }
                } else {
                    (void) this; // silence unused capture warning
                }

                return std::invoke(std::forward<C>(consume), lexical_backend.lexical_form);
            },
            [&consume](storage::view::ValueLiteralBackendView const &value_backend) noexcept {
                if constexpr (simplified) {
                    auto const serialize = datatypes::registry::DatatypeRegistry::get_serialize_simplified_string(value_backend.datatype);
                    return std::invoke(std::forward<C>(consume), value_backend.value, serialize);
                } else {
                    auto const serialize = datatypes::registry::DatatypeRegistry::get_serialize_canonical_string(value_backend.datatype);
                    return std::invoke(std::forward<C>(consume), value_backend.value, serialize);
                }
            });
}


// Using structs to ensure static-dispatch
struct ConsumeSafe {
    [[nodiscard]] CowString operator()(std::string_view lexical) const noexcept {
        return CowString{CowString::borrowed, lexical};
    }

    [[nodiscard]] CowString operator()(std::any const &value, datatypes::registry::DatatypeRegistry::serialize_fptr_t serialize) const noexcept {
        auto s = writer::StringWriter::oneshot([&value, serialize](writer::StringWriter &w) noexcept {
            return serialize(value, w);
        });

        return CowString{CowString::owned, std::move(s)};
    }
};

struct ConsumeMaybeSerialize {
    std::string_view *const out_lex_form;
    writer::BufWriterParts const writer;

    [[nodiscard]] FetchOrSerializeResult operator()(std::string_view const lexical) const noexcept {
        *out_lex_form = lexical;
        return FetchOrSerializeResult::Fetched;
    }

    [[nodiscard]] FetchOrSerializeResult operator()(std::any const &value, datatypes::registry::DatatypeRegistry::serialize_fptr_t const serialize) const noexcept {
        if (!serialize(value, writer)) {
            return FetchOrSerializeResult::SerializationFailed;
        }
        return FetchOrSerializeResult::Serialized;
    }
};

struct ConsumeSerialize {
    writer::BufWriterParts const writer;

    bool operator()(std::string_view const lexical_form) const noexcept {
        return writer::write_str(lexical_form, writer);
    }

    bool operator()(std::any const &value, datatypes::registry::DatatypeRegistry::serialize_fptr_t const serialize) const noexcept {
        return serialize(value, writer);
    }
};

CowString Literal::lexical_form() const noexcept {
    return serialize_lexical_form_impl<false>(ConsumeSafe{});
}

FetchOrSerializeResult Literal::fetch_or_serialize_lexical_form(std::string_view &out_lex_form, writer::BufWriterParts const writer) const noexcept {
    return serialize_lexical_form_impl<false>(ConsumeMaybeSerialize{&out_lex_form, writer});
}

Literal Literal::as_lexical_form(storage::DynNodeStoragePtr node_storage) const {
    if (this->null()) {
        return Literal{};
    }

    auto const lex = this->lexical_form();
    auto const needs_escape = lexical_form_needs_escape(lex);
    return Literal::make_simple_unchecked(lex, needs_escape, select_node_storage(node_storage));
}

CowString Literal::simplified_lexical_form() const noexcept {
    return serialize_lexical_form_impl<true>(ConsumeSafe{});
}

FetchOrSerializeResult Literal::fetch_or_serialize_simplified_lexical_form(std::string_view &out_lex_form, writer::BufWriterParts const writer) const noexcept {
    return serialize_lexical_form_impl<true>(ConsumeMaybeSerialize{&out_lex_form, writer});
}

Literal Literal::as_simplified_lexical_form(storage::DynNodeStoragePtr node_storage) const {
    if (this->null()) {
        return Literal{};
    }

    auto const lex = this->simplified_lexical_form();
    auto const needs_escape = lexical_form_needs_escape(lex);
    return Literal::make_simple_unchecked(lex, needs_escape, select_node_storage(node_storage));
}

std::string_view Literal::language_tag() const noexcept {
    if (this->datatype_eq<datatypes::rdf::LangString>()) {
        if (this->is_inlined()) {
            auto [tag, _] = rdf4cpp::datatypes::registry::DatatypeRegistry::LangTagInlines::from_inlined(this->handle_.node_id().literal_id());
            return rdf4cpp::datatypes::registry::DatatypeRegistry::LangTagInlines::inlined_to_tag(tag);
        }
        return handle_.literal_backend().get_lexical().language_tag;
    }

    return "";
}

Literal Literal::as_language_tag(storage::DynNodeStoragePtr node_storage) const {
    if (this->null()) {
        return Literal{};
    }

    return Literal::make_simple_unchecked(this->language_tag(), false, select_node_storage(node_storage));
}

TriBool Literal::language_tag_eq(std::string_view const lang_tag) const noexcept {
    if (!this->datatype_eq<datatypes::rdf::LangString>()) {
        return TriBool::Err;
    }

    return this->language_tag() == lang_tag;
}

TriBool Literal::language_tag_eq(Literal const &other) const noexcept {
    if (!this->datatype_eq<datatypes::rdf::LangString>() || !other.datatype_eq<datatypes::rdf::LangString>()) {
        return TriBool::Err;
    }

    return this->language_tag() == other.language_tag();
}

Literal Literal::as_language_tag_eq(std::string_view const lang_tag, storage::DynNodeStoragePtr node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    return Literal::make_boolean(this->language_tag_eq(lang_tag), select_node_storage(node_storage));
}

Literal Literal::as_language_tag_eq(Literal const &other, storage::DynNodeStoragePtr node_storage) const noexcept {
    if (this->null() || other.null()) {
        return Literal{};
    }

    return Literal::make_boolean(this->language_tag_eq(other), select_node_storage(node_storage));
}

// https://www.w3.org/TR/n-triples/#grammar-production-STRING_LITERAL_QUOTE
#define RDF4CPP_DETAIL_TRY_SER_QUOTED_LEXICAL(lexical) \
    RDF4CPP_DETAIL_TRY_WRITE_STR("\"");                \
    for (char const ch : lexical) {                    \
        switch (ch) {                                  \
            case '"': {                                \
                RDF4CPP_DETAIL_TRY_WRITE_STR(R"(\")"); \
                break;                                 \
            }                                          \
            case '\\': {                               \
                RDF4CPP_DETAIL_TRY_WRITE_STR(R"(\\)"); \
                break;                                 \
            }                                          \
            case '\n': {                               \
                RDF4CPP_DETAIL_TRY_WRITE_STR(R"(\n)"); \
                break;                                 \
            }                                          \
            case '\r': {                               \
                RDF4CPP_DETAIL_TRY_WRITE_STR(R"(\r)"); \
                break;                                 \
            }                                          \
            [[likely]] default: {                      \
                RDF4CPP_DETAIL_TRY_WRITE_STR(&ch, 1);  \
                break;                                 \
            }                                          \
        }                                              \
    }                                                  \
    RDF4CPP_DETAIL_TRY_WRITE_STR("\"");


bool Literal::serialize(writer::BufWriterParts const writer, NodeSerializationOpts opts) const noexcept {
    if (this->null()) {
        RDF4CPP_DETAIL_TRY_WRITE_STR("null");
        return true;
    }

    if (this->datatype_eq<datatypes::xsd::String>()) {
        auto const value = this->backend_handle().literal_backend().get_lexical();

        if (value.needs_escape) [[unlikely]] {
            RDF4CPP_DETAIL_TRY_SER_QUOTED_LEXICAL(value.lexical_form);
        } else {
            RDF4CPP_DETAIL_TRY_WRITE_STR("\"");
            RDF4CPP_DETAIL_TRY_WRITE_STR(value.lexical_form);
            RDF4CPP_DETAIL_TRY_WRITE_STR("\"");
        }

        return true;
    } else if (this->datatype_eq<datatypes::rdf::LangString>()) {
        auto const value = this->lang_tagged_get_de_inlined().backend_handle().literal_backend().get_lexical();

        if (value.needs_escape) [[unlikely]] {
            RDF4CPP_DETAIL_TRY_SER_QUOTED_LEXICAL(value.lexical_form);
        } else {
            RDF4CPP_DETAIL_TRY_WRITE_STR("\"");
            RDF4CPP_DETAIL_TRY_WRITE_STR(value.lexical_form);
            RDF4CPP_DETAIL_TRY_WRITE_STR("\"");
        }

        RDF4CPP_DETAIL_TRY_WRITE_STR("@");
        RDF4CPP_DETAIL_TRY_WRITE_STR(value.language_tag);
        return true;
    } else if (opts.use_short_form && (this->datatype_eq<datatypes::xsd::Integer>() || this->datatype_eq<datatypes::xsd::Double>()
            || this->datatype_eq<datatypes::xsd::Decimal>() || this->datatype_eq<datatypes::xsd::Boolean>())) {

        return this->serialize_lexical_form(writer);
    } else if (this->is_inlined()) {
        assert(!this->datatype_eq<datatypes::rdf::LangString>());
        // Notes:
        // 1. inlined values are assumed to not require escaping
        // 2. This is a known datatype because it is inlined => the registry contains the datatype IRI

        auto const *entry = datatypes::registry::DatatypeRegistry::get_entry(this->datatype_id());
        assert(entry != nullptr);
        assert(entry->inlining_ops.has_value());

        RDF4CPP_DETAIL_TRY_WRITE_STR("\"");

        auto const inlined_value = this->backend_handle().node_id().literal_id();
        if (!entry->serialize_canonical_string_fptr(entry->inlining_ops->from_inlined_fptr(inlined_value), writer)) {
            return false;
        }

        RDF4CPP_DETAIL_TRY_WRITE_STR("\"^^");
        return writer::write_fixed_type_iri(this->backend_handle().node_id().literal_type(), writer, opts.use_prefixes);
    } else {
        using storage::NodeStorage;
        using storage::identifier::NodeBackendHandle;

        return this->backend_handle().literal_backend().visit(
                [&](storage::view::LexicalFormLiteralBackendView const &lexical_backend) noexcept {
                    auto const dtype_iri = handle_.storage().find_iri_backend(lexical_backend.datatype_id);

                    if (lexical_backend.needs_escape) [[unlikely]] {
                        RDF4CPP_DETAIL_TRY_SER_QUOTED_LEXICAL(lexical_backend.lexical_form);
                    } else {
                        RDF4CPP_DETAIL_TRY_WRITE_STR("\"");
                        RDF4CPP_DETAIL_TRY_WRITE_STR(lexical_backend.lexical_form);
                        RDF4CPP_DETAIL_TRY_WRITE_STR("\"");
                    }

                    RDF4CPP_DETAIL_TRY_WRITE_STR("^^<");
                    RDF4CPP_DETAIL_TRY_WRITE_STR(dtype_iri.identifier);
                    RDF4CPP_DETAIL_TRY_WRITE_STR(">");
                    return true;
                },
                [&](storage::view::ValueLiteralBackendView const &value_backend) noexcept {
                    // Notes:
                    // 1. non-string storage values are assumed to not require escaping
                    // 2. This is a known datatype because it is stored in a value backend => the registry contains the datatype IRI

                    auto const *entry = datatypes::registry::DatatypeRegistry::get_entry(this->datatype_id());
                    assert(entry != nullptr);

                    RDF4CPP_DETAIL_TRY_WRITE_STR("\"");

                    if (!entry->serialize_canonical_string_fptr(value_backend.value, writer)) {
                        return false;
                    }

                    RDF4CPP_DETAIL_TRY_WRITE_STR("\"^^");
                    return writer::write_fixed_type_iri(this->backend_handle().node_id().literal_type(), writer, opts.use_prefixes);
                });
    }
}

#undef RDF4CPP_DETAIL_TRY_SER_QUOTED_LEXICAL

bool Literal::serialize_lexical_form(writer::BufWriterParts const writer) const noexcept {
    return serialize_lexical_form_impl<false>(ConsumeSerialize{writer});
}

bool Literal::serialize_simplified_lexical_form(writer::BufWriterParts const writer) const noexcept {
    return serialize_lexical_form_impl<true>(ConsumeSerialize{writer});
}

Literal::operator std::string() const noexcept {
    return writer::StringWriter::oneshot([this](auto &w) noexcept {
        return this->serialize(w);
    });
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

std::ostream &operator<<(std::ostream &os, Literal const &literal) {
    writer::BufOStreamWriter w{os};
    literal.serialize(w);
    w.finalize();

    return os;
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
            [&datatype](storage::view::LexicalFormLiteralBackendView const &lexical_backend) noexcept {
                if (auto const factory = registry::DatatypeRegistry::get_factory(datatype); factory != nullptr) {
                    return factory(lexical_backend.lexical_form);
                }

                return std::any{};
            },
            [&datatype](storage::view::ValueLiteralBackendView const &value_backend) noexcept {
                assert(value_backend.datatype == datatype);
                (void)datatype;

                return value_backend.value;
            });
}

Literal Literal::cast(IRI const &target, storage::DynNodeStoragePtr node_storage) const {
    using namespace datatypes::registry;
    using namespace datatypes::xsd;

    if (this->null()) {
        return Literal{};
    }

    node_storage = select_node_storage(node_storage);

    auto const this_dtid = this->datatype_id();
    DatatypeIDView const target_dtid{target};

    if (this_dtid == target_dtid) {
        return this->to_node_storage(node_storage);
    }

    if (this_dtid == String::datatype_id) {
        // string -> any
        try {
            auto const lex = this->lexical_form();
            return Literal::make_typed(lex, target, node_storage);
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
Literal Literal::numeric_binop_impl(OpSelect op_select, Literal const &other, storage::DynNodeStoragePtr node_storage) const {
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
Literal Literal::numeric_unop_impl(OpSelect op_select, storage::DynNodeStoragePtr node_storage) const {
    using namespace datatypes::registry;

    if (this->null()) {
        return Literal{};
    }

    node_storage = select_node_storage(node_storage);

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

    if (this->handle_.null() || other.handle_.null()) {
        if (out_alternative_ordering != nullptr) {
            // ordering extensions (for e.g. ORDER BY) require that null nodes
            // are always the smallest node
            if (this->handle_.null() && other.handle_.null()) {
                *out_alternative_ordering = std::strong_ordering::equivalent;
            } else {
                *out_alternative_ordering = this->handle_.null()
                                                ? std::strong_ordering::less
                                                : std::strong_ordering::greater;
            }
        }

        // "Apart from BOUND, COALESCE, NOT EXISTS and EXISTS, all functions and operators operate on RDF Terms and will produce a type error if any arguments are unbound."
        // - https://www.w3.org/TR/sparql11-query/#evaluation
        return std::partial_ordering::unordered;
    }

    if (this->handle_ == other.handle_) {
        return std::partial_ordering::equivalent;
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

std::weak_ordering Literal::order(Literal const &other) const noexcept {
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

TriBool Literal::eq(Literal const &other) const noexcept {
    return util::partial_weak_ordering_eq(this->compare(other), std::weak_ordering::equivalent);
}

Literal Literal::as_eq(Literal const &other, storage::DynNodeStoragePtr node_storage) const noexcept {
    return Literal::make_boolean(this->eq(other), select_node_storage(node_storage));
}

TriBool Literal::ne(Literal const &other) const noexcept {
    return !util::partial_weak_ordering_eq(this->compare(other), std::weak_ordering::equivalent);
}

Literal Literal::as_ne(Literal const &other, storage::DynNodeStoragePtr node_storage) const noexcept {
    return Literal::make_boolean(this->ne(other), select_node_storage(node_storage));
}

TriBool Literal::lt(Literal const &other) const noexcept {
    return util::partial_weak_ordering_eq(this->compare(other), std::weak_ordering::less);
}

Literal Literal::as_lt(Literal const &other, storage::DynNodeStoragePtr node_storage) const noexcept {
    return Literal::make_boolean(this->lt(other), select_node_storage(node_storage));
}

TriBool Literal::le(Literal const &other) const noexcept {
    return !util::partial_weak_ordering_eq(this->compare(other), std::weak_ordering::greater);
}

Literal Literal::as_le(Literal const &other, storage::DynNodeStoragePtr node_storage) const noexcept {
    return Literal::make_boolean(this->le(other), select_node_storage(node_storage));
}

TriBool Literal::gt(Literal const &other) const noexcept {
    return util::partial_weak_ordering_eq(this->compare(other), std::weak_ordering::greater);
}

Literal Literal::as_gt(Literal const &other, storage::DynNodeStoragePtr node_storage) const noexcept {
    return Literal::make_boolean(this->gt(other), select_node_storage(node_storage));
}

TriBool Literal::ge(Literal const &other) const noexcept {
    return !util::partial_weak_ordering_eq(this->compare(other), std::weak_ordering::less);
}

Literal Literal::as_ge(Literal const &other, storage::DynNodeStoragePtr node_storage) const noexcept {
    return Literal::make_boolean(this->ge(other), select_node_storage(node_storage));
}

bool Literal::order_eq(Literal const &other) const noexcept {
    return order(other) == std::weak_ordering::equivalent;
}

Literal Literal::as_order_eq(Literal const &other, storage::DynNodeStoragePtr node_storage) const noexcept {
    return Literal::make_boolean(this->order_eq(other), select_node_storage(node_storage));
}

bool Literal::order_ne(Literal const &other) const noexcept {
    return order(other) != std::weak_ordering::equivalent;
}

Literal Literal::as_order_ne(Literal const &other, storage::DynNodeStoragePtr node_storage) const noexcept {
    return Literal::make_boolean(this->order_ne(other), select_node_storage(node_storage));
}

bool Literal::order_lt(Literal const &other) const noexcept {
    return order(other) == std::weak_ordering::less;
}

Literal Literal::as_order_lt(Literal const &other, storage::DynNodeStoragePtr node_storage) const noexcept {
    return Literal::make_boolean(this->order_lt(other), select_node_storage(node_storage));
}

bool Literal::order_le(Literal const &other) const noexcept {
    return order(other) != std::weak_ordering::greater;
}

Literal Literal::as_order_le(Literal const &other, storage::DynNodeStoragePtr node_storage) const noexcept {
    return Literal::make_boolean(this->order_le(other), select_node_storage(node_storage));
}

bool Literal::order_gt(Literal const &other) const noexcept {
    return order(other) == std::weak_ordering::greater;
}

Literal Literal::as_order_gt(Literal const &other, storage::DynNodeStoragePtr node_storage) const noexcept {
    return Literal::make_boolean(this->order_gt(other), select_node_storage(node_storage));
}

bool Literal::order_ge(Literal const &other) const noexcept {
    return order(other) != std::weak_ordering::less;
}

Literal Literal::as_order_ge(Literal const &other, storage::DynNodeStoragePtr node_storage) const noexcept {
    return Literal::make_boolean(this->order_ge(other), select_node_storage(node_storage));
}

std::partial_ordering Literal::operator<=>(Literal const &other) const noexcept {
    return compare(other);
}

bool Literal::operator==(Literal const &other) const noexcept {
    return this->eq(other);
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

Literal Literal::add(Literal const &other, storage::DynNodeStoragePtr node_storage) const {
    node_storage = select_node_storage(node_storage);

    // DayTimeDuration + DayTimeDuration
    {
        auto this_v = this->cast_to_supertype_value<datatypes::xsd::DayTimeDuration>();
        auto other_v = other.cast_to_supertype_value<datatypes::xsd::DayTimeDuration>();
        if (this_v.has_value() && other_v.has_value()) {
            auto r = datatypes::registry::util::to_checked(*this_v) + datatypes::registry::util::to_checked(*other_v);
            if (r.count().is_invalid())
                return Literal{};

            return make_typed_from_value<datatypes::xsd::DayTimeDuration>(datatypes::registry::util::from_checked(r), node_storage);
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

            return make_typed_from_value<datatypes::xsd::YearMonthDuration>(datatypes::registry::util::from_checked(r), node_storage);
        }
    }
    // DateTime(&Subclasses) + Duration(&Subclasses)
    {
        try
        {
            auto this_dt = this->cast_to_supertype_value<datatypes::xsd::DateTime>();
            auto other_dt = other.cast_to_supertype_value<datatypes::xsd::Duration>();
            if (this_dt.has_value() && other_dt.has_value()) {
                if (this->datatype_eq<datatypes::xsd::Time>() && other.datatype_eq<datatypes::xsd::YearMonthDuration>()) {
                    return Literal{};
                }

                auto tp = datatypes::registry::util::add_duration_to_date_time(this_dt->first, *other_dt);

                if (this->datatype_eq<datatypes::xsd::Date>()) {
                    auto [date, _] = util::deconstruct_timepoint(tp);
                    return make_typed_from_value<datatypes::xsd::Date>(std::make_pair(date, this_dt->second), node_storage);
                }

                if (this->datatype_eq<datatypes::xsd::Time>()) {
                    auto [_, time] = util::deconstruct_timepoint(tp);
                    return make_typed_from_value<datatypes::xsd::Time>(std::make_pair(std::chrono::duration_cast<std::chrono::nanoseconds>(time), this_dt->second), node_storage);
                }

                return make_typed_from_value<datatypes::xsd::DateTime>(std::make_pair(tp, this_dt->second), node_storage);
            }
        } catch (std::overflow_error const &) {
            return Literal{};
        }
    }

    return this->numeric_binop_impl(
            [](auto const &num_ops) noexcept {
                return num_ops.add_fptr;
            },
            other, node_storage);
}

Literal Literal::operator+(Literal const &other) const {
    return this->add(other);
}

Literal &Literal::add_assign(const Literal &other, storage::DynNodeStoragePtr node_storage) {
    auto result = this->add(other, node_storage);
    this->handle_ = result.handle_;
    return *this;
}

Literal &Literal::operator+=(const Literal &other) {
    return this->add_assign(other);
}

Literal Literal::sub(Literal const &other, storage::DynNodeStoragePtr node_storage) const {
    node_storage = select_node_storage(node_storage);

    // DateTime(&Subclasses) - DateTime(&Subclasses)
    if (this->datatype_id() == other.datatype_id()) {
        try {
            auto this_dt = this->cast_to_supertype_value<datatypes::xsd::DateTime>();
            auto other_dt = other.cast_to_supertype_value<datatypes::xsd::DateTime>();
            if (this_dt.has_value() && other_dt.has_value()) {
                ZonedTime const this_tp{this_dt->second.has_value() ? *this_dt->second : Timezone{},
                                                                          this_dt->first};

                ZonedTime const other_tp{other_dt->second.has_value() ? *other_dt->second : Timezone{},
                                                                           other_dt->first};

                auto d = this_tp.get_sys_time() - other_tp.get_sys_time();
                return make_typed_from_value<datatypes::xsd::DayTimeDuration>(std::chrono::duration_cast<std::chrono::nanoseconds>(d), node_storage);
            }
        }
        catch (std::overflow_error const &) {
            return Literal{};
        }
    }
    // DateTime(&Subclasses) - Duration(&Subclasses)
    {
        try {
            auto this_dt = this->cast_to_supertype_value<datatypes::xsd::DateTime>();
            auto other_dt = other.cast_to_supertype_value<datatypes::xsd::Duration>();
            if (this_dt.has_value() && other_dt.has_value()) {
                auto tp = datatypes::registry::util::add_duration_to_date_time(this_dt->first, std::make_pair(-other_dt->first, -other_dt->second));

                if (this->datatype_eq<datatypes::xsd::Date>()) {
                    auto [date, _] = util::deconstruct_timepoint(tp);
                    return make_typed_from_value<datatypes::xsd::Date>(std::make_pair(date, this_dt->second), node_storage);
                }

                if (this->datatype_eq<datatypes::xsd::Time>()) {
                    auto [_, time] = util::deconstruct_timepoint(tp);
                    return make_typed_from_value<datatypes::xsd::Time>(std::make_pair(std::chrono::duration_cast<std::chrono::nanoseconds>(time), this_dt->second), node_storage);
                }

                return make_typed_from_value<datatypes::xsd::DateTime>(std::make_pair(tp, this_dt->second), node_storage);
            }
        }
        catch (std::overflow_error const &) {
            return Literal{};
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

            return make_typed_from_value<datatypes::xsd::DayTimeDuration>(datatypes::registry::util::from_checked(r), node_storage);
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

            return make_typed_from_value<datatypes::xsd::YearMonthDuration>(datatypes::registry::util::from_checked(r), node_storage);
        }
    }

    return this->numeric_binop_impl(
            [](auto const &num_ops) noexcept {
                return num_ops.sub_fptr;
            },
            other, node_storage);
}

Literal Literal::operator-(Literal const &other) const {
    return this->sub(other);
}

Literal &Literal::sub_assign(const Literal &other, storage::DynNodeStoragePtr node_storage) {
    auto result = this->sub(other, node_storage);
    this->handle_ = result.handle_;
    return *this;
}

Literal &Literal::operator-=(const Literal &other) {
    return this->sub_assign(other);
}

Literal Literal::mul(Literal const &other, storage::DynNodeStoragePtr node_storage) const {
    node_storage = select_node_storage(node_storage);

    // YearMonthDuration * double
    {
        auto this_v = this->cast_to_supertype_value<datatypes::xsd::YearMonthDuration>();
        auto other_v = other.cast_to_supertype_value<datatypes::xsd::Double>();
        if (this_v.has_value() && other_v.has_value()) {
            auto r = std::round(static_cast<double>(this_v->count()) * *other_v);
            if (!datatypes::registry::util::fits_into<int64_t>(r))
                return Literal{};

            return make_typed_from_value<datatypes::xsd::YearMonthDuration>(std::chrono::months{static_cast<int64_t>(r)}, node_storage);
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

            return make_typed_from_value<datatypes::xsd::DayTimeDuration>(std::chrono::nanoseconds{static_cast<int64_t>(r)}, node_storage);
        }
    }

    return this->numeric_binop_impl(
            [](auto const &num_ops) noexcept {
                return num_ops.mul_fptr;
            },
            other, node_storage);
}

Literal Literal::operator*(Literal const &other) const {
    return this->mul(other);
}

Literal &Literal::mul_assign(const Literal &other, storage::DynNodeStoragePtr node_storage) {
    auto result = this->mul(other, node_storage);
    this->handle_ = result.handle_;
    return *this;
}

Literal &Literal::operator*=(const Literal &other) {
    return this->mul_assign(other);
}

Literal Literal::div(Literal const &other, storage::DynNodeStoragePtr node_storage) const {
    node_storage = select_node_storage(node_storage);

    // YearMonthDuration / double
    {
        auto this_v = this->cast_to_supertype_value<datatypes::xsd::YearMonthDuration>();
        auto other_v = other.cast_to_supertype_value<datatypes::xsd::Double>();
        if (this_v.has_value() && other_v.has_value()) {
            auto r = std::round(static_cast<double>(this_v->count()) / *other_v);
            if (!datatypes::registry::util::fits_into<int64_t>(r))
                return Literal{};

            return make_typed_from_value<datatypes::xsd::YearMonthDuration>(std::chrono::months{static_cast<int64_t>(r)}, node_storage);
        }
    }
    // DayTimeDuration / double
    {
        auto this_v = this->cast_to_supertype_value<datatypes::xsd::DayTimeDuration>();
        auto other_v = other.cast_to_supertype_value<datatypes::xsd::Double>();
        if (this_v.has_value() && other_v.has_value()) {
            auto r = std::round(static_cast<double>(this_v->count()) / *other_v);
            if (!datatypes::registry::util::fits_into<int64_t>(r))
                return Literal{};

            return make_typed_from_value<datatypes::xsd::DayTimeDuration>(std::chrono::nanoseconds{static_cast<int64_t>(r)}, node_storage);
        }
    }
    // YearMonthDuration / YearMonthDuration
    {
        auto this_v = this->cast_to_supertype_value<datatypes::xsd::YearMonthDuration>();
        auto other_v = other.cast_to_supertype_value<datatypes::xsd::YearMonthDuration>();
        if (this_v.has_value() && other_v.has_value()) {
            auto r = BigDecimal<>{this_v->count(), 0}.div_checked(BigDecimal<>{other_v->count(), 0}, 1000);
            if (!r.has_value())
                return Literal{};

            return make_typed_from_value<datatypes::xsd::Decimal>(*r, node_storage);
        }
    }
    // DayTimeDuration / DayTimeDuration
    {
        auto this_v = this->cast_to_supertype_value<datatypes::xsd::DayTimeDuration>();
        auto other_v = other.cast_to_supertype_value<datatypes::xsd::DayTimeDuration>();
        if (this_v.has_value() && other_v.has_value()) {
            auto r = BigDecimal<>{this_v->count(), 0}.div_checked(BigDecimal<>{other_v->count(), 0}, 1000);
            if (!r.has_value())
                return Literal{};

            return make_typed_from_value<datatypes::xsd::Decimal>(*r, node_storage);
        }
    }

    return this->numeric_binop_impl(
            [](auto const &num_ops) noexcept {
                return num_ops.div_fptr;
            },
            other, node_storage);
}

Literal Literal::operator/(Literal const &other) const {
    return this->div(other);
}

Literal &Literal::div_assign(const Literal &other, storage::DynNodeStoragePtr node_storage) {
    auto result = this->div(other, node_storage);
    this->handle_ = result.handle_;
    return *this;
}

Literal &Literal::operator/=(const Literal &other) {
    return this->div_assign(other);
}

Literal Literal::pos(storage::DynNodeStoragePtr node_storage) const {
    return this->numeric_unop_impl(
            [](auto const &num_ops) noexcept {
                return num_ops.pos_fptr;
            },
            select_node_storage(node_storage));
}

Literal Literal::operator+() const {
    return this->pos();
}

Literal Literal::neg(storage::DynNodeStoragePtr node_storage) const {
    return this->numeric_unop_impl(
            [](auto const &num_ops) noexcept {
                return num_ops.neg_fptr;
            },
            select_node_storage(node_storage));
}

Literal Literal::operator-() const {
    return this->neg();
}

Literal Literal::abs(storage::DynNodeStoragePtr node_storage) const {
    return this->numeric_unop_impl(
            [](auto const &num_ops) noexcept {
                return num_ops.abs_fptr;
            },
            select_node_storage(node_storage));
}

Literal Literal::round(storage::DynNodeStoragePtr node_storage) const {
    return this->numeric_unop_impl(
            [](auto const &num_ops) noexcept {
                return num_ops.round_fptr;
            },
            select_node_storage(node_storage));
}

Literal Literal::floor(storage::DynNodeStoragePtr node_storage) const {
    return this->numeric_unop_impl(
            [](auto const &num_ops) noexcept {
                return num_ops.floor_fptr;
            },
            select_node_storage(node_storage));
}

Literal Literal::ceil(storage::DynNodeStoragePtr node_storage) const {
    return this->numeric_unop_impl(
            [](auto const &num_ops) noexcept {
                return num_ops.ceil_fptr;
            },
            select_node_storage(node_storage));
}

TriBool Literal::ebv() const noexcept {
    if (this->null()) {
        return TriBool::Err;
    }

    auto const ebv = datatypes::registry::DatatypeRegistry::get_ebv(this->datatype_id());

    if (ebv == nullptr) {
        return TriBool::Err;
    }

    return ebv(this->value());
}

Literal Literal::as_ebv(storage::DynNodeStoragePtr node_storage) const noexcept {
    return Literal::make_boolean(this->ebv(), select_node_storage(node_storage));
}

Literal Literal::logical_and(Literal const &other, storage::DynNodeStoragePtr node_storage) const noexcept {
    return Literal::make_boolean(this->ebv() && other.ebv(), select_node_storage(node_storage));;
}

Literal Literal::operator&&(Literal const &other) const noexcept {
    return this->logical_and(other);
}

Literal Literal::logical_or(Literal const &other, storage::DynNodeStoragePtr node_storage) const noexcept {
    return Literal::make_boolean(this->ebv() || other.ebv(), select_node_storage(node_storage));
}

Literal Literal::operator||(Literal const &other) const noexcept {
    return this->logical_or(other);
}

Literal Literal::logical_not(storage::DynNodeStoragePtr node_storage) const noexcept {
    return Literal::make_boolean(!this->ebv(), select_node_storage(node_storage));
}

Literal Literal::operator!() const noexcept {
    return this->logical_not();
}

std::optional<size_t> Literal::strlen() const noexcept {
    if (!this->is_string_like()) {
        return std::nullopt;
    }

    auto const lf = this->lexical_form();
    auto u = lf | una::views::utf8;
    return std::distance(u.begin(), u.end());
}

Literal Literal::as_strlen(storage::DynNodeStoragePtr node_storage) const {
    if (this->null()) {
        return Literal{};
    }

    auto const len = this->strlen();
    if (!len.has_value()) {
        return Literal{};
    }

    return Literal::make_typed_from_value<datatypes::xsd::Integer>(datatypes::xsd::Integer::cpp_type{*len}, select_node_storage(node_storage));
}

TriBool Literal::language_tag_matches_range(std::string_view const lang_range) const noexcept {
    if (!this->is_string_like()) {
        return TriBool::Err;
    }

    return lang_matches(this->language_tag(), lang_range);
}

Literal Literal::as_language_tag_matches_range(std::string_view lang_range, storage::DynNodeStoragePtr node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    auto const res = this->language_tag_matches_range(lang_range);
    return Literal::make_boolean(res, select_node_storage(node_storage));
}

Literal Literal::as_language_tag_matches_range(Literal const &lang_range, storage::DynNodeStoragePtr node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    if (!lang_range.datatype_eq<datatypes::xsd::String>()) {
        return Literal{};
    }

    auto const lang_range_lex = lang_range.lexical_form();
    auto const res = this->language_tag_matches_range(lang_range_lex);
    return Literal::make_boolean(res, select_node_storage(node_storage));
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
            case 'm':
                return facc | regex::RegexFlag::Multiline;
            case 'x':
                return facc | regex::RegexFlag::RemoveWhitespace;
            default:
                throw std::runtime_error{std::string{"Encountered unsupported regex flag: "} + c};
        }
    });
}

TriBool Literal::regex_matches(regex::Regex const &pattern) const noexcept {
    if (!this->is_string_like()) {
        return TriBool::Err;
    }

    auto const lex = this->lexical_form();
    return pattern.regex_search(lex);
}

Literal Literal::as_regex_matches(regex::Regex const &pattern, storage::DynNodeStoragePtr node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    auto const res = this->regex_matches(pattern);
    return Literal::make_boolean(res, select_node_storage(node_storage));
}

Literal Literal::as_regex_matches(Literal const &pattern, Literal const &flags, storage::DynNodeStoragePtr node_storage) const noexcept {
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
    return Literal::make_boolean(res, select_node_storage(node_storage));
}

Literal Literal::regex_replace(regex::RegexReplacer const &replacer, storage::DynNodeStoragePtr node_storage) const {
    if (!this->is_string_like()) {
        return Literal{};
    }

    auto lf = this->lexical_form().into_owned();
    replacer.regex_replace(lf);

    return Literal::make_string_like_copy_lang_tag(lf, *this, select_node_storage(node_storage));
}

Literal Literal::regex_replace(Literal const &pattern, Literal const &replacement, Literal const &flags, storage::DynNodeStoragePtr node_storage) const {
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

TriBool Literal::contains(std::string_view const needle) const noexcept {
    if (!this->is_string_like()) {
        return TriBool::Err;
    }

    auto const s = this->lexical_form();
    auto const r = una::casesens::find_utf8(s, needle);
    return static_cast<bool>(r);
}

Literal Literal::as_contains(std::string_view const needle, storage::DynNodeStoragePtr node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    auto const res = this->contains(needle);
    return Literal::make_boolean(res, select_node_storage(node_storage));
}

Literal Literal::as_contains(Literal const &needle, storage::DynNodeStoragePtr node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    if (needle.datatype_eq<datatypes::rdf::LangString>() && this->language_tag() != needle.language_tag()) {
        return Literal{};
    }

    if (!needle.is_string_like())
        return Literal::make_boolean(true, select_node_storage(node_storage));

    auto const needle_lex = needle.lexical_form();
    auto const res = this->contains(needle_lex);
    return Literal::make_boolean(res, select_node_storage(node_storage));
}

Literal Literal::substr_before(std::string_view const needle, storage::DynNodeStoragePtr node_storage) const {
    if (!this->is_string_like()) {
        return Literal{};
    }

    if (needle.empty()) {
        return Literal::make_string_like_copy_lang_tag("", *this, select_node_storage(node_storage));
    }

    auto const s = this->lexical_form();
    auto const r = una::casesens::find_utf8(s, needle);

    if (!r) {
        return Literal::make_simple_unchecked("", false, select_node_storage(node_storage));
    }

    auto substr = s.view().substr(0, r.pos());  // find_utf8 returns byte position, not unicode character position
    return Literal::make_string_like_copy_lang_tag(substr, *this, select_node_storage(node_storage));
}

Literal Literal::substr_before(Literal const &needle, storage::DynNodeStoragePtr node_storage) const {
    if (needle.datatype_eq<datatypes::rdf::LangString>() && this->language_tag() != needle.language_tag()) {
        return Literal{};
    }

    if (!needle.is_string_like()) {
        return Literal{};
    }

    auto const needle_lex = needle.lexical_form();
    return this->substr_before(needle_lex, node_storage);
}

Literal Literal::substr_after(std::string_view const needle, storage::DynNodeStoragePtr node_storage) const {
    if (!this->is_string_like()) {
        return Literal{};
    }

    if (needle.empty()) {
        return *this;
    }

    auto const s = this->lexical_form();
    auto const r = una::casesens::find_utf8(s, needle);

    if (!r) {
        return Literal::make_simple_unchecked("", false, select_node_storage(node_storage));
    }

    auto const substr = s.view().substr(r.pos() + needle.size());  // find_utf8 returns byte position, not unicode character position
    return Literal::make_string_like_copy_lang_tag(substr, *this, select_node_storage(node_storage));
}

Literal Literal::substr_after(Literal const &needle, storage::DynNodeStoragePtr node_storage) const {
    if (needle.datatype_eq<datatypes::rdf::LangString>() && this->language_tag() != needle.language_tag()) {
        return Literal{};
    }

    if (!needle.is_string_like()) {
        return Literal{};
    }

    return this->substr_after(needle.lexical_form(), node_storage);
}

TriBool Literal::str_starts_with(std::string_view const needle) const noexcept {
    if (!this->is_string_like()) {
        return TriBool::Err;
    }

    auto const s = this->lexical_form();

    auto norm_needle = needle | una::ranges::views::utf8;
    auto len = std::ranges::distance(norm_needle);
    // TODO use std::ranges::starts_with as soon as c++ 23 arrives
    return std::ranges::equal(norm_needle, s | una::ranges::views::utf8 | una::views::take(len));
}

Literal Literal::as_str_starts_with(std::string_view const needle, storage::DynNodeStoragePtr node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    auto const res = this->str_starts_with(needle);
    return Literal::make_boolean(res, select_node_storage(node_storage));
}

Literal Literal::as_str_starts_with(Literal const &needle, storage::DynNodeStoragePtr node_storage) const noexcept {
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
    return Literal::make_boolean(res, select_node_storage(node_storage));
}

TriBool Literal::str_ends_with(std::string_view needle) const noexcept {
    if (!this->is_string_like()) {
        return TriBool::Err;
    }

    auto const s = this->lexical_form();
    auto norm_needle = needle | una::views::utf8;
    auto norm_this = s | una::views::utf8;
    auto const len_needle = std::ranges::distance(norm_needle);
    auto const len_this = std::ranges::distance(norm_this);

    // TODO use std::ranges::ends_with as soon as c++ 23 arrives
    if (len_needle > len_this)
        return false;

    return std::ranges::equal(norm_needle, norm_this | una::views::drop(len_this - len_needle));
}

Literal Literal::as_str_ends_with(std::string_view const needle, storage::DynNodeStoragePtr node_storage) const noexcept {
    if (this->null()) {
        return Literal{};
    }

    auto const res = this->str_ends_with(needle);
    return Literal::make_boolean(res, select_node_storage(node_storage));
}

Literal Literal::as_str_ends_with(Literal const &needle, storage::DynNodeStoragePtr node_storage) const noexcept {
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
    return Literal::make_boolean(res, select_node_storage(node_storage));
}

Literal Literal::uppercase(storage::DynNodeStoragePtr node_storage) const {
    if (!this->is_string_like()) {
        return Literal{};
    }

    auto const s = this->lexical_form();
    auto const upper = una::cases::to_uppercase_utf8(s);

    return Literal::make_string_like_copy_lang_tag(upper, *this, select_node_storage(node_storage));
}

Literal Literal::lowercase(storage::DynNodeStoragePtr node_storage) const {
    if (!this->is_string_like()) {
        return Literal{};
    }

    auto const s = this->lexical_form();
    const auto lower = una::cases::to_lowercase_utf8(s);

    return Literal::make_string_like_copy_lang_tag(lower, *this, select_node_storage(node_storage));
}

Literal Literal::concat(Literal const &other, storage::DynNodeStoragePtr node_storage) const {
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
            return Literal::make_lang_tagged_unchecked(combined, needs_escape, lang, select_node_storage(node_storage));
        }
    }

    return Literal::make_simple_unchecked(combined, needs_escape, select_node_storage(node_storage));
}

Literal Literal::encode_for_uri(std::string_view string, storage::DynNodeStoragePtr node_storage) {
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

Literal Literal::encode_for_uri(storage::DynNodeStoragePtr node_storage) const {
    if (!this->is_string_like())
        return Literal{};
    const auto s = this->lexical_form();
    return encode_for_uri(s, select_node_storage(node_storage));
}

Literal Literal::substr(size_t start, size_t len, storage::DynNodeStoragePtr node_storage) const {
    if (!this->is_string_like()) {
        return Literal{};
    }

    auto const s = this->lexical_form();

    auto substr = s | una::ranges::views::utf8 | una::views::drop(start) | una::views::take(len) | una::ranges::to_utf8<std::string>();

    return Literal::make_string_like_copy_lang_tag(substr, *this, select_node_storage(node_storage));
}

Literal Literal::substr(Literal const &start, Literal const &len, storage::DynNodeStoragePtr node_storage) const {
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
        return Literal::make_string_like_copy_lang_tag("", *this, select_node_storage(node_storage));
    }

    auto const len_val = len_double.value<xsd::Double>();
    if (std::isnan(len_val) || len_val <= 0) {
        return Literal::make_string_like_copy_lang_tag("", *this, select_node_storage(node_storage));
    }

    auto const start_ix = static_cast<size_t>(std::round(std::max(0.0, start_val - 1.0)));

    if (std::isinf(len_val)) {
        return this->substr(start_ix);
    }

    auto const len_ix = static_cast<size_t>(std::round(len_val) - (start_val < 1.0 ? std::round(1.0 - start_val) : 0.0));

    return this->substr(start_ix, len_ix, node_storage);
}

Literal Literal::hash_with(char const *alg, storage::DynNodeStoragePtr node_storage) const {
    if (this->handle_.node_id().literal_type() != datatypes::xsd::String::fixed_id)
        return Literal{};

    auto const s = this->lexical_form();

    unsigned char hash_buffer[EVP_MAX_MD_SIZE];
    size_t len = 0;

    if (!EVP_Q_digest(nullptr, alg, nullptr, s.data(), s.size(), hash_buffer, &len))
        return Literal{};

    std::span<std::byte const> const bytes{reinterpret_cast<std::byte const *>(hash_buffer), len};

    auto const lex = writer::StringWriter::oneshot([bytes](auto &w) {
        return datatypes::xsd::HexBinary::cpp_type::serialize_hash(bytes, w);
    });

    return Literal::make_simple(lex, select_node_storage(node_storage));
}

Literal Literal::md5(storage::DynNodeStoragePtr node_storage) const {
    return this->hash_with("MD5", node_storage);
}

Literal Literal::sha1(storage::DynNodeStoragePtr node_storage) const {
    return this->hash_with("SHA1", node_storage);
}

Literal Literal::sha256(storage::DynNodeStoragePtr node_storage) const {
    return this->hash_with("SHA2-256", node_storage);
}

Literal Literal::sha384(storage::DynNodeStoragePtr node_storage) const {
    return this->hash_with("SHA2-384", node_storage);
}

Literal Literal::sha512(storage::DynNodeStoragePtr node_storage) const {
    return this->hash_with("SHA2-512", node_storage);
}

Literal Literal::now(storage::DynNodeStoragePtr node_storage) {
    auto n = std::chrono::floor<std::chrono::milliseconds>(std::chrono::system_clock::now());
    Timezone tz{};
    TimePoint t = tz.to_local(n);
    OptionalTimezone opt = std::nullopt;
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

Literal Literal::as_year(storage::DynNodeStoragePtr node_storage) const {
    auto r = this->year();
    if (!r.has_value())
        return Literal{};
    return Literal::make_typed_from_value<datatypes::xsd::Integer>(static_cast<int>(*r), select_node_storage(node_storage));
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

Literal Literal::as_month(storage::DynNodeStoragePtr node_storage) const {
    auto r = this->month();
    if (!r.has_value())
        return Literal{};
    return Literal::make_typed_from_value<datatypes::xsd::Integer>(static_cast<unsigned int>(*r), select_node_storage(node_storage));
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

Literal Literal::as_day(storage::DynNodeStoragePtr node_storage) const {
    auto r = this->day();
    if (!r.has_value())
        return Literal{};
    return Literal::make_typed_from_value<datatypes::xsd::Integer>(static_cast<unsigned int>(*r), select_node_storage(node_storage));
}

std::optional<std::chrono::hours> Literal::hours() const noexcept {
    if (!datatype_eq<datatypes::xsd::DateTime>() && !datatype_eq<datatypes::xsd::DateTimeStamp>() && !datatype_eq<datatypes::xsd::Time>())
        return std::nullopt;
    auto casted = this->cast_to_value<datatypes::xsd::DateTime>();
    if (!casted.has_value())
        return std::nullopt;
    auto [_, time] = util::deconstruct_timepoint(casted->first);
    return std::chrono::hh_mm_ss{std::chrono::duration_cast<std::chrono::nanoseconds>(time)}.hours();
}

Literal Literal::as_hours(storage::DynNodeStoragePtr node_storage) const {
    auto r = this->hours();
    if (!r.has_value())
        return Literal{};
    return Literal::make_typed_from_value<datatypes::xsd::Integer>(r->count(), select_node_storage(node_storage));
}

std::optional<std::chrono::minutes> Literal::minutes() const noexcept {
    if (!datatype_eq<datatypes::xsd::DateTime>() && !datatype_eq<datatypes::xsd::DateTimeStamp>() && !datatype_eq<datatypes::xsd::Time>())
        return std::nullopt;
    auto casted = this->cast_to_value<datatypes::xsd::DateTime>();
    if (!casted.has_value())
        return std::nullopt;
    auto [_, time] = util::deconstruct_timepoint(casted->first);
    return std::chrono::hh_mm_ss{std::chrono::duration_cast<std::chrono::nanoseconds>(time)}.minutes();
}

Literal Literal::as_minutes(storage::DynNodeStoragePtr node_storage) const {
    auto r = this->minutes();
    if (!r.has_value())
        return Literal{};
    return Literal::make_typed_from_value<datatypes::xsd::Integer>(r->count(), select_node_storage(node_storage));
}

std::optional<std::chrono::nanoseconds> Literal::seconds() const noexcept {
    if (!datatype_eq<datatypes::xsd::DateTime>() && !datatype_eq<datatypes::xsd::DateTimeStamp>() && !datatype_eq<datatypes::xsd::Time>())
        return std::nullopt;
    auto casted = this->cast_to_value<datatypes::xsd::DateTime>();
    if (!casted.has_value())
        return std::nullopt;
    auto [_, t] = util::deconstruct_timepoint(casted->first);
    std::chrono::hh_mm_ss time{std::chrono::duration_cast<std::chrono::nanoseconds>(t)};
    return time.seconds() + time.subseconds();
}

Literal Literal::as_seconds(storage::DynNodeStoragePtr node_storage) const {
    auto r = this->seconds();
    if (!r.has_value())
        return Literal{};
    return Literal::make_typed_from_value<datatypes::xsd::Decimal>(rdf4cpp::BigDecimal<>{r->count(), 9}, select_node_storage(node_storage));
}

std::optional<Timezone> Literal::timezone() const noexcept {
    auto casted = this->cast_to_value<datatypes::xsd::DateTime>();
    if (!casted.has_value())
        return std::nullopt;
    auto tz = casted->second;
    if (!tz.has_value())
        return std::nullopt;
    return *tz;
}

Literal Literal::as_timezone(storage::DynNodeStoragePtr node_storage) const {
    auto r = this->timezone();
    if (!r.has_value())
        return Literal{};
    return Literal::make_typed_from_value<datatypes::xsd::DayTimeDuration>(r->offset, select_node_storage(node_storage));
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
Literal Literal::as_tz(storage::DynNodeStoragePtr node_storage) const {
    auto r = this->tz();
    if (!r.has_value())
        return Literal{};
    return Literal::make_simple_unchecked(*r, false, select_node_storage(node_storage));
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

Literal lang_matches(Literal const &lang_tag, Literal const &lang_range, storage::DynNodeStoragePtr node_storage) noexcept {
    if (lang_tag.null() || lang_range.null()) {
        return Literal{};
    }

    if (!lang_tag.datatype_eq<datatypes::xsd::String>() || !lang_range.datatype_eq<datatypes::xsd::String>()) {
        return Literal{};
    }

    auto const res = lang_matches(lang_tag.lexical_form(), lang_range.lexical_form());
    return Literal::make_boolean(res, lang_tag.select_node_storage(node_storage));
}

inline namespace shorthands {

Literal operator""_xsd_string(char const *str, size_t const len) {
    return Literal::make_simple(std::string_view{str, len});
}

Literal operator""_xsd_double(long double d) {
    return Literal::make_typed_from_value<datatypes::xsd::Double>(static_cast<datatypes::xsd::Double::cpp_type>(d));
}

Literal operator""_xsd_float(long double d) noexcept {
    return Literal::make_typed_from_value<datatypes::xsd::Float>(static_cast<datatypes::xsd::Float::cpp_type>(d));
}

Literal operator""_xsd_decimal(char const *str, size_t const len) {
    return Literal::make_typed<datatypes::xsd::Decimal>(std::string_view{str, len});
}

Literal operator""_xsd_integer(unsigned long long int i) {
    return Literal::make_typed_from_value<datatypes::xsd::Integer>(i);
}

Literal operator""_xsd_byte(unsigned long long int i) noexcept {
    return Literal::make_typed_from_value<datatypes::xsd::Byte>(static_cast<datatypes::xsd::Byte::cpp_type>(i));
}

Literal operator""_xsd_ubyte(unsigned long long int i) noexcept {
    return Literal::make_typed_from_value<datatypes::xsd::UnsignedByte>(static_cast<datatypes::xsd::UnsignedByte::cpp_type>(i));
}

Literal operator""_xsd_short(unsigned long long int i) noexcept {
    return Literal::make_typed_from_value<datatypes::xsd::Short>(static_cast<datatypes::xsd::Short::cpp_type>(i));
}

Literal operator""_xsd_ushort(unsigned long long int i) noexcept {
    return Literal::make_typed_from_value<datatypes::xsd::UnsignedShort>(static_cast<datatypes::xsd::UnsignedShort::cpp_type>(i));
}

Literal operator""_xsd_int(unsigned long long int i) noexcept {
    return Literal::make_typed_from_value<datatypes::xsd::Int>(static_cast<datatypes::xsd::Int::cpp_type>(i));
}

Literal operator""_xsd_uint(unsigned long long int i) noexcept {
    return Literal::make_typed_from_value<datatypes::xsd::UnsignedInt>(static_cast<datatypes::xsd::UnsignedInt::cpp_type>(i));
}

Literal operator""_xsd_long(unsigned long long int i) {
    return Literal::make_typed_from_value<datatypes::xsd::Long>(static_cast<datatypes::xsd::Long::cpp_type>(i));
}

Literal operator""_xsd_ulong(unsigned long long int i) {
    return Literal::make_typed_from_value<datatypes::xsd::UnsignedLong>(static_cast<datatypes::xsd::UnsignedLong::cpp_type>(i));
}

}  // namespace shorthands
}  // namespace rdf4cpp

auto std::formatter<rdf4cpp::Literal>::format(rdf4cpp::Literal n, format_context &ctx) const -> decltype(ctx.out()) {
    rdf4cpp::writer::BufOutputIteratorWriter w{ctx.out()};
    n.serialize(w);
    w.finalize();
    return w.buffer().iter;
}
