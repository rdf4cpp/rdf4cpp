#ifndef RDF4CPP_DATATYPEREGISTRY_HPP
#define RDF4CPP_DATATYPEREGISTRY_HPP

#include <rdf4cpp/rdf/datatypes/LiteralDatatype.hpp>

#include <algorithm>
#include <any>
#include <string>
#include <string_view>
#include <vector>

namespace rdf4cpp::rdf::datatypes::registry {

/**
 * Registry for LiteralDatatype implementations.
 * Data types are registered by defining, implementing and specializing members of LiteralDatatype.
 * @see LiteralDatatype
 */
class DatatypeRegistry {
public:
    /**
     * Constructs an instance of a type from a string.
     */
    using factory_fptr_t = std::any (*)(std::string_view);
    using to_string_fptr_t = std::string (*)(const std::any &);

    struct DatatypeEntry {
        std::string datatype_iri;
        factory_fptr_t factory_fptr;
        to_string_fptr_t to_string_fptr;
    };

    using registered_datatypes_t = std::vector<DatatypeEntry>;

private:
    inline static registered_datatypes_t &get_mutable() {
        static registered_datatypes_t registry_;
        return registry_;
    }

public:
    /**
     * Auto-register a datatype that fulfills DatatypeConcept
     * @tparam datatype_info type that is registered.
     */
    template<datatypes::LiteralDatatype datatype_info>
    inline static void add();

    /**
     * Register an datatype manually
     * @param datatype_iri datatypes iri
     * @param factory_fptr factory function to construct an instance from a string
     * @param to_string_fptr converts type instance to its string representation
     */
    inline static void add(std::string datatype_iri, factory_fptr_t factory_fptr, to_string_fptr_t to_string_fptr) {
        auto &registry = DatatypeRegistry::get_mutable();
        auto found = std::find_if(registry.begin(), registry.end(), [&](const auto &entry) { return entry.datatype_iri == datatype_iri; });
        if (found == registry.end()) {
            registry.push_back({datatype_iri, factory_fptr, to_string_fptr});
            std::sort(registry.begin(), registry.end(),
                      [](const auto &left, const auto &right) { return left.datatype_iri < right.datatype_iri; });
        } else {
            found->factory_fptr = factory_fptr;
            found->to_string_fptr = to_string_fptr;
        }
    }

    /**
     * Retrieve all registered datatypes.
     * @return vector of pairs mapping datatype IRI std::string to factory_fptr_t
     */
    inline static const registered_datatypes_t &registered_datatypes() {
        return DatatypeRegistry::get_mutable();
    }

    /**
     * Get a factory_fptr_t for a datatype IRI std::string. The factory_fptr_t can be used like `std::any type_instance = factory_fptr("types string repressentation")`.
     * @param datatype_iri datatype IRI std::string
     * @return function pointer or nullptr
     */
    inline static factory_fptr_t get_factory(std::string_view datatype_iri) {
        const auto &registry = registered_datatypes();
        auto found = std::lower_bound(registry.begin(), registry.end(),
                                      DatatypeEntry{std::string{datatype_iri}, nullptr, nullptr},
                                      [](const auto &left, const auto &right) { return left.datatype_iri < right.datatype_iri; });
        if (found != registry.end() and found->datatype_iri == datatype_iri) {
            return found->factory_fptr;
        } else {
            return nullptr;
        }
    }

    /**
     * Get a to_string function for a datatype IRI std::string. The factory_fptr_t can be used like `std::string str_repr = to_string_fptr(any_typed_arg)`.
     * @param datatype_iri datatype IRI std::string
     * @return function pointer or nullptr
     */
    inline static to_string_fptr_t get_to_string(std::string_view datatype_iri) {
        const auto &registry = registered_datatypes();
        auto found = std::lower_bound(registry.begin(), registry.end(),
                                      DatatypeEntry{std::string{datatype_iri}, nullptr, nullptr},
                                      [](const auto &left, const auto &right) { return left.datatype_iri < right.datatype_iri; });
        if (found != registry.end() and found->datatype_iri == datatype_iri) {
            return found->to_string_fptr;
        } else {
            return nullptr;
        }
    }
};


/**
 * To register a datatype, a LiteralDatatypeImpl instantiation must be provided.
 * If LiteralDatatype_t::cpp_type does not overload `operator<<`, the to_string(const datatype_t &value) must be specialized.
 * @tparam LiteralDatatype_t datatype that is being registered
 */
template<datatypes::LiteralDatatype LiteralDatatype_t>
inline void DatatypeRegistry::add() {
    DatatypeRegistry::add(
            LiteralDatatype_t::identifier,
            [](std::string_view string_repr) -> std::any {
                return std::any(LiteralDatatype_t::from_string(string_repr));
            },
            [](const std::any &value) -> std::string {
                return LiteralDatatype_t::to_string(std::any_cast<typename LiteralDatatype_t::cpp_type>(value));
            });
}
}  // namespace rdf4cpp::rdf::datatypes::registry
#endif  //RDF4CPP_DATATYPEREGISTRY_HPP
