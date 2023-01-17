#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Literal - Check for only lexical form") {

    auto iri = IRI{"http://www.w3.org/2001/XMLSchema#string"};
    auto lit1 = Literal{"Bugs Bunny"};

    CHECK(not lit1.is_blank_node());
    CHECK(lit1.is_literal());
    CHECK(not lit1.is_variable());
    CHECK(not lit1.is_iri());
    CHECK(lit1.lexical_form() == "Bugs Bunny");
    CHECK(lit1.datatype() == iri);
    CHECK(lit1.language_tag() == "");
    CHECK(std::string(lit1) == "\"Bugs Bunny\"^^<http://www.w3.org/2001/XMLSchema#string>");
}

TEST_CASE("Literal - Check for lexical form with IRI") {

    SUBCASE("string datatype") {
        auto iri = IRI{"http://www.w3.org/2001/XMLSchema#string"};
        auto lit1 = Literal{"Bugs Bunny", iri};

        CHECK(not lit1.is_blank_node());
        CHECK(lit1.is_literal());
        CHECK(not lit1.is_variable());
        CHECK(not lit1.is_iri());
        CHECK(lit1.lexical_form() == "Bugs Bunny");
        CHECK(lit1.datatype() == iri);
        CHECK(lit1.language_tag() == "");
        CHECK(std::string(lit1) == "\"Bugs Bunny\"^^<http://www.w3.org/2001/XMLSchema#string>");
    }
    SUBCASE("int datatype") {
        auto iri = IRI{"http://www.w3.org/2001/XMLSchema#int"};
        auto lit1 = Literal{"101", iri};

        CHECK(not lit1.is_blank_node());
        CHECK(lit1.is_literal());
        CHECK(not lit1.is_variable());
        CHECK(not lit1.is_iri());
        CHECK(lit1.lexical_form() == "101");
        CHECK(lit1.datatype() == iri);
        CHECK(lit1.language_tag() == "");
        CHECK(std::string(lit1) == "\"101\"^^<http://www.w3.org/2001/XMLSchema#int>");
    }
    SUBCASE("date datatype") {
        auto iri = IRI{"http://www.w3.org/2001/XMLSchema#date"};
        auto lit1 = Literal{"2021-11-21", iri};

        CHECK(not lit1.is_blank_node());
        CHECK(lit1.is_literal());
        CHECK(not lit1.is_variable());
        CHECK(not lit1.is_iri());
        CHECK(lit1.lexical_form() == "2021-11-21");
        CHECK(lit1.datatype() == iri);
        CHECK(lit1.language_tag() == "");
        CHECK(std::string(lit1) == "\"2021-11-21\"^^<http://www.w3.org/2001/XMLSchema#date>");
    }
    SUBCASE("decimal datatype") {
        auto iri = IRI{"http://www.w3.org/2001/XMLSchema#decimal"};
        auto lit1 = Literal{"2.0", iri};

        CHECK(not lit1.is_blank_node());
        CHECK(lit1.is_literal());
        CHECK(not lit1.is_variable());
        CHECK(not lit1.is_iri());
        CHECK(lit1.lexical_form() == "2.0");
        CHECK(lit1.datatype() == iri);
        CHECK(lit1.language_tag() == "");
        CHECK(std::string(lit1) == "\"2.0\"^^<http://www.w3.org/2001/XMLSchema#decimal>");
    }
    SUBCASE("boolean datatype - true") {
        auto iri = IRI{"http://www.w3.org/2001/XMLSchema#boolean"};
        auto lit1 = Literal{"true", iri};

        CHECK(not lit1.is_blank_node());
        CHECK(lit1.is_literal());
        CHECK(not lit1.is_variable());
        CHECK(not lit1.is_iri());
        CHECK(lit1.lexical_form() == "true");
        CHECK(lit1.datatype() == iri);
        CHECK(lit1.language_tag() == "");
        CHECK(std::string(lit1) == "\"true\"^^<http://www.w3.org/2001/XMLSchema#boolean>");
    }
    SUBCASE("boolean datatype - false") {
        auto iri = IRI{"http://www.w3.org/2001/XMLSchema#boolean"};
        auto lit1 = Literal{"false", iri};

        CHECK(not lit1.is_blank_node());
        CHECK(lit1.is_literal());
        CHECK(not lit1.is_variable());
        CHECK(not lit1.is_iri());
        CHECK(lit1.lexical_form() == "false");
        CHECK(lit1.datatype() == iri);
        CHECK(lit1.language_tag() == "");
        CHECK(std::string(lit1) == "\"false\"^^<http://www.w3.org/2001/XMLSchema#boolean>");
    }
    SUBCASE("boolean datatype - 0") {
        auto iri = IRI{"http://www.w3.org/2001/XMLSchema#boolean"};
        auto lit1 = Literal{"0", iri};

        CHECK(not lit1.is_blank_node());
        CHECK(lit1.is_literal());
        CHECK(not lit1.is_variable());
        CHECK(not lit1.is_iri());
        CHECK(lit1.lexical_form() == "false");
        CHECK(lit1.datatype() == iri);
        CHECK(lit1.language_tag() == "");
        CHECK(std::string(lit1) == "\"false\"^^<http://www.w3.org/2001/XMLSchema#boolean>");
    }
    SUBCASE("boolean datatype - 1") {
        auto iri = IRI{"http://www.w3.org/2001/XMLSchema#boolean"};
        auto lit1 = Literal{"1", iri};

        CHECK(not lit1.is_blank_node());
        CHECK(lit1.is_literal());
        CHECK(not lit1.is_variable());
        CHECK(not lit1.is_iri());
        CHECK(lit1.lexical_form() == "true");
        CHECK(lit1.datatype() == iri);
        CHECK(lit1.language_tag() == "");
        CHECK(std::string(lit1) == "\"true\"^^<http://www.w3.org/2001/XMLSchema#boolean>");
    }
}

TEST_CASE("Literal - Check for lexical form with language tag") {

    auto iri = IRI{"http://www.w3.org/1999/02/22-rdf-syntax-ns#langString"};
    auto lit1 = Literal{"Bugs Bunny", "en"};

    CHECK(not lit1.is_blank_node());
    CHECK(lit1.is_literal());
    CHECK(not lit1.is_variable());
    CHECK(not lit1.is_iri());
    CHECK(lit1.lexical_form() == "Bugs Bunny");
    CHECK(lit1.datatype() == iri);
    CHECK(lit1.language_tag() == "en");
    CHECK(std::string(lit1) == "\"Bugs Bunny\"@en");
}

TEST_CASE("Literal - ctor edge-case") {
    IRI const iri{"http://www.w3.org/2001/XMLSchema#int"};
    Literal const lit1{"1", iri};
    Literal const lit2{"2", iri};

    Literal const expected{"3", iri};
    CHECK(lit1 + lit2 == expected);
}

TEST_CASE("Literal - check fixed id") {
    IRI const iri{datatypes::registry::xsd_string};
    Literal const lit{"hello", iri};

    CHECK(lit.backend_handle().node_id().literal_type().is_fixed());
    CHECK(lit.datatype().backend_handle().node_id().value() < datatypes::registry::min_dynamic_datatype_id);
    CHECK(iri.backend_handle().node_id().value() < datatypes::registry::min_dynamic_datatype_id);
}

TEST_CASE("Literal - casting") {
    using namespace datatypes::xsd;

    auto const lit1 = Literal::make<datatypes::xsd::Int>(123);

    SUBCASE("id cast") {
        auto const lit1 = Literal::make<String>("hello");
        auto const lit2 = lit1.template cast<String>();

        CHECK(lit2.datatype() == IRI{String::identifier});
        CHECK(lit2.template value<String>() == "hello");
    }

    SUBCASE("str -> any") {
        auto const lit1 = Literal::make<String>("1.2");
        auto const lit2 = lit1.template cast<Float>();

        CHECK(lit2.datatype() == IRI{Float::identifier});
        CHECK(lit2.value<Float>() == 1.2f);
    }

    SUBCASE("any -> str") {
        auto const lit1 = Literal::make<Decimal>(1.5);
        auto const lit2 = lit1.template cast<String>();

        CHECK(lit2.template value<String>() == "1.5");
    }

    SUBCASE("any -> bool") {
        auto const lit1 = Literal::make<Float>(1.4);
        auto const lit2 = lit1.template cast<Boolean>();

        CHECK(lit2.datatype() == IRI{Boolean::identifier});
        CHECK(lit2.template value<Boolean>() == true);
    }

    SUBCASE("downcast: dbl -> flt") {
        auto const lit1 = Literal::make<Double>(1.4);
        auto const lit2 = lit1.template cast<Float>();

        CHECK(lit2.datatype() == IRI{Float::identifier});
        CHECK(lit2.template value<Float>() == 1.4f);
    }

    SUBCASE("dec -> flt") {
        auto const lit1 = Literal::make<Decimal>(1.0);
        auto const lit2 = lit1.template cast<Float>();

        CHECK(lit2.datatype() == IRI{Float::identifier});
        CHECK(lit2.template value<Float>() == 1.f);
    }

    SUBCASE("dec -> dbl") {
        auto const lit1 = Literal::make<Decimal>(1.0);
        auto const lit2 = lit1.template cast<Double>();

        CHECK(lit2.datatype() == IRI{Double::identifier});
        CHECK(lit2.template value<Double>() == 1.0);
    }

    SUBCASE("dec -> int") {
        auto const lit1 = Literal::make<Decimal>(1.2);
        auto const lit2 = lit1.template cast<Int>();

        CHECK(lit2.datatype() == IRI{Int::identifier});
        CHECK(lit2.template value<Int>() == 1);
    }

    SUBCASE("downcast: int -> dec") {
        auto const lit1 = Literal::make<Integer>(1);
        auto const lit2 = lit1.template cast<Decimal>();

        CHECK(lit2.datatype() == IRI{Decimal::identifier});
        CHECK(lit2.template value<Decimal>() == 1);
    }

    SUBCASE("int -> flt") {
        auto const lit1 = Literal::make<Integer>(1);
        auto const lit2 = lit1.template cast<Float>();

        CHECK(lit2.datatype() == IRI{Float::identifier});
        CHECK(lit2.template value<Float>() == 1.f);
    }

    SUBCASE("int -> dbl") {
        auto const lit1 = Literal::make<Integer>(1);
        auto const lit2 = lit1.template cast<Double>();

        CHECK(lit2.datatype() == IRI{Double::identifier});
        CHECK(lit2.template value<Double>() == 1.0);
    }

    SUBCASE("id cast") {
        auto const lit1 = Literal::make<Int>(5);
        auto const lit2 = lit1.template cast<Int>();

        CHECK(lit1 == lit2);
    }

    SUBCASE("bool -> numeric") {
        SUBCASE("integers") {
            SUBCASE("regular case") {
                auto const lit1 = Literal::make<Boolean>(true);
                auto const lit2 = lit1.template cast<Byte>();
                CHECK(!lit2.null());
                CHECK(lit2.datatype() == IRI{Byte::identifier});
                CHECK(lit2.template value<Byte>() == 1);
            }

            SUBCASE("partially representable - representable case") {
                auto const lit3 = Literal::make<Boolean>(false);
                auto const lit4 = lit3.template cast<NonPositiveInteger>();
                CHECK(!lit4.null());
                CHECK(lit4.datatype() == IRI{NonPositiveInteger::identifier});
                CHECK(lit4.template value<NonPositiveInteger>() == 0);
            }

            SUBCASE("partially representable - unrepresentable case") {
                auto const lit3 = Literal::make<Boolean>(true);
                auto const lit4 = lit3.template cast<NegativeInteger>();
                CHECK(lit4.null());
            }
        }

        SUBCASE("decimal") {
            auto const lit1 = Literal::make<Boolean>(false);
            auto const lit2 = lit1.template cast<Decimal>();
            CHECK(!lit2.null());
            CHECK(lit2.datatype() == IRI{Decimal::identifier});
            CHECK(lit2.template value<Decimal>() == 0.0);
        }

        SUBCASE("float") {
            auto const lit1 = Literal::make<Boolean>(true);
            auto const lit2 = lit1.template cast<Float>();
            CHECK(!lit2.null());
            CHECK(lit2.datatype() == IRI{Float::identifier});
            CHECK(lit2.template value<Float>() == 1.f);
        }

        SUBCASE("double") {
            auto const lit1 = Literal::make<Boolean>(false);
            auto const lit2 = lit1.template cast<Double>();
            CHECK(!lit2.null());
            CHECK(lit2.datatype() == IRI{Double::identifier});
            CHECK(lit2.template value<Double>() == 0.0);
        }
    }

    SUBCASE("cross hierarchy: int -> unsignedInt") {
        auto const lit1 = Literal::make<Int>(1);
        auto const lit2 = lit1.template cast<UnsignedInt>();

        CHECK(lit2.datatype() == IRI{UnsignedInt::identifier});
        CHECK(lit2.value<UnsignedInt>() == 1);
    }

    SUBCASE("subtypes") {
        CHECK(lit1.template cast<Integer>().datatype() == IRI{Integer::identifier});
        CHECK(lit1.template cast<Float>().datatype() == IRI{Float::identifier});

        auto const lit2 = Literal::make<Integer>(420);
        CHECK(lit2.template cast<Int>() == Literal::make<Int>(420));
    }

    SUBCASE("value too large") {
        auto const lit1 = Literal::make<Int>(67000);
        auto const lit2 = lit1.template cast<Short>();

        CHECK(lit2.null());
    }

    SUBCASE("negative to unsigned") {
        auto const lit1 = Literal::make<Int>(-10);
        auto const lit2 = lit1.template cast<UnsignedInt>();

        CHECK(lit2.null());
    }
}

TEST_CASE("Literal - misc functions") {
    using namespace rdf4cpp::rdf;

    SUBCASE("strlen") {
        CHECK("12345"_xsd_string .as_strlen() == 5_xsd_integer);
        CHECK(1_xsd_int .as_strlen().null());
        CHECK("123"_xsd_string .as_strlen() == 3_xsd_integer);
        CHECK(Literal{"hello", "en"}.as_strlen() == 5_xsd_integer);

        CHECK("z\u00df\u6c34\U0001f34c"_xsd_string .as_strlen() == 4_xsd_integer); // "zß水🍌"
    }

    SUBCASE("substr") {
        // from https://www.w3.org/TR/xpath-functions/#func-substring
        CHECK("motor car"_xsd_string.substr(6_xsd_integer) == " car"_xsd_string);
        CHECK("metadata"_xsd_string.substr(4_xsd_integer, 3_xsd_integer) == "ada"_xsd_string);
        CHECK("12345"_xsd_string.substr("1.5"_xsd_decimal, "2.6"_xsd_decimal) == "234"_xsd_string);
        CHECK("12345"_xsd_string.substr(0_xsd_integer, 3_xsd_integer) == "12"_xsd_string);
        CHECK("12345"_xsd_string.substr(5_xsd_integer, -3_xsd_integer) == ""_xsd_string);
        CHECK("12345"_xsd_string.substr(-3_xsd_integer, 5_xsd_integer) == "1"_xsd_string);
        CHECK("12345"_xsd_string.substr(0_xsd_integer / 0.0_xsd_double, 3_xsd_integer) == ""_xsd_string);
        CHECK("12345"_xsd_string.substr(1_xsd_integer, 0_xsd_integer / 0.0_xsd_double) == ""_xsd_string);
        CHECK("12345"_xsd_string.substr(-42_xsd_integer, 1_xsd_integer / 0.0_xsd_double) == "12345"_xsd_string);

        // from https://www.w3.org/TR/sparql11-query/#func-substr
        CHECK("foobar"_xsd_string .substr(4_xsd_integer) == "bar"_xsd_string);
        CHECK(Literal{"foobar", "en"}.substr(4_xsd_integer) == Literal{"bar", "en"});
        CHECK("foobar"_xsd_string .substr(4_xsd_integer, 1_xsd_integer) == "b"_xsd_string);
        CHECK(Literal{"foobar", "en"}.substr(4_xsd_integer, 1_xsd_integer) == Literal{"b", "en"});

        // check correct casting
        auto const s = "Hello World"_xsd_string;
        CHECK(s.substr(2_xsd_long, -1.3_xsd_double) == ""_xsd_string);
        CHECK(s.substr(2.1_xsd_double, 3.2_xsd_double) == "ell"_xsd_string);
        CHECK(s.substr(100_xsd_integer, 10_xsd_int) == ""_xsd_string);
    }

    // TODO: comment in when langMatches finished
    /*SUBCASE("langMatches") {
        CHECK(Literal{"Hello", "en"}.lang_matches("*"_xsd_string).ebv());
        CHECK(Literal{"Bonjour", "fr"}.lang_matches("FR"_xsd_string).ebv());
        CHECK(Literal{"Hello", "en-US"}.lang_matches("en-US"_xsd_string).ebv());
        CHECK(5_xsd_int .lang_matches("*"_xsd_string).null());
        CHECK("Hello"_xsd_string .lang_matches(""_xsd_string).ebv());
        CHECK("Hello"_xsd_string .lang_matches("*"_xsd_string).ebv() == util::TriBool::False);
    }*/

    SUBCASE("ucase") {
        // from https://www.w3.org/TR/sparql11-query/#func-ucase
        CHECK("foo"_xsd_string .uppercase() == "FOO"_xsd_string);
        CHECK(Literal{"foo", "en"}.uppercase() == Literal{"FOO", "en"});
    }

    SUBCASE("lcase") {
        // from https://www.w3.org/TR/sparql11-query/#func-lcase
        CHECK("BAR"_xsd_string .lowercase() == "bar"_xsd_string);
        CHECK(Literal{"BAR", "en"}.lowercase() == Literal{"bar", "en"});
    }

    SUBCASE("contains") {
        // from https://www.w3.org/TR/sparql11-query/#func-contains
        CHECK("foobar"_xsd_string .contains("bar"_xsd_string).ebv());
        CHECK(Literal{"foobar", "en"}.contains(Literal{"foo", "en"}).ebv());
        CHECK(Literal{"foobar", "en"}.contains("bar"_xsd_string).ebv());

        CHECK(Literal{"hello", "en"}.contains(Literal{"o", "fr"}).null());
        CHECK("123"_xsd_string .contains(Literal{"1", "en"}).null());
    }

    SUBCASE("substr_before") {
        // from https://www.w3.org/TR/sparql11-query/#func-strbefore
        CHECK("abc"_xsd_string .substr_before("b"_xsd_string) == "a"_xsd_string);
        CHECK(Literal{"abc", "en"}.substr_before("bc"_xsd_string) == Literal{"a", "en"});
        CHECK(Literal{"abc", "en"}.substr_before(Literal{"b", "cy"}).null());
        CHECK("abc"_xsd_string .substr_before(""_xsd_string) == ""_xsd_string);
        CHECK("abc"_xsd_string .substr_before("xyz"_xsd_string) == ""_xsd_string);
        CHECK(Literal{"abc", "en"}.substr_before(Literal{"z", "en"}) == ""_xsd_string);
        CHECK(Literal{"abc", "en"}.substr_before("z"_xsd_string) == ""_xsd_string);
        CHECK(Literal{"abc", "en"}.substr_before(Literal{"", "en"}) == Literal{"", "en"});
        CHECK(Literal{"abc", "en"}.substr_before(""_xsd_string) == Literal{"", "en"});
    }

    SUBCASE("substr_after") {
        // from https://www.w3.org/TR/sparql11-query/#func-strafter
        CHECK("abc"_xsd_string .substr_after("b"_xsd_string) == "c"_xsd_string);
        CHECK(Literal{"abc", "en"}.substr_after("ab"_xsd_string) == Literal{"c", "en"});
        CHECK(Literal{"abc", "en"}.substr_after(Literal{"b", "cy"}).null());
        CHECK("abc"_xsd_string .substr_after(""_xsd_string) == "abc"_xsd_string);
        CHECK("abc"_xsd_string .substr_after("xyz"_xsd_string) == ""_xsd_string);
        CHECK(Literal{"abc", "en"}.substr_after(Literal{"z", "en"}) == ""_xsd_string);
        CHECK(Literal{"abc", "en"}.substr_after("z"_xsd_string) == ""_xsd_string);
        CHECK(Literal{"abc", "en"}.substr_after(Literal{"", "en"}) == Literal{"abc", "en"});
        CHECK(Literal{"abc", "en"}.substr_after(""_xsd_string) == Literal{"abc", "en"});
    }

    SUBCASE("str_start_with") {
        // from https://www.w3.org/TR/sparql11-query/#func-strstarts
        CHECK("foobar"_xsd_string .str_starts_with("foo"_xsd_string).ebv());
        CHECK(Literal{"foobar", "en"}.str_starts_with(Literal{"foo", "en"}).ebv());
        CHECK(Literal{"foobar", "en"}.str_starts_with("foo"_xsd_string).ebv());

        CHECK(Literal{"foobar", "fr"}.str_starts_with(Literal{"foo", "en"}).null());
        CHECK("foobar"_xsd_string .str_starts_with(Literal{"foo", "en"}).null());
    }

    SUBCASE("str_ends_with") {
        // from https://www.w3.org/TR/sparql11-query/#func-strstarts
        CHECK("foobar"_xsd_string .str_ends_with("bar"_xsd_string).ebv());
        CHECK(Literal{"foobar", "en"}.str_ends_with(Literal{"bar", "en"}).ebv());
        CHECK(Literal{"foobar", "en"}.str_ends_with("bar"_xsd_string).ebv());

        CHECK(Literal{"foobar", "fr"}.str_ends_with(Literal{"bar", "en"}).null());
        CHECK("foobar"_xsd_string .str_ends_with(Literal{"bar", "en"}).null());
    }

    SUBCASE("concat") {
        // from https://www.w3.org/TR/sparql11-query/#func-concat
        CHECK("foo"_xsd_string .concat("bar"_xsd_string) == "foobar"_xsd_string);
        CHECK(Literal{"foo", "en"}.concat(Literal{"bar", "en"}) == Literal{"foobar", "en"});
        CHECK(Literal{"foo", "en"}.concat("bar"_xsd_string) == "foobar"_xsd_string);

        CHECK(Literal{"foo", "fr"}.concat(Literal{"bar", "en"}) == "foobar"_xsd_string);
        CHECK(5_xsd_int .concat(" + "_xsd_string).concat(1.0_xsd_double).concat(Literal{" = ", "en"}).concat("6.0"_xsd_decimal) == "5 + 1.0E0 = 6.0"_xsd_string);
    }

    SUBCASE("regex_match") {
        // from https://www.w3.org/TR/xpath-functions/#func-matches
        CHECK("abracadabra"_xsd_string .regex_match("bra"_xsd_string).ebv());
        CHECK("abracadabra"_xsd_string .regex_match("^a.*a$"_xsd_string).ebv());
        CHECK("abracadabra"_xsd_string .regex_match("^bra"_xsd_string).ebv() == util::TriBool::False);

        std::string_view const poem = "<poem author=\"Wilhelm Busch\">\n"
                                      "Kaum hat dies der Hahn gesehen,\n"
                                      "Fängt er auch schon an zu krähen:\n"
                                      "Kikeriki! Kikikerikih!!\n"
                                      "Tak, tak, tak! - da kommen sie.\n"
                                      "</poem>";
        Literal const poem_lit{poem};

        CHECK(poem_lit.regex_match("Kaum.*krähen"_xsd_string).ebv() == util::TriBool::False);
        //CHECK(poem_lit.regex_match("^Kaum.*gesehen,$"_xsd_string, "m"_xsd_string).ebv()); TODO: support multiline flag
        CHECK(poem_lit.regex_match("^Kaum.*gesehen,$"_xsd_string).ebv() == util::TriBool::False);
        CHECK(poem_lit.regex_match("kiki"_xsd_string, "i"_xsd_string).ebv());

        // check lang tag behaviour
        CHECK(Literal{"abcd", "en"}.regex_match("b"_xsd_string).ebv());
        CHECK(Literal{"abcd", "en"}.regex_match(Literal{"b", "en"}).ebv());
        CHECK(Literal{"abcd", "en"}.regex_match(Literal{"b", "fr"}).null());
    }

    SUBCASE("regex_replace") {
        // from https://www.w3.org/TR/sparql11-query/#func-replace
        CHECK("abcd"_xsd_string .regex_replace("b"_xsd_string, "Z"_xsd_string) == "aZcd"_xsd_string);
        CHECK("abab"_xsd_string .regex_replace("B"_xsd_string, "Z"_xsd_string, "i"_xsd_string) == "aZaZ"_xsd_string);
        CHECK("abab"_xsd_string .regex_replace("B."_xsd_string, "Z"_xsd_string, "i"_xsd_string) == "aZb"_xsd_string);

        // from https://www.w3.org/TR/xpath-functions/#func-replace
        CHECK("abracadabra"_xsd_string .regex_replace("bra"_xsd_string, "*"_xsd_string) == "a*cada*"_xsd_string);
        CHECK("abracadabra"_xsd_string .regex_replace("a.*a"_xsd_string, "*"_xsd_string) == "*"_xsd_string);
        CHECK("abracadabra"_xsd_string .regex_replace("a.*?a"_xsd_string, "*"_xsd_string) == "*c*bra"_xsd_string);
        CHECK("abracadabra"_xsd_string .regex_replace("a"_xsd_string, ""_xsd_string) == "brcdbr"_xsd_string);
        CHECK("abracadabra"_xsd_string .regex_replace("a(.)"_xsd_string, "a$1$1"_xsd_string) == "abbraccaddabbra"_xsd_string);
        CHECK("AAAA"_xsd_string .regex_replace("A+"_xsd_string, "b"_xsd_string) == "b"_xsd_string);
        CHECK("AAAA"_xsd_string .regex_replace("A+?"_xsd_string, "b"_xsd_string) == "bbbb"_xsd_string);
        CHECK("darted"_xsd_string .regex_replace("^(.*?)d(.*)$"_xsd_string, "$1c$2"_xsd_string) == "carted"_xsd_string);

        // 'The expression fn:replace("abracadabra", ".*?", "$1") raises an error, because the pattern matches the zero-length string'
        // TODO: figure out how implement correct behaviour here (currently returns ""^^xsd:string)
        //CHECK("abracadabra"_xsd_string .regex_replace(".*?"_xsd_string, "$1"_xsd_string).null());

        CHECK("abcd"_xsd_string .regex_match(".*"_xsd_string, "q"_xsd_string).ebv() == util::TriBool::False);
        CHECK("Mr. B. Obama"_xsd_string .regex_match("B. OBAMA"_xsd_string, "qi"_xsd_string).ebv());

        // check lang tag behaviour
        CHECK(Literal{"abcd", "en"}.regex_replace("b"_xsd_string, "Z"_xsd_string) == Literal{"aZcd", "en"});
        CHECK(Literal{"abcd", "en"}.regex_replace(Literal{"b", "en"}, "Z"_xsd_string) == Literal{"aZcd", "en"});
        CHECK(Literal{"abcd", "en"}.regex_replace(Literal{"b", "fr"}, "Z"_xsd_string).null());
    }
}
