#ifndef RDF4CPP_LITERALDATATYPEIMPL_HPP
#define RDF4CPP_LITERALDATATYPEIMPL_HPP

#include <rdf4cpp/rdf/datatypes/registry/ConstexprString.hpp>
#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/DatatypeRegistry.hpp>

#include <cstddef>
#include <iosfwd>
#include <sstream>
#include <string_view>
#include <type_traits>

namespace rdf4cpp::rdf::datatypes::registry {

template<ConstexprString type_iri_t>
struct LiteralDatatypeImpl {
private:
    /**
     * static_assert would always trigger if it wasn't dependent on a template parameter.
     * With this helper template, it only triggers if the function is instantiated.
     */
    template<typename>
    using always_false = std::false_type;
    template<typename T>
    static constexpr bool always_false_v = always_false<T>::value;


public:
    /**
     * IRI of the LiteralDatatype.
     */
    static constexpr ConstexprString identifier = type_iri_t;

    /**
     * The C++ type that this LiteralDatatype is mapped to.
     */
    using cpp_type = typename DatatypeMapping<identifier>::cpp_datatype;

    /**
     * Factory function that parses a string representing datatype_t and builds an instance of datatype_t
     * @return instance of datatype_t
     */
    inline static cpp_type from_string(std::string_view) {
        // If this implementation is used the user forgot to provide their own.
        static_assert(always_false_v<cpp_type>, "'from_string' is not implemented for this type!");
    }
    /**
     * Returns string representation of a datatype_t.
     * @param value the value
     * @return <div>value</div>'s canonical string representation
     */
    inline static std::string to_string( cpp_type const &value) {
        // If not further specified, to_string is instanciated via operator<<. If operator<< is not defined for cpp_type instanciation will fail.
        std::stringstream str_s;
        str_s << value;
        return str_s.str();
    }

private:
    inline static std::nullptr_t init();
    inline static const auto dummy = init();

    // Force `dummy` to be instantiated, even though it's unused.
    static constexpr std::integral_constant<decltype(&dummy), &dummy> dummy_helper{};
};
template<ConstexprString xsd_string>
std::nullptr_t LiteralDatatypeImpl<xsd_string>::init() {
    DatatypeRegistry::add<LiteralDatatypeImpl<xsd_string>>();
    return nullptr;
}
}  // namespace rdf4cpp::rdf::datatypes::registry
#endif  //RDF4CPP_LITERALDATATYPEIMPL_HPP
