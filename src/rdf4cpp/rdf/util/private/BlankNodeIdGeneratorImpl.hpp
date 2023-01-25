#ifndef RDF4CPP_RDF_UTIL_PRIVATE_BLANKNODEIDGENERATOR_HPP
#define RDF4CPP_RDF_UTIL_PRIVATE_BLANKNODEIDGENERATOR_HPP

#include <rdf4cpp/rdf/util/BlankNodeIdManager.hpp>
#include <random>

namespace rdf4cpp::rdf::util {

struct BlankNodeIdGenerator::Impl {
    std::default_random_engine rng;
    std::uniform_int_distribution<size_t> dist;

    explicit Impl(uint64_t seed);

    void reseed(uint64_t seed);
    [[nodiscard]] std::string generate_id();
};

}  //namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_RDF_UTIL_PRIVATE_BLANKNODEIDGENERATOR_HPP
