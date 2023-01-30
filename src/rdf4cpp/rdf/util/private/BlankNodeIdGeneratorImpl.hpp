#ifndef RDF4CPP_RDF_UTIL_PRIVATE_BLANKNODEIDGENERATOR_HPP
#define RDF4CPP_RDF_UTIL_PRIVATE_BLANKNODEIDGENERATOR_HPP

#include <rdf4cpp/rdf/util/BlankNodeIdGenerator.hpp>

#include <random>
#include <mutex>

namespace rdf4cpp::rdf::util {

struct BlankNodeIdGenerator::Impl {
    static constexpr size_t generated_id_len = 32;

private:
    std::mutex mutable mutex;
    std::mt19937_64 rng;
    std::uniform_int_distribution<size_t> dist;

    char next_char();
public:
    Impl();
    explicit Impl(uint64_t seed);

    void generate_to_string(std::string &buf);
    char *generate_to_buf(char *buf);
};

}  //namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_RDF_UTIL_PRIVATE_BLANKNODEIDGENERATOR_HPP
