#ifndef RDF4CPP_DATATYPEREGISTRY_HPP
#define RDF4CPP_DATATYPEREGISTRY_HPP

#include <algorithm>
#include <any>
#include <vector>

namespace rdf4cpp::rdf::datatypes {

/**
 * Concept required from classes deriving DatatypeBase. Guarantees full compatibility with Literal.
 */
template<typename T>
concept DatatypeConcept =
        ((std::is_constructible_v<T, std::string> || std::is_constructible_v<T, const std::string &> || std::is_constructible_v<T, std::string &&>) &&  //
         requires(T a) {
             { T::datatype_iri } -> std::convertible_to<std::string>;
             { static_cast<std::string>(a) } -> std::convertible_to<std::string>;
         });

/**
 * Registry for Literal datatype implementations.
 * Datatypes must be registered for automatic conversation from a typed Literal with Literal::value() const.
 * Datatypes that derived from DatatypeBase are automatically registered.
 */
class DatatypeRegistry {
public:
    /**
     * Constructs an instance of a type from a string.
     */
    using factory_function_ptr = std::any (*)(std::string);

    using registered_datatypes_t = std::vector<std::pair<std::string, factory_function_ptr>>;

private:
    static inline std::vector<std::pair<std::string, factory_function_ptr>> &get_mutable() {
        static std::vector<std::pair<std::string, factory_function_ptr>> registry_;
        return registry_;
    }

public:
    /**
     * Auto-register a datatype that fulfills DatatypeConcept
     * @tparam datatype type that is registered.
     */
    template<DatatypeConcept datatype>
    static inline void add() {
        DatatypeRegistry::add(datatype::datatype_iri, [](std::string string_repr) -> std::any { return std::any(datatype{std::move(string_repr)}); });
    }

    /**
     * Register an datatype manually
     * @param datatype_iri datatypes iri
     * @param factory_function factory function to construct an instance from a string
     */
    static inline void add(std::string datatype_iri, factory_function_ptr factory_function) {
        auto &registry = DatatypeRegistry::get_mutable();
        auto found = std::find_if(registry.begin(), registry.end(), [&](const auto &pair) { return pair.first == datatype_iri; });
        if (found == registry.end()) {
            registry.emplace_back(datatype_iri, factory_function);
            std::sort(registry.begin(), registry.end(),
                      [](const auto &left, const auto &right) { return left.first < right.first; });
        } else {
            found->second = factory_function;
        }
    }

    /**
     * Retrieve all registered datatypes.
     * @return vector of pairs mapping datatype IRI std::string to factory_function_ptr
     */
    static inline const registered_datatypes_t &registered_datatypes() {
        return DatatypeRegistry::get_mutable();
    }

    /**
     * Get a factory_function_ptr for a datatype IRI std::string. The factory_function_ptr can be used like `std::any type_instance = factory_function_ptr("types string repressentation")`.
     * @param datatype_iri datatype IRI std::string
     * @return function pointer or nullptr
     */
    static inline factory_function_ptr lookup(const std::string &datatype_iri) {
        const auto &registry = registered_datatypes();
        auto found = std::lower_bound(registry.begin(), registry.end(),
                                      std::pair<std::string, factory_function_ptr>{datatype_iri, nullptr},
                                      [](const auto &left, const auto &right) { return left.first < right.first; });
        if (found != registry.end() and found->first == datatype_iri) {
            return found->second;
        } else {
            return nullptr;
        }
    }
};

/**
 * Datatypes can have DatatypeBase as <a href="https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern">CRTP</a> base class. If they have, they are registered automatically.
 * @tparam Derived The derived class is known at compile time to the base class.
 */
template<typename Derived>
class DatatypeBase {
    static inline std::nullptr_t init();
    inline static const auto dummy = init();

    // Force `dummy` to be instantiated, even though it's unused.
    static constexpr std::integral_constant<decltype(&dummy), &dummy> dummy_helper{};
};
template<typename Derived>
std::nullptr_t DatatypeBase<Derived>::init() {
    DatatypeRegistry::add<Derived>();
    return nullptr;
}
}  // namespace rdf4cpp::rdf::datatypes

#endif  //RDF4CPP_DATATYPEREGISTRY_HPP
