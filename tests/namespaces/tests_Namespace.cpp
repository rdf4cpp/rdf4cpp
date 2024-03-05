#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp/rdf.hpp>
#include <rdf4cpp/rdf/storage/reference_node_storage/SyncReferenceNodeStorageBackend.hpp>

using namespace rdf4cpp::rdf;

TEST_CASE("Namespace sanity check") {
    using namespace rdf4cpp::rdf;

    SUBCASE("closed NS") {
        storage::reference_node_storage::SyncReferenceNodeStorageBackend ns;
        namespaces::RDF rdf{ns};

        auto rdf_property = rdf + "Property";
        CHECK_EQ(rdf_property.identifier(), std::string{namespaces::RDF::prefix} + "Property");

        CHECK_THROWS(rdf + "AAAAAAAAAAAA");
    }

    SUBCASE("open NS") {
        storage::reference_node_storage::SyncReferenceNodeStorageBackend ns;
        namespaces::XSD xsd{ns};

        CHECK_EQ((xsd + "AAAAAAAAA").identifier(), std::string{xsd.prefix} + "AAAAAAAAA"); // create
        CHECK_EQ((xsd + "AAAAAAAAA").identifier(), std::string{xsd.prefix} + "AAAAAAAAA"); // fetch
    }
}
