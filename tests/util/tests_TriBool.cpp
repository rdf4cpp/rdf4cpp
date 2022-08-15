#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using rdf4cpp::rdf::util::TriBool;

TEST_SUITE("TriBool") {

    TEST_CASE("TriBool operators") {
        TriBool const tt = TriBool::True;
        TriBool const tf = TriBool::False;
        TriBool const te = TriBool::Err;

        SUBCASE("and") {
            CHECK((tt && tt) == TriBool::True);
            CHECK((tt && tf) == TriBool::False);
            CHECK((tf && tt) == TriBool::False);
            CHECK((tf && tf) == TriBool::False);
            CHECK((tt && te) == TriBool::Err);
            CHECK((te && tt) == TriBool::Err);
            CHECK((tf && te) == TriBool::False);
            CHECK((te && tf) == TriBool::False);
            CHECK((te && te) == TriBool::Err);
        }

        SUBCASE("or") {
            CHECK((tt || tt) == TriBool::True);
            CHECK((tt || tf) == TriBool::True);
            CHECK((tf || tt) == TriBool::True);
            CHECK((tf || tf) == TriBool::False);
            CHECK((tt || te) == TriBool::True);
            CHECK((te || tt) == TriBool::True);
            CHECK((tf || te) == TriBool::Err);
            CHECK((te || tf) == TriBool::Err);
            CHECK((te || te) == TriBool::Err);
        }

        SUBCASE("not") {
            CHECK(!tt == TriBool::False);
            CHECK(!tf == TriBool::True);
            CHECK(!te == TriBool::Err);
        }
    }


    TEST_CASE("TriBool bool interop") {
        TriBool const tt = true;
        TriBool const tf = false;
        TriBool const te = TriBool::Err;

        SUBCASE("converting constructor") {
            CHECK(tt == TriBool::True);
            CHECK(tf == TriBool::False);
        }

        SUBCASE("operator bool") {
            if (tt) {
                // expected
            } else {
                FAIL_CHECK("wrong conversion true -> bool");
            }

            if (!tf) {
                // expected
            } else {
                FAIL_CHECK("wrong conversion false -> bool");
            }

            if (te) {
                FAIL_CHECK("wrong conversion err -> bool");
            } else if (!te) {
                FAIL_CHECK("wrong conversion err -> bool");
            } else {
                // expected
            }
        }

        SUBCASE("logic operators") {
            SUBCASE("and") {
                CHECK((tt && true) == TriBool::True);
                CHECK((true && tt) == TriBool::True);
                CHECK((tt && false) == TriBool::False);
                CHECK((false && tt) == TriBool::False);

                CHECK((tf && true) == TriBool::False);
                CHECK((true && tf) == TriBool::False);
                CHECK((tf && false) == TriBool::False);
                CHECK((false && tf) == TriBool::False);

                CHECK((te && true) == TriBool::Err);
                CHECK((true && te) == TriBool::Err);
                CHECK((te && false) == TriBool::False);
                CHECK((false && te) == TriBool::False);
            }

            SUBCASE("or") {
                CHECK((tt || true) == TriBool::True);
                CHECK((true || tt) == TriBool::True);
                CHECK((tt || false) == TriBool::True);
                CHECK((false || tt) == TriBool::True);

                CHECK((tf || true) == TriBool::True);
                CHECK((true || tf) == TriBool::True);
                CHECK((tf || false) == TriBool::False);
                CHECK((false || tf) == TriBool::False);

                CHECK((te || true) == TriBool::True);
                CHECK((true || te) == TriBool::True);
                CHECK((te || false) == TriBool::Err);
                CHECK((false || te) == TriBool::Err);
            }
        }
    }
}
