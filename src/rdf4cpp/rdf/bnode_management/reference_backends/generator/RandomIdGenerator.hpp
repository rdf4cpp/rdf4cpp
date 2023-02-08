#ifndef RDF4CPP_RDF_UTIL_REFERENCEBACKENDS_RANDOMBLANKNODEIDGENERATOR_HPP
#define RDF4CPP_RDF_UTIL_REFERENCEBACKENDS_RANDOMBLANKNODEIDGENERATOR_HPP

#include <rdf4cpp/rdf/bnode_management/IIdGenerator.hpp>

#include <random>
#include <mutex>

namespace rdf4cpp::rdf::util {

/**
 * Generates 32-char long random ids consisting of [0-9a-z]
 */
struct RandomIdGenerator : IIdGenerator {
private:
    std::mutex mutable mutex;
    std::mt19937_64 rng;
    std::uniform_int_distribution<size_t> dist;

    char next_char();
public:
    /**
     * Creates a generator by seeding it with os entropy
     */
    RandomIdGenerator();

    /**
     * Creates a generator by seeding it with the given seed
     * @param seed seed for the random generator
     */
    explicit RandomIdGenerator(uint64_t seed);

    [[nodiscard]] size_t max_generated_id_size() const noexcept override;
    char *generate_to_buf(char *buf) override;
};

}  //namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_RDF_UTIL_REFERENCEBACKENDS_RANDOMBLANKNODEIDGENERATOR_HPP
