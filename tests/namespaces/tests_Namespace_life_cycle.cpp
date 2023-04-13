#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Namespace life cycle") {
    using namespace rdf4cpp::rdf;

    [[maybe_unused]] IRI iri0{"http://first.iri/"};
    // default node storage is initialized now

    {
        auto volatile_node_storage = storage::node::NodeStorage::new_instance();
        REQUIRE(1 == volatile_node_storage.id().value);
        auto &rdf = namespaces::RDF::instance(volatile_node_storage);
        auto rdf_property = rdf + "Property";
        assert(rdf_property.identifier() == std::string{namespaces::RDF::NAMESPACE} + "Property");
    }
    // volatile_node_storage and namespaces::RDF instance for node storage with node_storage_id 1 are destroyed


    {
        auto volatile_node_storage2 = storage::node::NodeStorage::new_instance();
        // volatile_node_storage2 should be able to have the same node_storage_id without any problem
        REQUIRE(volatile_node_storage2.id().value == 1);
        auto &rdf = namespaces::RDF::instance(volatile_node_storage2);
        // the NodeID used by Bag
        const IRI &rdf_bag = rdf + "Bag";
        REQUIRE(rdf_bag.identifier() == std::string{namespaces::RDF::NAMESPACE} + "Bag");
    }

    {
        auto volatile_node_storage3 = storage::node::NodeStorage::new_instance();
        auto &rdf = namespaces::RDF::instance(volatile_node_storage3);

        volatile_node_storage3 = storage::node::NodeStorage::new_instance();

        CHECK_THROWS(rdf + "Bag");
    }
}