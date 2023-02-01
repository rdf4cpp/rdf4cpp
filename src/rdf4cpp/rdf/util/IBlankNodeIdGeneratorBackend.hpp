#ifndef RDF4CPP_RDF_UTIL_IBLANKNODEIDGENERATORBACKEND
#define RDF4CPP_RDF_UTIL_IBLANKNODEIDGENERATORBACKEND

#include <string>
#include <cstddef>

namespace rdf4cpp::rdf::util {

struct IBlankNodeIdGeneratorBackend {
    virtual ~IBlankNodeIdGeneratorBackend() = default;

    [[nodiscard]] virtual size_t max_generated_id_size() const noexcept = 0;
    virtual void generate_to_string(std::string &buf) = 0;
    virtual char *generate_to_buf(char *buf) = 0;
};

}  // namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_RDF_UTIL_IBLANKNODEIDGENERATORBACKEND
