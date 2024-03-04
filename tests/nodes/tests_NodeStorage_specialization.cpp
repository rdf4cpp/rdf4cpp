#define DOCTEST_CONFIG_IMPLEMENT

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/SyncReferenceNodeStorageBackend.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/UnsyncReferenceNodeStorageBackend.hpp>

#include <array>
#include <iostream>

using namespace rdf4cpp::rdf;
using namespace storage::node;
using namespace datatypes;

int main(int argc, char **argv) {
    /*NodeStorage::set_default_instance(NodeStorage::new_instance<reference_node_storage::SyncReferenceNodeStorageBackend>());
    doctest::Context{argc, argv}.run();

    NodeStorage::set_default_instance(NodeStorage::new_instance<reference_node_storage::UnsyncReferenceNodeStorageBackend>());*/
    doctest::Context{argc, argv}.run(); // TODO
}

template<typename It>
void print_handles(It begin, It const end) {
    for (; begin != end; ++begin) {
        std::cout << begin->id_to_underlying() << " ";
    }
    std::cout << '\n';
}

template<FixedIdLiteralDatatype T, size_t N>
void check_specialized_storage_usage(std::array<typename T::cpp_type, N> const &test_values) {
    std::string const case_name{T::identifier};

    SUBCASE(case_name.c_str()) {
        CHECK(storage::node::default_node_storage.has_specialized_storage_for(T::fixed_id));

        std::array<identifier::NodeBackendHandle, N> assigned_ids;

        for (size_t ix = 0; ix < N; ++ix) {
            auto const &value = test_values[ix];

            auto const lex = writer::StringWriter::oneshot([&value](writer::StringWriter &w) noexcept {
                return T::serialize_canonical_string(value, w);
            });

            std::cout << "Testing with: " << lex << " as " << std::string_view{T::identifier} << '\n';

            view::ValueLiteralBackendView view{.datatype = T::fixed_id, .value = value};
            auto const id = default_node_storage.find_or_make_id(view);
            auto const id2 = default_node_storage.find_or_make_id(view);
            CHECK(id == id2);

            auto const erased_1 = default_node_storage.erase_literal(id);
            CHECK(erased_1);
            auto const erased_2 = default_node_storage.erase_literal(id2);
            CHECK(!erased_2);

            auto lit1 = Literal::make_typed_from_value<T>(value);
            auto lit2 = Literal::make_typed<T>(lex);
            auto lit3 = Literal::make_typed(lex, IRI{T::identifier});

            CHECK(lit1.backend_handle() == lit2.backend_handle());
            CHECK(lit2.backend_handle() == lit3.backend_handle());
            CHECK(lit3.backend_handle() == lit1.backend_handle());

            assigned_ids[ix] = lit1.backend_handle();

            auto const backend1 = lit1.backend_handle().literal_backend().get_value();
            auto const backend2 = lit2.backend_handle().literal_backend().get_value();
            auto const backend3 = lit3.backend_handle().literal_backend().get_value();

            CHECK(T::fixed_id == backend1.datatype);
            CHECK(backend1.datatype == backend2.datatype);
            CHECK(backend2.datatype == backend3.datatype);
            CHECK(backend3.datatype == T::fixed_id);

            CHECK(value == std::any_cast<typename T::cpp_type>(backend1.value));
            CHECK(std::any_cast<typename T::cpp_type>(backend1.value) == std::any_cast<typename T::cpp_type>(backend2.value));
            CHECK(std::any_cast<typename T::cpp_type>(backend2.value) == std::any_cast<typename T::cpp_type>(backend3.value));
            CHECK(std::any_cast<typename T::cpp_type>(backend3.value) == value);

            auto const value1 = std::any_cast<typename T::cpp_type>(lit1.value());
            auto const value2 = std::any_cast<typename T::cpp_type>(lit2.value());
            auto const value3 = std::any_cast<typename T::cpp_type>(lit3.value());
            auto const value4 = lit1.template value<T>();
            auto const value5 = lit2.template value<T>();
            auto const value6 = lit3.template value<T>();

            CHECK(value == value1);
            CHECK(value1 == value2);
            CHECK(value2 == value3);
            CHECK(value3 == value4);
            CHECK(value4 == value5);
            CHECK(value5 == value6);
            CHECK(value6 == value);
        }

        std::sort(assigned_ids.begin(), assigned_ids.end());
        print_handles(assigned_ids.begin(), assigned_ids.end());

        auto const old_sz = assigned_ids.size();
        auto new_end = std::unique(assigned_ids.begin(), assigned_ids.end());
        print_handles(assigned_ids.begin(), new_end);

        CHECK(std::distance(assigned_ids.begin(), new_end) == old_sz);
    }
}

TEST_CASE_TEMPLATE("NodeStorage specialization big positive numbers", T,
                   xsd::Long, xsd::Integer, xsd::PositiveInteger,
                   xsd::NonNegativeInteger, xsd::Decimal) {
    std::array<typename T::cpp_type, 2> const test_values{
            static_cast<typename T::cpp_type>(std::numeric_limits<xsd::Long::cpp_type>::max()),
            static_cast<typename T::cpp_type>(std::numeric_limits<xsd::Long::cpp_type>::max() - 10)};

    check_specialized_storage_usage<T>(test_values);
}

TEST_CASE_TEMPLATE("NodeStorage specialization uninlineable doubles", T, xsd::Double) {
    std::array<xsd::Double::cpp_type, 2> const test_values{
            1.23141321,
            54234.12378312};

    check_specialized_storage_usage<T>(test_values);
}

TEST_CASE_TEMPLATE("NodeStorage specialization big negative numbers", T,
                   xsd::NonPositiveInteger, xsd::NegativeInteger) {
    std::array<typename T::cpp_type, 2> const test_values{
            static_cast<typename T::cpp_type>(std::numeric_limits<xsd::Long::cpp_type>::min()),
            static_cast<typename T::cpp_type>(std::numeric_limits<xsd::Long::cpp_type>::min() + 10)};

    check_specialized_storage_usage<T>(test_values);
}

TEST_CASE("NodeStorage specialization xsd:hexBinary") {
    std::array<xsd::HexBinary::cpp_type, 2> const test_values{
            xsd::HexBinary::cpp_type{{std::byte{0x12}, std::byte{0x34}, std::byte{0x56}}},
            xsd::HexBinary::cpp_type{{std::byte{0x78}, std::byte{0x42}, std::byte{0x78}}}};

    check_specialized_storage_usage<xsd::HexBinary>(test_values);
}

TEST_CASE("NodeStorage specialization xsd:base64Binary") {
    std::array<xsd::Base64Binary::cpp_type, 2> const test_values{
            xsd::Base64Binary::cpp_type{{std::byte{0x12}, std::byte{0x34}, std::byte{0x56}}},
            xsd::Base64Binary::cpp_type{{std::byte{0x78}, std::byte{0x42}, std::byte{0x78}}}};

    check_specialized_storage_usage<xsd::Base64Binary>(test_values);
}

TEST_CASE("NodeStorage non-specialization xsd:String") {
    CHECK(!default_node_storage.has_specialized_storage_for(xsd::String::fixed_id));

    std::array<xsd::String::cpp_type, 2> const test_values{
            "Hello World",
            "Spherical Cow"};

    std::array<identifier::NodeBackendHandle, 2> assigned_ids;

    for (size_t ix = 0; ix < test_values.size(); ++ix) {
        auto const &value = test_values[ix];

        std::cout << "Testing with: " << std::quoted(value) << " as " << std::string_view{xsd::String::identifier} << '\n';

        view::LexicalFormLiteralBackendView view{.datatype_id = identifier::NodeID::xsd_string_iri.first, .lexical_form = value, .language_tag = "", .needs_escape = false};
        auto const id = default_node_storage.find_or_make_id(view);
        auto const id2 = default_node_storage.find_or_make_id(view);
        CHECK(id == id2);

        auto const erased_1 = default_node_storage.erase_literal(id);
        CHECK(erased_1);
        auto const erased_2 = default_node_storage.erase_literal(id2);
        CHECK(!erased_2);

        auto lit1 = Literal::make_typed_from_value<xsd::String>(value);
        auto lit2 = Literal::make_typed<xsd::String>(value);
        auto lit3 = Literal::make_typed(value, IRI{xsd::String::identifier});
        auto lit4 = Literal::make_simple(value);

        CHECK(lit1.backend_handle() == lit2.backend_handle());
        CHECK(lit2.backend_handle() == lit3.backend_handle());
        CHECK(lit3.backend_handle() == lit4.backend_handle());
        CHECK(lit4.backend_handle() == lit1.backend_handle());

        assigned_ids[ix] = lit1.backend_handle();

        auto const backend1 = lit1.backend_handle().literal_backend().get_lexical();
        auto const backend2 = lit2.backend_handle().literal_backend().get_lexical();
        auto const backend3 = lit3.backend_handle().literal_backend().get_lexical();
        auto const backend4 = lit3.backend_handle().literal_backend().get_lexical();

        CHECK(identifier::NodeID::xsd_string_iri.first == backend1.datatype_id);
        CHECK(backend1.datatype_id == backend2.datatype_id);
        CHECK(backend2.datatype_id == backend3.datatype_id);
        CHECK(backend3.datatype_id == backend4.datatype_id);
        CHECK(backend4.datatype_id == identifier::NodeID::xsd_string_iri.first);

        CHECK(backend1.language_tag == "");
        CHECK(backend2.language_tag == "");
        CHECK(backend3.language_tag == "");
        CHECK(backend4.language_tag == "");

        CHECK(value == backend1.lexical_form);
        CHECK(backend1.lexical_form == backend2.lexical_form);
        CHECK(backend2.lexical_form == backend3.lexical_form);
        CHECK(backend3.lexical_form == backend4.lexical_form);
        CHECK(backend4.lexical_form == value);

        auto const value1 = std::any_cast<xsd::String::cpp_type>(lit1.value());
        auto const value2 = std::any_cast<xsd::String::cpp_type>(lit2.value());
        auto const value3 = std::any_cast<xsd::String::cpp_type>(lit3.value());
        auto const value4 = std::any_cast<xsd::String::cpp_type>(lit4.value());
        auto const value5 = lit1.template value<xsd::String>();
        auto const value6 = lit2.template value<xsd::String>();
        auto const value7 = lit3.template value<xsd::String>();
        auto const value8 = lit4.template value<xsd::String>();

        CHECK(value == value1);
        CHECK(value1 == value2);
        CHECK(value2 == value3);
        CHECK(value3 == value4);
        CHECK(value4 == value5);
        CHECK(value5 == value6);
        CHECK(value6 == value7);
        CHECK(value7 == value8);
        CHECK(value8 == value);
    }

    std::sort(assigned_ids.begin(), assigned_ids.end());
    print_handles(assigned_ids.begin(), assigned_ids.end());

    auto const old_sz = assigned_ids.size();
    auto new_end = std::unique(assigned_ids.begin(), assigned_ids.end());
    print_handles(assigned_ids.begin(), new_end);

    CHECK(std::distance(assigned_ids.begin(), new_end) == old_sz);
}

TEST_CASE("NodeStorage non-specialization rdf:langString") {
    auto extract_backend_handle = [](Literal l) {
        auto h = l.backend_handle();
        if (!h.is_inlined())
            return h;
        auto [_, id] = rdf4cpp::rdf::datatypes::registry::DatatypeRegistry::LangTagInlines::from_inlined(h.node_id().literal_id());
        auto node_id = storage::node::identifier::NodeID{id, h.node_id().literal_type()};
        return rdf4cpp::rdf::storage::node::identifier::NodeBackendHandle{node_id,
                                                                          storage::node::identifier::RDFNodeType::Literal,
                                                                          h.storage()};
    };

    CHECK(!default_node_storage.has_specialized_storage_for(rdf::LangString::fixed_id));

    std::array<rdf::LangString::cpp_type, 3> const test_values{
            rdf::LangString::cpp_type{.lexical_form = "Hello World", .language_tag = "en"},
            rdf::LangString::cpp_type{.lexical_form = "Spherical Cow", .language_tag = "en"},
            rdf::LangString::cpp_type{.lexical_form = "Hund", .language_tag = "de"}};

    std::array<identifier::NodeBackendHandle, 3> assigned_ids;

    for (size_t ix = 0; ix < test_values.size(); ++ix) {
        auto const &value = test_values[ix];

        std::cout << "Testing with: " << std::quoted(value.lexical_form) << "@" << value.language_tag << " as " << std::string_view{rdf::LangString::identifier} << '\n';

        view::LexicalFormLiteralBackendView view{.datatype_id = identifier::NodeID::xsd_string_iri.first,
                                                 .lexical_form = value.lexical_form,
                                                 .language_tag = value.language_tag,
                                                 .needs_escape = false};
        auto const id = default_node_storage.find_or_make_id(view);
        auto const id2 = default_node_storage.find_or_make_id(view);
        CHECK(id == id2);

        auto const erased_1 = default_node_storage.erase_literal(id);
        CHECK(erased_1);
        auto const erased_2 = default_node_storage.erase_literal(id2);
        CHECK(!erased_2);

        auto lit1 = Literal::make_typed_from_value<rdf::LangString>(value);
        auto lit2 = Literal::make_lang_tagged(value.lexical_form, value.language_tag);

        CHECK(lit1.backend_handle() == lit2.backend_handle());
        CHECK(lit2.backend_handle() == lit1.backend_handle());

        assigned_ids[ix] = extract_backend_handle(lit1);

        auto const backend1 = extract_backend_handle(lit1).literal_backend().get_lexical();
        auto const backend2 = extract_backend_handle(lit2).literal_backend().get_lexical();

        CHECK(identifier::NodeID::rdf_langstring_iri.first == backend1.datatype_id);
        CHECK(backend1.datatype_id == backend2.datatype_id);
        CHECK(backend2.datatype_id == identifier::NodeID::rdf_langstring_iri.first);

        CHECK(value.language_tag == backend1.language_tag);
        CHECK(backend1.language_tag == backend2.language_tag);
        CHECK(backend2.language_tag == value.language_tag);

        CHECK(value.lexical_form == backend1.lexical_form);
        CHECK(backend1.lexical_form == backend2.lexical_form);
        CHECK(backend2.lexical_form == value.lexical_form);

        auto const value1 = std::any_cast<rdf::LangString::cpp_type>(lit1.value());
        auto const value2 = std::any_cast<rdf::LangString::cpp_type>(lit2.value());
        auto const value3 = lit1.template value<rdf::LangString>();
        auto const value4 = lit2.template value<rdf::LangString>();

        CHECK(value == value1);
        CHECK(value1 == value2);
        CHECK(value2 == value3);
        CHECK(value3 == value4);
        CHECK(value4 == value);
    }

    std::sort(assigned_ids.begin(), assigned_ids.end());
    print_handles(assigned_ids.begin(), assigned_ids.end());

    auto const old_sz = assigned_ids.size();
    auto new_end = std::unique(assigned_ids.begin(), assigned_ids.end());
    print_handles(assigned_ids.begin(), new_end);

    CHECK(std::distance(assigned_ids.begin(), new_end) == old_sz);
}
