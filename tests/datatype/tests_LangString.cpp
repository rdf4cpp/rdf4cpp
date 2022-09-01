#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("rdf:langString") {
    CHECK_THROWS(Literal{"hello", IRI{datatypes::rdf::LangString::identifier}});
    CHECK_THROWS(Literal::make("hello", IRI{datatypes::rdf::LangString::identifier}));

    Literal const lit1{"hello", "en"};
    Literal const lit2 = Literal::make<datatypes::rdf::LangString>(datatypes::registry::LangStringRepr{"hello", "en"});

    CHECK(lit1 == lit2);

    Literal const lit3{"hello", "de-DE"};
    Literal const lit4{"hallo", "de-DE"};
    CHECK(lit1 != lit3);
    CHECK(lit1 != lit4);
    CHECK(lit3 != lit4);

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
