#ifndef RDF4CPP_PRIVATE_TRY_SERIALIZE_HPP
#define RDF4CPP_PRIVATE_TRY_SERIALIZE_HPP

#include <rdf4cpp/rdf/Serialize.hpp>

#define RDF4CPP_DETAIL_TRY_SER(...)                             \
    if (!serialize_str({__VA_ARGS__}, buffer, cursor, flush)) { \
        return false;                                           \
    }

#endif // RDF4CPP_PRIVATE_TRY_SERIALIZE_HPP
