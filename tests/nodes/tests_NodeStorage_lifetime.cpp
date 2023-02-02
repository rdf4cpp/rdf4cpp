#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

#include <chrono>
#include <iostream>
#include <thread>

using namespace rdf4cpp::rdf;
using namespace rdf4cpp::rdf::storage::node;

struct SlowDestructingBackend : reference_node_storage::ReferenceNodeStorageBackend {
    std::mutex m;

    ~SlowDestructingBackend() override {
        std::cout << "begin destruction in " << std::this_thread::get_id() << std::endl;
        CHECK(m.try_lock()); // if this doesnt work another thread got here first, and we are still executing this => error
        std::this_thread::sleep_for(std::chrono::milliseconds{200});
    }
};

TEST_SUITE("NodeStorage lifetime and ref counting") {
    TEST_CASE("default_instance refcount") {
        NodeStorage &dns = NodeStorage::default_instance();
        CHECK(dns.ref_count() == 1);

        {
            NodeStorage dns_copy = dns;
            CHECK(dns.ref_count() == 2);
        }

        CHECK(dns.ref_count() == 1);

        {
            NodeStorage dns_copy = dns;
            NodeStorage moved = std::move(dns_copy);

            CHECK(dns.ref_count() == 2);
        }

        CHECK(dns.ref_count() == 1);


        identifier::NodeStorageID id;
        {
            NodeStorage dns_copy = dns;
            id = dns_copy.id();
            NodeStorage::default_instance(NodeStorage::new_instance());

            CHECK(dns_copy.ref_count() == 1);
            CHECK(dns.ref_count() == 1);
        }

        CHECK(dns.ref_count() == 1);
        CHECK(!NodeStorage::lookup_instance(id).has_value()); // old default should have died
    }

    TEST_CASE("ephemeral node storages") {
        SUBCASE("same backend") {
            NodeStorage ns = NodeStorage::new_instance();
            CHECK(ns.ref_count() == 1);

            NodeStorage ns_copy = ns;
            CHECK(ns.ref_count() == 2);

            NodeStorage ns_copy2 = ns;
            CHECK(ns.ref_count() == 3);

            ns_copy = ns_copy;  // expecting noop
            CHECK(ns.ref_count() == 3);

            ns_copy = std::move(ns_copy);  // expecting noop
            CHECK(ns.ref_count() == 3);

            ns_copy = ns_copy2;
            CHECK(ns.ref_count() == 3);

            ns_copy = std::move(ns_copy2);
            CHECK(ns.ref_count() == 2);
        }

        SUBCASE("different backends") {
            NodeStorage ns = NodeStorage::new_instance();
            NodeStorage ns2 = NodeStorage::new_instance();
            NodeStorage ns3 = NodeStorage::new_instance();

            CHECK(ns.id() != ns2.id());
            CHECK(ns2.id() != ns3.id());
            CHECK(ns.id() != ns3.id());
            CHECK(ns.ref_count() == 1);
            CHECK(ns2.ref_count() == 1);
            CHECK(ns3.ref_count() == 1);

            {
                NodeStorage ns4 = ns3;
                CHECK(ns3.ref_count() == 2);
            }
            CHECK(ns3.ref_count() == 1);

            {
                NodeStorage ns4 = std::move(ns3);
                CHECK(ns4.ref_count() == 1);
            }
            CHECK(ns3.ref_count() == 0);

            ns3 = std::move(ns2);
            CHECK(ns3.ref_count() == 1);

            ns2 = ns;
            CHECK(ns.ref_count() == 2);
            CHECK(ns2.ref_count() == 2);
        }
    }

    TEST_CASE("weak interaction") {
        SUBCASE("expecting dangle") {
            WeakNodeStorage weak = []() {
                NodeStorage ns = NodeStorage::new_instance();
                return ns.downgrade();
            }();

            CHECK(!weak.try_upgrade().has_value());

            NodeStorage dummy = NodeStorage::default_instance();
            CHECK_THROWS(dummy = weak.upgrade());
        }

        SUBCASE("dangle on replacement") {
            NodeStorage ns = NodeStorage::new_instance();
            WeakNodeStorage weak = ns.downgrade();

            auto old_id = ns.id();
            ns = NodeStorage::new_instance();

            CHECK(ns.id() != old_id); // creating the new one first then move assignment
            CHECK(!weak.try_upgrade().has_value());
        }

        SUBCASE("valid use") {
            NodeStorage ns = NodeStorage::new_instance();
            CHECK(ns.ref_count() == 1);

            {
                WeakNodeStorage weak = ns.downgrade();
                CHECK(ns.ref_count() == 1);

                {
                    NodeStorage upgraded = weak.upgrade();
                    CHECK(ns.ref_count() == 2);
                    CHECK(ns.id() == upgraded.id());
                }

                CHECK(ns.ref_count() == 1);
            }

            CHECK(ns.ref_count() == 1);
        }
    }

    TEST_CASE("nodes") {
        NodeStorage ns = NodeStorage::new_instance();
        IRI iri{"https://hello.com", ns};

        CHECK(iri.identifier() == "https://hello.com");
        ns.erase_iri(iri.backend_handle().node_id());

        identifier::NodeID iri_id2 = ns.find_or_make_id(view::IRIBackendView {.identifier = "https://hello.com"});
        CHECK(iri.backend_handle().node_id() != iri_id2);
    }

    TEST_CASE("destruction race") {
        using namespace std::chrono_literals;

        for (size_t i = 0; i < 10; ++i) {
            NodeStorage ns = NodeStorage::new_instance<SlowDestructingBackend>();

            auto t1 = std::jthread([&]() {
                NodeStorage tn = ns;
                std::this_thread::sleep_for(200ms);
            });

            auto t2 = std::jthread([&]() {
                NodeStorage tn = ns;
                std::this_thread::sleep_for(200ms);
            });

            ns = NodeStorage::new_instance();
        }
    }

    TEST_CASE("allocation race") {

    }
}
