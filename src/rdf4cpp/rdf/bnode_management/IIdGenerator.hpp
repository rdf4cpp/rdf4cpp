#ifndef RDF4CPP_RDF_UTIL_IBLANKNODEIDGENERATORBACKEND
#define RDF4CPP_RDF_UTIL_IBLANKNODEIDGENERATORBACKEND

#include <string>
#include <cstddef>

namespace rdf4cpp::rdf::util {

struct IIdGenerator {
    virtual ~IIdGenerator() = default;

    /**
     * @return the _maximum_ id length this generator will generate
     * @warning this has to be an actual upper bound as it is used to determine buffer sizes
     */
    [[nodiscard]] virtual size_t max_generated_id_size() const noexcept = 0;

    /**
     * Generates an id into the given buffer
     *
     * @param buf the buffer to generate into
     * @return the one-past-the-end pointer of the characters written
     *
     * @note implementors of this interface should assume that the provided buffer is large enough
     *      to fit _any_ id this generator may generate (i.e. having at least max_generated_id_size() in length)
     * @note the resulting buffer will _not_ be null terminated
     */
    virtual char *generate_to_buf(char *buf) = 0;
};

}  // namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_RDF_UTIL_IBLANKNODEIDGENERATORBACKEND
