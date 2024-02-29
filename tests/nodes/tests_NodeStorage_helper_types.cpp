#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/SyncReferenceNodeStorageBackend.hpp>

TEST_SUITE("NodeStorage helper types") {
    using namespace rdf4cpp::rdf::storage::node::reference_node_storage::detail;

    TEST_CASE("IndexFreeList") {
        IndexFreeList<> freelist;
        CHECK_EQ(freelist.occupy_next_available(), 0);
        CHECK_EQ(freelist.occupy_next_available(), 1);

        freelist.occupy_until(2); // noop
        CHECK_EQ(freelist.occupy_next_available(), 2);

        freelist.occupy_until(64); // first word occupied
        CHECK_EQ(freelist.occupy_next_available(), 64);

        freelist.occupy_until(280); // occupy multiple words at once
        CHECK_EQ(freelist.occupy_next_available(), 280);


        freelist.vacate(0);
        freelist.vacate(3);

        CHECK_EQ(freelist.occupy_next_available(), 0);

        freelist.vacate(4);
        freelist.vacate(63);
        freelist.vacate(64);
        freelist.vacate(185);

        CHECK_EQ(freelist.occupy_next_available(), 3);
        CHECK_EQ(freelist.occupy_next_available(), 4);
        CHECK_EQ(freelist.occupy_next_available(), 63);
        CHECK_EQ(freelist.occupy_next_available(), 64);
        CHECK_EQ(freelist.occupy_next_available(), 185);

        freelist.shrink_to_fit();
        CHECK_EQ(freelist.occupy_next_available(), 281);
    }

    TEST_CASE("integration test") {
        using namespace rdf4cpp::rdf;
        using namespace rdf4cpp::rdf::storage::node;

        NodeStorage ns = NodeStorage::new_instance();

        auto l1 = Literal::make_simple("Spherical Cow", ns);
        auto l2 = Literal::make_simple("Spherical Cow", ns);

        CHECK_EQ(l1, l2);
        CHECK_EQ(l1.backend_handle(), l2.backend_handle());
        CHECK_EQ(l1.backend_handle().node_id().literal_id().to_underlying(), 1);
        CHECK_EQ(l1.lexical_form(), "Spherical Cow");
        CHECK_EQ(l1.lexical_form(), "Spherical Cow");

        auto l3 = Literal::make_simple("Hello World", ns);
        CHECK_NE(l1, l3);
        CHECK_NE(l2, l3);
        CHECK_EQ(l3.backend_handle().node_id().literal_id().to_underlying(), 2);
        CHECK_EQ(l3.lexical_form(), "Hello World");


        CHECK(ns.erase_literal(l1.backend_handle().node_id()));

        try {
            auto dummy = ns.find_literal_backend_view(l1.backend_handle().node_id());
            FAIL("Did not throw exception");
        } catch (...) {
            // expecting exception
        }

        auto l4 = Literal::make_simple("Hello World", ns);
        CHECK_EQ(l3, l4);
        CHECK_EQ(l3.backend_handle(), l4.backend_handle());
        CHECK_EQ(l4.lexical_form(), "Hello World");

        auto l5 = Literal::make_simple("Not yet named", ns);
        CHECK_NE(l3, l5);
        CHECK_EQ(l5.backend_handle().node_id().literal_id().to_underlying(), 1);
        CHECK_EQ(l5.lexical_form(), "Not yet named");

        auto l6 = Literal::make_typed_from_value<datatypes::xsd::Long>(std::numeric_limits<datatypes::xsd::Long::cpp_type>::max(), ns); // different storage
        CHECK_EQ(l6.backend_handle().node_id().literal_id().to_underlying(), 1);
    }
}
