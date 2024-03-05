#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <rdf4cpp.hpp>
#include <rdf4cpp/storage/reference_node_storage/SyncReferenceNodeStorage.hpp>

using namespace rdf4cpp;

TEST_CASE("Namespace sanity check") {
    using namespace rdf4cpp;

    SUBCASE("closed NS") {
        storage::reference_node_storage::SyncReferenceNodeStorage ns;
        namespaces::RDF rdf{ns};

        auto rdf_property = rdf + "Property";
        CHECK_EQ(rdf_property.identifier(), std::string{namespaces::RDF::prefix} + "Property");

        CHECK_THROWS(rdf + "AAAAAAAAAAAA");
    }

    SUBCASE("open NS") {
        storage::reference_node_storage::SyncReferenceNodeStorage ns;
        namespaces::XSD xsd{ns};

        CHECK_EQ((xsd + "AAAAAAAAA").identifier(), std::string{xsd.prefix} + "AAAAAAAAA"); // create
        CHECK_EQ((xsd + "AAAAAAAAA").identifier(), std::string{xsd.prefix} + "AAAAAAAAA"); // fetch
    }
}
