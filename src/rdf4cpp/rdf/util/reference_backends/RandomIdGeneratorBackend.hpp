#ifndef RDF4CPP_RDF_UTIL_REFERENCEBACKENDS_RANDOMBLANKNODEIDGENERATOR_HPP
#define RDF4CPP_RDF_UTIL_REFERENCEBACKENDS_RANDOMBLANKNODEIDGENERATOR_HPP

#include <rdf4cpp/rdf/util/IBlankNodeIdGeneratorBackend.hpp>

#include <random>
#include <mutex>

namespace rdf4cpp::rdf::util {

struct RandomBlankNodeIdGenerator : IBlankNodeIdGeneratorBackend {
private:
    std::mutex mutable mutex;
    std::mt19937_64 rng;
    std::uniform_int_distribution<size_t> dist;

    char next_char();
public:
    RandomBlankNodeIdGenerator();
    explicit RandomBlankNodeIdGenerator(uint64_t seed);

    [[nodiscard]] size_t max_generated_id_size() const noexcept override;
    void generate_to_string(std::string &buf) override;
    char *generate_to_buf(char *buf) override;
};

}  //namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_RDF_UTIL_REFERENCEBACKENDS_RANDOMBLANKNODEIDGENERATOR_HPP
