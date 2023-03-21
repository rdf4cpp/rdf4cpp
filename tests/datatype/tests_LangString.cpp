#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("rdf:langString") {
    Literal dummy;
    CHECK_THROWS(dummy = Literal::make_typed("hello", IRI{datatypes::rdf::LangString::identifier}));

    auto const lit1 = Literal::make_lang_tagged("hello", "en");
    auto const lit2 = Literal::make_typed_from_value<datatypes::rdf::LangString>(datatypes::registry::LangStringRepr{"hello", "en"});

    CHECK(lit1 == lit2);

    auto const lit3 = Literal::make_lang_tagged("hello", "de-DE");
    auto const lit4 = Literal::make_lang_tagged("hallo", "de-DE");
    auto const lit5 = Literal::make_lang_tagged("hallo", "de-de");
    CHECK(lit1 != lit3);
    CHECK(lit1 != lit4);
    CHECK(lit3 != lit4);
    CHECK(lit3 != lit5);
    CHECK(lit4 == lit5);

    SUBCASE("value extraction") {
        CHECK(static_cast<std::string>(lit1) == R"("hello"@en)");
        CHECK(static_cast<std::string>(lit2) == R"("hello"@en)");

        CHECK(lit1.lexical_form() == "hello");
        CHECK(lit2.lexical_form() == "hello");

        CHECK(lit1.language_tag() == "en");
        CHECK(lit2.language_tag() == "en");

        using datatypes::registry::LangStringRepr;

        CHECK(std::any_cast<LangStringRepr>(lit1.value()) == LangStringRepr{"hello", "en"});
        CHECK(std::any_cast<LangStringRepr>(lit2.value()) == LangStringRepr{"hello", "en"});

        CHECK(lit1.value<datatypes::rdf::LangString>() == LangStringRepr{"hello", "en"});
        CHECK(lit2.value<datatypes::rdf::LangString>() == LangStringRepr{"hello", "en"});
    }
}

TEST_CASE("rdf::langString inlining") {
    using namespace rdf4cpp::rdf::datatypes::registry::lang_tags;
    // these 2 checks need to be modified, if the default inlined tags change
    CHECK(tags_to_inline.size() == 4);
    CHECK(inlined_size() == 2);

    CHECK(Literal::make_lang_tagged("hello world", "en").is_inlined());
    CHECK(Literal::make_lang_tagged("hello world", "de").is_inlined());
    CHECK(!Literal::make_lang_tagged("hello world", "en-us").is_inlined());

    CHECK(!try_into_inlined(storage::node::identifier::LiteralID{1l << 41}, 0).has_value());
}
