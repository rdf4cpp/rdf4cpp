#include "ParsingError.hpp"

namespace rdf4cpp {
    ParsingError::ParsingError(std::string const &msg)
        : runtime_error(msg) {
    }
    ParsingError::ParsingError(const char *msg)
        : runtime_error(msg) {
    }
}  // namespace rdf4cpp