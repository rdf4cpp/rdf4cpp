#ifndef RDF4CPP_RDF_UTIL_REFERENCEBACKENDS_INCREASINGBLANKNODEIDGENERATOR_HPP
#define RDF4CPP_RDF_UTIL_REFERENCEBACKENDS_INCREASINGBLANKNODEIDGENERATOR_HPP

#include <rdf4cpp/rdf/util/IBlankNodeIdGeneratorBackend.hpp>

#include <atomic>
#include <string>

namespace rdf4cpp::rdf::util {

struct IncreasingIdGeneratorBackend : IBlankNodeIdGeneratorBackend {
private:
    std::string prefix;
    std::atomic<size_t> counter;

public:
    IncreasingIdGeneratorBackend() noexcept;
    explicit IncreasingIdGeneratorBackend(size_t initial_value, std::string prefix = "");

    [[nodiscard]] size_t max_generated_id_size() const noexcept override;
    void generate_to_string(std::string &buf) override;
    char *generate_to_buf(char *buf) override;
};

}  //namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_RDF_UTIL_REFERENCEBACKENDS_INCREASINGBLANKNODEIDGENERATOR_HPP
