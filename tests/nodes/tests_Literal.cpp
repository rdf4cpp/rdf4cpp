#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

#include <thread>

using namespace rdf4cpp::rdf;

TEST_CASE("Literal - Check for only lexical form") {

    auto iri = IRI{"http://www.w3.org/2001/XMLSchema#string"};
    auto lit1 = Literal::make_simple("Bunny");

    CHECK(not lit1.is_blank_node());
    CHECK(lit1.is_literal());
    CHECK(not lit1.is_variable());
    CHECK(not lit1.is_iri());
    CHECK(lit1.lexical_form() == "Bunny");
    CHECK(lit1.datatype() == iri);
    CHECK(lit1.language_tag() == "");
    CHECK(std::string(lit1) == "\"Bunny\"^^<http://www.w3.org/2001/XMLSchema#string>");
}

TEST_CASE("Literal - Check for lexical form with IRI") {

    SUBCASE("string datatype") {
        auto iri = IRI{"http://www.w3.org/2001/XMLSchema#string"};
        auto lit1 = Literal::make_typed("Bunny", iri);

        CHECK(not lit1.is_blank_node());
        CHECK(lit1.is_literal());
        CHECK(not lit1.is_variable());
        CHECK(not lit1.is_iri());
        CHECK(lit1.lexical_form() == "Bunny");
        CHECK(lit1.datatype() == iri);
        CHECK(lit1.language_tag() == "");
        CHECK(std::string(lit1) == "\"Bunny\"^^<http://www.w3.org/2001/XMLSchema#string>");
    }
    SUBCASE("int datatype") {
        auto iri = IRI{"http://www.w3.org/2001/XMLSchema#int"};
        auto lit1 = Literal::make_typed("101", iri);

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
        auto lit1 = Literal::make_typed("2021-11-21", iri);

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
        auto lit1 = Literal::make_typed("2.0", iri);

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
        auto lit1 = Literal::make_typed("true", iri);

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
        auto lit1 = Literal::make_typed("false", iri);

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
        auto lit1 = Literal::make_typed("0", iri);

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
        auto lit1 = Literal::make_typed("1", iri);

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
    auto lit1 = Literal::make_lang_tagged("Bunny", "en");

    CHECK(not lit1.is_blank_node());
    CHECK(lit1.is_literal());
    CHECK(not lit1.is_variable());
    CHECK(not lit1.is_iri());
    CHECK(lit1.lexical_form() == "Bunny");
    CHECK(lit1.datatype() == iri);
    CHECK(lit1.language_tag() == "en");
    CHECK(std::string(lit1) == "\"Bunny\"@en");
}

TEST_CASE("Literal - ctor edge-case") {
    IRI const iri{"http://www.w3.org/2001/XMLSchema#int"};
    auto const lit1 = Literal::make_typed("1", iri);
    auto const lit2 = Literal::make_typed("2", iri);

    auto const expected = Literal::make_typed("3", iri);
    CHECK(lit1 + lit2 == expected);
}

TEST_CASE("Literal - check fixed id") {
    IRI const iri{datatypes::registry::xsd_string};
    auto const lit = Literal::make_typed("hello", iri);

    CHECK(lit.backend_handle().node_id().literal_type().is_fixed());
    CHECK(lit.datatype().backend_handle().node_id().value() < datatypes::registry::min_dynamic_datatype_id);
    CHECK(iri.backend_handle().node_id().value() < datatypes::registry::min_dynamic_datatype_id);
}

TEST_CASE("Literal - casting") {
    using namespace datatypes::xsd;

    auto const lit1 = Literal::make_typed_from_value<datatypes::xsd::Int>(123);

    SUBCASE("id cast") {
        auto const lit1 = Literal::make_typed_from_value<String>("hello");
        auto const lit2 = lit1.template cast<String>();

        CHECK(lit2.datatype() == IRI{String::identifier});
        CHECK(lit2.template value<String>() == "hello");
    }

    SUBCASE("str -> any") {
        auto const lit1 = Literal::make_typed_from_value<String>("1.2");
        auto const lit2 = lit1.template cast<Float>();

        CHECK(lit2.datatype() == IRI{Float::identifier});
        CHECK(lit2.value<Float>() == 1.2f);
    }

    SUBCASE("str -> boolean") {
        SUBCASE("word-form") {
            auto const lit1 = Literal::make_typed_from_value<String>("true");
            auto const lit2 = lit1.template cast<Boolean>();

            CHECK(lit2.datatype() == IRI{Boolean::identifier});
            CHECK(lit2.value<Boolean>());
        }

        SUBCASE("numeric form") {
            auto const lit1 = Literal::make_typed_from_value<String>("1");
            auto const lit2 = lit1.template cast<Boolean>();

            CHECK(lit2.datatype() == IRI{Boolean::identifier});
            CHECK(lit2.value<Boolean>());
        }
    }

    SUBCASE("any -> str") {
        SUBCASE("decimal") {
            SUBCASE("integral") {
                auto const lit1 = Literal::make_typed_from_value<Decimal>(Decimal::cpp_type{"1005.0"});
                auto const lit2 = lit1.cast<String>();

                CHECK(lit2.value<String>() == "1005");
            }

            SUBCASE("non-integral") {
                auto const lit1 = Literal::make_typed_from_value<Decimal>(1.5);
                auto const lit2 = lit1.template cast<String>();

                CHECK(lit2.template value<String>() == "1.5");
            }
        }

        SUBCASE("float") {
            SUBCASE("fixed notation - non-integral") {
                auto const lit1 = Literal::make_typed_from_value<Float>(10.5f);
                auto const lit2 = lit1.template cast<String>();

                CHECK(lit2.template value<String>() == "10.5");
            }

            SUBCASE("fixed notation - integral") {
                auto const lit1 = Literal::make_typed_from_value<Float>(100000.f);
                auto const lit2 = lit1.template cast<String>();

                CHECK(lit2.template value<String>() == "100000");
            }

            SUBCASE("large - scientific") {
                auto const lit1 = Literal::make_typed_from_value<Float>(1000001.f);
                auto const lit2 = lit1.template cast<String>();

                CHECK(lit2.template value<String>() == "1.000001E6");
            }

            SUBCASE("small - scientific") {
                auto const lit1 = Literal::make_typed_from_value<Float>(0.0000009f);
                auto const lit2 = lit1.template cast<String>();

                CHECK(lit2.template value<String>() == "9.0E-7");
            }

            SUBCASE("zero") {
                auto const lit1 = Literal::make_typed_from_value<Float>(0.0f);
                auto const lit2 = lit1.template cast<String>();

                CHECK(lit2.template value<String>() == "0");
            }

            SUBCASE("minus zero") {
                auto const lit1 = Literal::make_typed_from_value<Float>(-0.0f);
                auto const lit2 = lit1.template cast<String>();

                CHECK(lit2.template value<String>() == "-0");
            }
        }

        SUBCASE("double") {
            SUBCASE("fixed notation - non-integral") {
                auto const lit1 = Literal::make_typed_from_value<Double>(10.5);
                auto const lit2 = lit1.template cast<String>();

                CHECK(lit2.template value<String>() == "10.5");
            }

            SUBCASE("fixed notation - integral") {
                auto const lit1 = Literal::make_typed_from_value<Double>(100000.0);
                auto const lit2 = lit1.template cast<String>();

                CHECK(lit2.template value<String>() == "100000");
            }

            SUBCASE("large - scientific") {
                auto const lit1 = Literal::make_typed_from_value<Double>(1000001.0);
                auto const lit2 = lit1.template cast<String>();

                CHECK(lit2.template value<String>() == "1.000001E6");
            }

            SUBCASE("small - scientific") {
                auto const lit1 = Literal::make_typed_from_value<Double>(0.0000009);
                auto const lit2 = lit1.template cast<String>();

                CHECK(lit2.template value<String>() == "9.0E-7");
            }

            SUBCASE("zero") {
                auto const lit1 = Literal::make_typed_from_value<Double>(0.0);
                auto const lit2 = lit1.template cast<String>();

                CHECK(lit2.template value<String>() == "0");
            }

            SUBCASE("minus zero") {
                auto const lit1 = Literal::make_typed_from_value<Double>(-0.0);
                auto const lit2 = lit1.template cast<String>();

                CHECK(lit2.template value<String>() == "-0");
            }
        }
    }

    SUBCASE("any -> bool") {
        auto const lit1 = Literal::make_typed_from_value<Float>(1.4);
        auto const lit2 = lit1.template cast<Boolean>();

        CHECK(lit2.datatype() == IRI{Boolean::identifier});
        CHECK(lit2.template value<Boolean>() == true);
    }

    SUBCASE("downcast: dbl -> flt") {
        auto const lit1 = Literal::make_typed_from_value<Double>(1.4);
        auto const lit2 = lit1.template cast<Float>();

        CHECK(lit2.datatype() == IRI{Float::identifier});
        CHECK(lit2.template value<Float>() == 1.4f);
    }

    SUBCASE("dec -> flt") {
        auto const lit1 = Literal::make_typed_from_value<Decimal>(1.0);
        auto const lit2 = lit1.template cast<Float>();

        CHECK(lit2.datatype() == IRI{Float::identifier});
        CHECK(lit2.template value<Float>() == 1.f);
    }

    SUBCASE("dec -> dbl") {
        auto const lit1 = Literal::make_typed_from_value<Decimal>(1.0);
        auto const lit2 = lit1.template cast<Double>();

        CHECK(lit2.datatype() == IRI{Double::identifier});
        CHECK(lit2.template value<Double>() == 1.0);
    }

    SUBCASE("dec -> int") {
        auto const lit1 = Literal::make_typed_from_value<Decimal>(1.2);
        auto const lit2 = lit1.template cast<Int>();

        CHECK(lit2.datatype() == IRI{Int::identifier});
        CHECK(lit2.template value<Int>() == 1);
    }

    SUBCASE("downcast: int -> dec") {
        auto const lit1 = Literal::make_typed_from_value<Integer>(1);
        auto const lit2 = lit1.template cast<Decimal>();

        CHECK(lit2.datatype() == IRI{Decimal::identifier});
        CHECK(lit2.template value<Decimal>() == 1);
    }

    SUBCASE("int -> flt") {
        auto const lit1 = Literal::make_typed_from_value<Integer>(1);
        auto const lit2 = lit1.template cast<Float>();

        CHECK(lit2.datatype() == IRI{Float::identifier});
        CHECK(lit2.template value<Float>() == 1.f);
    }

    SUBCASE("int -> dbl") {
        auto const lit1 = Literal::make_typed_from_value<Integer>(1);
        auto const lit2 = lit1.template cast<Double>();

        CHECK(lit2.datatype() == IRI{Double::identifier});
        CHECK(lit2.template value<Double>() == 1.0);
    }

    SUBCASE("id cast") {
        auto const lit1 = Literal::make_typed_from_value<Int>(5);
        auto const lit2 = lit1.template cast<Int>();

        CHECK(lit1 == lit2);
    }

    SUBCASE("bool -> numeric") {
        SUBCASE("integers") {
            SUBCASE("regular case") {
                auto const lit1 = Literal::make_typed_from_value<Boolean>(true);
                auto const lit2 = lit1.template cast<Byte>();
                CHECK(!lit2.null());
                CHECK(lit2.datatype() == IRI{Byte::identifier});
                CHECK(lit2.template value<Byte>() == 1);
            }

            SUBCASE("partially representable - representable case") {
                auto const lit3 = Literal::make_typed_from_value<Boolean>(false);
                auto const lit4 = lit3.template cast<NonPositiveInteger>();
                CHECK(!lit4.null());
                CHECK(lit4.datatype() == IRI{NonPositiveInteger::identifier});
                CHECK(lit4.template value<NonPositiveInteger>() == 0);
            }

            SUBCASE("partially representable - unrepresentable case") {
                auto const lit3 = Literal::make_typed_from_value<Boolean>(true);
                auto const lit4 = lit3.template cast<NegativeInteger>();
                CHECK(lit4.null());
            }
        }

        SUBCASE("decimal") {
            auto const lit1 = Literal::make_typed_from_value<Boolean>(false);
            auto const lit2 = lit1.template cast<Decimal>();
            CHECK(!lit2.null());
            CHECK(lit2.datatype() == IRI{Decimal::identifier});
            CHECK(lit2.template value<Decimal>() == 0.0);
        }

        SUBCASE("float") {
            auto const lit1 = Literal::make_typed_from_value<Boolean>(true);
            auto const lit2 = lit1.template cast<Float>();
            CHECK(!lit2.null());
            CHECK(lit2.datatype() == IRI{Float::identifier});
            CHECK(lit2.template value<Float>() == 1.f);
        }

        SUBCASE("double") {
            auto const lit1 = Literal::make_typed_from_value<Boolean>(false);
            auto const lit2 = lit1.template cast<Double>();
            CHECK(!lit2.null());
            CHECK(lit2.datatype() == IRI{Double::identifier});
            CHECK(lit2.template value<Double>() == 0.0);
        }
    }

    SUBCASE("cross hierarchy: int -> unsignedInt") {
        auto const lit1 = Literal::make_typed_from_value<Int>(1);
        auto const lit2 = lit1.template cast<UnsignedInt>();

        CHECK(lit2.datatype() == IRI{UnsignedInt::identifier});
        CHECK(lit2.value<UnsignedInt>() == 1);
    }

    SUBCASE("subtypes") {
        CHECK(lit1.template cast<Integer>().datatype() == IRI{Integer::identifier});
        CHECK(lit1.template cast<Float>().datatype() == IRI{Float::identifier});

        auto const lit2 = Literal::make_typed_from_value<Integer>(420);
        CHECK(lit2.template cast<Int>() == Literal::make_typed_from_value<Int>(420));
    }

    SUBCASE("value too large") {
        auto const lit1 = Literal::make_typed_from_value<Int>(67000);
        auto const lit2 = lit1.template cast<Short>();

        CHECK(lit2.null());
    }

    SUBCASE("negative to unsigned") {
        auto const lit1 = Literal::make_typed_from_value<Int>(-10);
        auto const lit2 = lit1.template cast<UnsignedInt>();

        CHECK(lit2.null());
    }
}

TEST_CASE("Literal - misc functions") {
    using namespace rdf4cpp::rdf;

    SUBCASE("rand") {
        SUBCASE("same thread") {
            auto const l1 = Literal::generate_random_double();
            auto const l2 = Literal::generate_random_double();

            CHECK(l1 >= 0.0_xsd_double);
            CHECK(l1 < 1.0_xsd_double);

            CHECK(l2 >= 0.0_xsd_double);
            CHECK(l2 < 1.0_xsd_double);
            CHECK(l1 != l2);  // note: non-deterministic but should basically never fail
        }

        SUBCASE("difference threads") {
            auto const l1 = Literal::generate_random_double();
            Literal l2;

            std::thread t{[&]() {
                l2 = Literal::generate_random_double();
            }};

            t.join();

            CHECK(l1 != l2);  // note: non-deterministic but should basically never fail
        }
    }

    SUBCASE("abs") {
        CHECK((-1_xsd_int).abs() == 1_xsd_integer);
        CHECK((-100.0_xsd_double).abs() == 100.0_xsd_double);
        CHECK((99.0_xsd_float).abs() == 99.0_xsd_float);
        CHECK(("hello"_xsd_string).abs().null());
    }

    SUBCASE("round") {
        CHECK((99_xsd_int).round() == 99_xsd_integer);
        CHECK((1.2_xsd_double).round() == 1.0_xsd_double);
        CHECK((1.5_xsd_double).round() == 2.0_xsd_double);
        CHECK(("hello"_xsd_string).round().null());
    }

    SUBCASE("floor") {
        CHECK((99_xsd_int).floor() == 99_xsd_integer);
        CHECK((1.2_xsd_double).floor() == 1.0_xsd_double);
        CHECK((1.5_xsd_double).floor() == 1.0_xsd_double);
        CHECK(("hello"_xsd_string).floor().null());
    }

    SUBCASE("ceil") {
        CHECK((99_xsd_int).ceil() == 99_xsd_integer);
        CHECK((1.2_xsd_double).ceil() == 2.0_xsd_double);
        CHECK((1.5_xsd_double).ceil() == 2.0_xsd_double);
        CHECK(("hello"_xsd_string).ceil().null());
    }

    SUBCASE("strlen") {
        CHECK(("12345"_xsd_string).as_strlen() == 5_xsd_integer);
        CHECK(1_xsd_int .as_strlen().null());
        CHECK(("123"_xsd_string).as_strlen() == 3_xsd_integer);
        CHECK(Literal::make_lang_tagged("hello", "en").as_strlen() == 5_xsd_integer);

        CHECK(("z\u00df\u6c34\U0001f34c"_xsd_string).as_strlen() == 4_xsd_integer);  // "zß水🍌"
    }

    SUBCASE("substr") {
        // from https://www.w3.org/TR/xpath-functions/#func-substring
        CHECK(("motor car"_xsd_string).substr(6_xsd_integer) == " car"_xsd_string);
        CHECK(("metadata"_xsd_string).substr(4_xsd_integer, 3_xsd_integer) == "ada"_xsd_string);
        CHECK(("12345"_xsd_string).substr("1.5"_xsd_decimal, "2.6"_xsd_decimal) == "234"_xsd_string);
        CHECK(("12345"_xsd_string).substr(0_xsd_integer, 3_xsd_integer) == "12"_xsd_string);
        CHECK(("12345"_xsd_string).substr(5_xsd_integer, -3_xsd_integer) == ""_xsd_string);
        CHECK(("12345"_xsd_string).substr(-3_xsd_integer, 5_xsd_integer) == "1"_xsd_string);
        CHECK(("12345"_xsd_string).substr(0_xsd_integer / 0.0_xsd_double, 3_xsd_integer) == ""_xsd_string);
        CHECK(("12345"_xsd_string).substr(1_xsd_integer, 0_xsd_integer / 0.0_xsd_double) == ""_xsd_string);
        CHECK(("12345"_xsd_string).substr(-42_xsd_integer, 1_xsd_integer / 0.0_xsd_double) == "12345"_xsd_string);

        // from https://www.w3.org/TR/sparql11-query/#func-substr
        CHECK(("foobar"_xsd_string).substr(4_xsd_integer) == "bar"_xsd_string);
        CHECK(Literal::make_lang_tagged("foobar", "en").substr(4_xsd_integer) == Literal::make_lang_tagged("bar", "en"));
        CHECK(("foobar"_xsd_string).substr(4_xsd_integer, 1_xsd_integer) == "b"_xsd_string);
        CHECK(Literal::make_lang_tagged("foobar", "en").substr(4_xsd_integer, 1_xsd_integer) == Literal::make_lang_tagged("b", "en"));

        // check correct casting
        auto const s = "Hello World"_xsd_string;
        CHECK(s.substr(2_xsd_long, -1.3_xsd_double) == ""_xsd_string);
        CHECK(s.substr(2.1_xsd_double, 3.2_xsd_double) == "ell"_xsd_string);
        CHECK(s.substr(100_xsd_integer, 10_xsd_int) == ""_xsd_string);
    }

    SUBCASE("langMatches") {
        CHECK(Literal::make_lang_tagged("Hello", "en").as_language_tag_matches_range("*"_xsd_string).ebv());
        CHECK(Literal::make_lang_tagged("Bonjour", "fr").as_language_tag_matches_range("FR"_xsd_string).ebv());
        CHECK(Literal::make_lang_tagged("Hello", "en-US").as_language_tag_matches_range("en-US"_xsd_string).ebv());
        CHECK(5_xsd_int .as_language_tag_matches_range("*"_xsd_string).null());
        CHECK(("Hello"_xsd_string).as_language_tag_matches_range(""_xsd_string).ebv());
        CHECK(("Hello"_xsd_string).as_language_tag_matches_range("*"_xsd_string).ebv() == util::TriBool::False);
    }

    SUBCASE("ucase") {
        // from https://www.w3.org/TR/sparql11-query/#func-ucase
        CHECK(("foo"_xsd_string).uppercase() == "FOO"_xsd_string);
        CHECK(Literal::make_lang_tagged("foo", "en").uppercase() == Literal::make_lang_tagged("FOO", "en"));
    }

    SUBCASE("lcase") {
        // from https://www.w3.org/TR/sparql11-query/#func-lcase
        CHECK(("BAR"_xsd_string).lowercase() == "bar"_xsd_string);
        CHECK(Literal::make_lang_tagged("BAR", "en").lowercase() == Literal::make_lang_tagged("bar", "en"));
    }

    SUBCASE("contains") {
        // from https://www.w3.org/TR/sparql11-query/#func-contains
        CHECK(("foobar"_xsd_string).as_contains("bar"_xsd_string).ebv());
        CHECK(Literal::make_lang_tagged("foobar", "en").as_contains(Literal::make_lang_tagged("foo", "en")).ebv());
        CHECK(Literal::make_lang_tagged("foobar", "en").as_contains("bar"_xsd_string).ebv());

        CHECK(Literal::make_lang_tagged("hello", "en").as_contains(Literal::make_lang_tagged("o", "fr")).null());
        CHECK(("123"_xsd_string).as_contains(Literal::make_lang_tagged("1", "en")).null());
    }

    SUBCASE("substr_before") {
        // from https://www.w3.org/TR/sparql11-query/#func-strbefore
        CHECK(("abc"_xsd_string).substr_before("b"_xsd_string) == "a"_xsd_string);
        CHECK(Literal::make_lang_tagged("abc", "en").substr_before("bc"_xsd_string) == Literal::make_lang_tagged("a", "en"));
        CHECK(Literal::make_lang_tagged("abc", "en").substr_before(Literal::make_lang_tagged("b", "cy")).null());
        CHECK(("abc"_xsd_string).substr_before(""_xsd_string) == ""_xsd_string);
        CHECK(("abc"_xsd_string).substr_before("xyz"_xsd_string) == ""_xsd_string);
        CHECK(Literal::make_lang_tagged("abc", "en").substr_before(Literal::make_lang_tagged("z", "en")) == ""_xsd_string);
        CHECK(Literal::make_lang_tagged("abc", "en").substr_before("z"_xsd_string) == ""_xsd_string);
        CHECK(Literal::make_lang_tagged("abc", "en").substr_before(Literal::make_lang_tagged("", "en")) == Literal::make_lang_tagged("", "en"));
        CHECK(Literal::make_lang_tagged("abc", "en").substr_before(""_xsd_string) == Literal::make_lang_tagged("", "en"));
    }

    SUBCASE("substr_after") {
        // from https://www.w3.org/TR/sparql11-query/#func-strafter
        CHECK(("abc"_xsd_string).substr_after("b"_xsd_string) == "c"_xsd_string);
        CHECK(Literal::make_lang_tagged("abc", "en").substr_after("ab"_xsd_string) == Literal::make_lang_tagged("c", "en"));
        CHECK(Literal::make_lang_tagged("abc", "en").substr_after(Literal::make_lang_tagged("b", "cy")).null());
        CHECK(("abc"_xsd_string).substr_after(""_xsd_string) == "abc"_xsd_string);
        CHECK(("abc"_xsd_string).substr_after("xyz"_xsd_string) == ""_xsd_string);
        CHECK(Literal::make_lang_tagged("abc", "en").substr_after(Literal::make_lang_tagged("z", "en")) == ""_xsd_string);
        CHECK(Literal::make_lang_tagged("abc", "en").substr_after("z"_xsd_string) == ""_xsd_string);
        CHECK(Literal::make_lang_tagged("abc", "en").substr_after(Literal::make_lang_tagged("", "en")) == Literal::make_lang_tagged("abc", "en"));
        CHECK(Literal::make_lang_tagged("abc", "en").substr_after(""_xsd_string) == Literal::make_lang_tagged("abc", "en"));
    }

    SUBCASE("str_start_with") {
        // from https://www.w3.org/TR/sparql11-query/#func-strstarts
        CHECK(("foobar"_xsd_string).as_str_starts_with("foo"_xsd_string).ebv());
        CHECK(Literal::make_lang_tagged("foobar", "en").as_str_starts_with(Literal::make_lang_tagged("foo", "en")).ebv());
        CHECK(Literal::make_lang_tagged("foobar", "en").as_str_starts_with("foo"_xsd_string).ebv());

        CHECK(Literal::make_lang_tagged("foobar", "fr").as_str_starts_with(Literal::make_lang_tagged("foo", "en")).null());
        CHECK(("foobar"_xsd_string).as_str_starts_with(Literal::make_lang_tagged("foo", "en")).null());
    }

    SUBCASE("str_ends_with") {
        // from https://www.w3.org/TR/sparql11-query/#func-strstarts
        CHECK(("foobar"_xsd_string).as_str_ends_with("bar"_xsd_string).ebv());
        CHECK(Literal::make_lang_tagged("foobar", "en").as_str_ends_with(Literal::make_lang_tagged("bar", "en")).ebv());
        CHECK(Literal::make_lang_tagged("foobar", "en").as_str_ends_with("bar"_xsd_string).ebv());

        CHECK(Literal::make_lang_tagged("foobar", "fr").as_str_ends_with(Literal::make_lang_tagged("bar", "en")).null());
        CHECK(("foobar"_xsd_string).as_str_ends_with(Literal::make_lang_tagged("bar", "en")).null());
    }

    SUBCASE("concat") {
        // from https://www.w3.org/TR/sparql11-query/#func-concat
        CHECK(("foo"_xsd_string).concat("bar"_xsd_string) == "foobar"_xsd_string);
        CHECK(Literal::make_lang_tagged("foo", "en").concat(Literal::make_lang_tagged("bar", "en")) == Literal::make_lang_tagged("foobar", "en"));
        CHECK(Literal::make_lang_tagged("foo", "en").concat("bar"_xsd_string) == "foobar"_xsd_string);

        CHECK(Literal::make_lang_tagged("foo", "fr").concat(Literal::make_lang_tagged("bar", "en")) == "foobar"_xsd_string);
        CHECK(5_xsd_int .concat(" + "_xsd_string).concat(1.0_xsd_double).concat(Literal::make_lang_tagged(" = ", "en")).concat("6.0"_xsd_decimal) == "5 + 1.0E0 = 6.0"_xsd_string);
    }

    SUBCASE("regex_match") {
        // from https://www.w3.org/TR/xpath-functions/#func-matches
        CHECK(("abracadabra"_xsd_string).as_regex_matches("bra"_xsd_string).ebv());
        CHECK(("abracadabra"_xsd_string).as_regex_matches("^a.*a$"_xsd_string).ebv());
        CHECK(("abracadabra"_xsd_string).as_regex_matches("^bra"_xsd_string).ebv() == util::TriBool::False);

        std::string_view const poem = "<poem author=\"Wilhelm Busch\">\n"
                                      "Kaum hat dies der Hahn gesehen,\n"
                                      "Fängt er auch schon an zu krähen:\n"
                                      "Kikeriki! Kikikerikih!!\n"
                                      "Tak, tak, tak! - da kommen sie.\n"
                                      "</poem>";
        auto const poem_lit = Literal::make_simple(poem);

        CHECK(poem_lit.as_regex_matches("Kaum.*krähen"_xsd_string).ebv() == util::TriBool::False);
        //CHECK(poem_lit.regex_match("^Kaum.*gesehen,$"_xsd_string, "m"_xsd_string).ebv()); TODO: support multiline flag
        CHECK(poem_lit.as_regex_matches("^Kaum.*gesehen,$"_xsd_string).ebv() == util::TriBool::False);
        CHECK(poem_lit.as_regex_matches("kiki"_xsd_string, "i"_xsd_string).ebv());

        // check lang tag behaviour
        CHECK(Literal::make_lang_tagged("abcd", "en").as_regex_matches("b"_xsd_string).ebv());
        CHECK(Literal::make_lang_tagged("abcd", "en").as_regex_matches(Literal::make_lang_tagged("b", "en")).ebv());
        CHECK(Literal::make_lang_tagged("abcd", "en").as_regex_matches(Literal::make_lang_tagged("b", "fr")).null());
    }

    SUBCASE("regex_replace") {
        // from https://www.w3.org/TR/sparql11-query/#func-replace
        CHECK(("abcd"_xsd_string).regex_replace("b"_xsd_string, "Z"_xsd_string) == "aZcd"_xsd_string);
        CHECK(("abab"_xsd_string).regex_replace("B"_xsd_string, "Z"_xsd_string, "i"_xsd_string) == "aZaZ"_xsd_string);
        CHECK(("abab"_xsd_string).regex_replace("B."_xsd_string, "Z"_xsd_string, "i"_xsd_string) == "aZb"_xsd_string);

        // from https://www.w3.org/TR/xpath-functions/#func-replace
        CHECK(("abracadabra"_xsd_string).regex_replace("bra"_xsd_string, "*"_xsd_string) == "a*cada*"_xsd_string);
        CHECK(("abracadabra"_xsd_string).regex_replace("a.*a"_xsd_string, "*"_xsd_string) == "*"_xsd_string);
        CHECK(("abracadabra"_xsd_string).regex_replace("a.*?a"_xsd_string, "*"_xsd_string) == "*c*bra"_xsd_string);
        CHECK(("abracadabra"_xsd_string).regex_replace("a"_xsd_string, ""_xsd_string) == "brcdbr"_xsd_string);
        CHECK(("abracadabra"_xsd_string).regex_replace("a(.)"_xsd_string, "a$1$1"_xsd_string) == "abbraccaddabbra"_xsd_string);
        CHECK(("AAAA"_xsd_string).regex_replace("A+"_xsd_string, "b"_xsd_string) == "b"_xsd_string);
        CHECK(("AAAA"_xsd_string).regex_replace("A+?"_xsd_string, "b"_xsd_string) == "bbbb"_xsd_string);
        CHECK(("darted"_xsd_string).regex_replace("^(.*?)d(.*)$"_xsd_string, "$1c$2"_xsd_string) == "carted"_xsd_string);

        // 'The expression fn:replace("abracadabra", ".*?", "$1") raises an error, because the pattern matches the zero-length string'
        // TODO: figure out how implement correct behaviour here (currently returns ""^^xsd:string)
        //CHECK(("abracadabra"_xsd_string).regex_replace(".*?"_xsd_string, "$1"_xsd_string).null());

        CHECK(("abcd"_xsd_string).as_regex_matches(".*"_xsd_string, "q"_xsd_string).ebv() == util::TriBool::False);
        CHECK(("Mr. B. Obama"_xsd_string).as_regex_matches("B. OBAMA"_xsd_string, "qi"_xsd_string).ebv());

        // check lang tag behaviour
        CHECK(Literal::make_lang_tagged("abcd", "en").regex_replace("b"_xsd_string, "Z"_xsd_string) == Literal::make_lang_tagged("aZcd", "en"));
        CHECK(Literal::make_lang_tagged("abcd", "en").regex_replace(Literal::make_lang_tagged("b", "en"), "Z"_xsd_string) == Literal::make_lang_tagged("aZcd", "en"));
        CHECK(Literal::make_lang_tagged("abcd", "en").regex_replace(Literal::make_lang_tagged("b", "fr"), "Z"_xsd_string).null());
    }
}

TEST_CASE("indirect casting precision") {
    CHECK(Literal::make_typed_from_value<datatypes::xsd::Double>(2e-1) + Literal::make_typed_from_value<datatypes::xsd::Decimal>(datatypes::xsd::Decimal::cpp_type{"0.2"}) == Literal::make_typed_from_value<datatypes::xsd::Double>(4e-1));
}

TEST_CASE("URI encoding") {
    static constexpr const char *data = "www.example %\t*}.com";
    static constexpr const char *data_encoded = "www.example%20%25%09%2A%7D.com";
    SUBCASE("valid URI") {
        static constexpr const char *str = "www.example123_-AZaz09~.com";
        CHECK(Literal::encode_for_uri(str) == Literal::make_simple(str));
    }
    SUBCASE("ASCII escapes") {
        CHECK(Literal::encode_for_uri(" %\t*}") == Literal::make_simple("%20%25%09%2A%7D"));
    }
    SUBCASE("ASCII mixed") {
        CHECK(Literal::encode_for_uri(data) == Literal::make_simple(data_encoded));
    }
    SUBCASE("high UTF-8") {
        CHECK(Literal::encode_for_uri("\xce\xa4") == Literal::make_simple("%CE%A4"));                // greek capital sigma
        CHECK(Literal::encode_for_uri("\xf0\x90\x8f\x92") == Literal::make_simple("%F0%90%8F%92"));  // old persian 2
        CHECK(Literal::encode_for_uri("\xe2\x88\x80") == Literal::make_simple("%E2%88%80"));         // math forall
    }
    SUBCASE("high UTF-8 mixed") {
        CHECK(Literal::encode_for_uri("www.e\xce\xa4\xf0\x90\x8f\x92\xe2\x88\x80xample.com") == Literal::make_simple("www.e%CE%A4%F0%90%8F%92%E2%88%80xample.com"));
    }
    SUBCASE("nonstatic") {
        CHECK(Literal::make_simple(data).encode_for_uri() == Literal::make_simple(data_encoded));
        CHECK(Literal::make_lang_tagged(data, "en").encode_for_uri() == Literal::make_simple(data_encoded));
        CHECK(Literal::make_typed(data, IRI{"http://www.w3.org/2001/XMLSchema#string"}).encode_for_uri() == Literal::make_simple(data_encoded));
    }
    SUBCASE("invalid UTF-8") {
        CHECK(Literal::encode_for_uri("\xce") == Literal{});
    }
}
