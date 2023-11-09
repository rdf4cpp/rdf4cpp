#ifndef RDF4CPP_RDF_UTIL_REFERENCEBACKENDS_INCREASINGBLANKNODEIDGENERATOR_HPP
#define RDF4CPP_RDF_UTIL_REFERENCEBACKENDS_INCREASINGBLANKNODEIDGENERATOR_HPP

#include <rdf4cpp/rdf/bnode_mngt/IIdGenerator.hpp>

#include <atomic>
#include <string>

namespace rdf4cpp::rdf::bnode_mngt {

/**
 * Generates identifiers consisting of a optional prefix and an integer in increasing value.
 */
struct IncreasingIdGenerator : IIdGenerator {
private:
    std::string prefix;
    std::atomic<size_t> counter;

public:
    /**
     * Creates a generator from a given start value and a prefix
     *
     * @param prefix prefix for the generated ids
     * @param initial_value initial value of the counter
     */
    explicit IncreasingIdGenerator(std::string prefix = "", size_t initial_value = 0) noexcept;

    [[nodiscard]] size_t max_generated_id_size() const noexcept override;
    char *generate_to_buf(char *buf) override;
};

}  //namespace rdf4cpp::rdf::bnode_mngt

#endif  //RDF4CPP_RDF_UTIL_REFERENCEBACKENDS_INCREASINGBLANKNODEIDGENERATOR_HPP
