#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/SyncReferenceNodeStorageBackend.hpp>

#include <atomic>
#include <shared_mutex>
#include <mutex>
#include <chrono>
#include <iostream>
#include <thread>

using namespace rdf4cpp::rdf;
using namespace rdf4cpp::rdf::storage::node;
using namespace std::chrono_literals;

struct SlowDestructingBackend : reference_node_storage::SyncReferenceNodeStorageBackend {
    std::mutex m;

    ~SlowDestructingBackend() override {
        REQUIRE(m.try_lock()); // if this doesnt work another thread got here first, and we are still executing this => error
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
            NodeStorage::set_default_instance(NodeStorage::new_instance());

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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#endif
            ns_copy = ns_copy;  // expecting noop
            CHECK(ns.ref_count() == 3);

            ns_copy = std::move(ns_copy);  // expecting noop
            CHECK(ns.ref_count() == 3);
#pragma GCC diagnostic pop

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
        CHECK(iri.backend_handle().node_id() == iri_id2);
    }

    /* the following functions are by nature non-deterministic, but they at least _attempt_ to detect race-conditions */

    TEST_CASE("destruction race") {
        for (size_t i = 0; i < 100; ++i) {
            std::atomic<bool> run{false};
            NodeStorage ns = NodeStorage::new_instance<SlowDestructingBackend>();

            auto t1 = std::jthread([&]() {
                NodeStorage tn = ns;
                while (!run.load(std::memory_order_acquire)) {} // spin before destruction
            });

            auto t2 = std::jthread([&]() {
                NodeStorage tn = ns;
                while (!run.load(std::memory_order_acquire)) {} // spin before destruction
            });

            std::this_thread::sleep_for(50ms); // wait until threads run into lock
            run.store(true, std::memory_order_release); // release threads
        }
    }

    TEST_CASE("allocation race") {
        for (size_t i = 0; i < 100; ++i) {
            std::atomic<bool> run{false};

            uint16_t id1 = -1;
            uint16_t id2 = -1;

            {
                auto t1 = std::jthread([&]() {
                    auto backend = new reference_node_storage::SyncReferenceNodeStorageBackend{};

                    while (!run.load(std::memory_order_acquire)) {} // spin until told not to

                    NodeStorage ns = NodeStorage::register_backend(backend);
                    id1 = ns.id().value;
                    std::this_thread::sleep_for(50ms);
                });

                auto t2 = std::jthread([&]() {
                    auto backend = new reference_node_storage::SyncReferenceNodeStorageBackend{};

                    while (!run.load(std::memory_order_acquire)) {} // spin until told not to

                    NodeStorage ns = NodeStorage::register_backend(backend);
                    id2 = ns.id().value;
                    std::this_thread::sleep_for(50ms);
                });

                std::this_thread::sleep_for(50ms);  // wait until threads run into lock
                run.store(true, std::memory_order_release); // release threads
            } // wait for join

            REQUIRE(id1 != id2);
            std::cout << "Constructed " << id1 << " " << id2 << std::endl;
        }
    }

    TEST_CASE("destruction and upgrade race") {
        for (size_t i = 0; i < 100; ++i) {
            std::atomic<bool> run{false};

            NodeStorage ns = NodeStorage::new_instance();
            WeakNodeStorage weak = ns.downgrade();

            auto t1 = std::jthread([&]() {
                while (!run.load(std::memory_order_acquire)) {}
                auto ns = weak.try_upgrade(); // trying to trigger asserts in try_upgrade
            });

            auto t2 = std::jthread([&]() {
                while (!run.load(std::memory_order_acquire)) {}
                ns.~NodeStorage();
            });

            std::this_thread::sleep_for(50ms);
            run.store(true, std::memory_order_release);
        }
    }

    TEST_CASE("destruction and lookup race") {
        for (size_t i = 0; i < 100; ++i) {
            std::atomic<bool> run{false};

            NodeStorage ns = NodeStorage::new_instance();
            WeakNodeStorage weak = ns.downgrade();
            auto const id = ns.id();

            auto t1 = std::jthread([&]() {
                while (!run.load(std::memory_order_acquire)) {}
                auto ns = NodeStorage::lookup_instance(id);

                if (ns.has_value()) {
                    REQUIRE(ns->ref_count() > 0);
                    REQUIRE(weak.try_upgrade().has_value()); // weak referring to node storage must still be valid else ns is not valid
                }
            });

            auto t2 = std::jthread([&]() {
                while (!run.load(std::memory_order_acquire)) {}
                ns.~NodeStorage();
            });

            std::this_thread::sleep_for(50ms);
            run.store(true, std::memory_order_release);
        }
    }

    TEST_CASE("no default instance overwrite") {
        auto inst = NodeStorage::new_instance();
        NodeStorage::set_default_instance(inst);
        CHECK(NodeStorage::default_instance().id() == inst.id());
    }
}
