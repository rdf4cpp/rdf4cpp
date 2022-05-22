#ifndef RDF4CPP_DATATYPEREGISTRY_HPP
#define RDF4CPP_DATATYPEREGISTRY_HPP

#include <algorithm>
#include <any>
#include <sstream>
#include <vector>

namespace rdf4cpp::rdf::datatypes {

/**
 * Literal class type that wraps a constant expression string.
 *
 * Uses implicit conversion to allow templates to *seemingly* accept constant strings.
 *
 * Extended version of https://ctrpeach.io/posts/cpp20-string-literal-template-parameters/.
 */
template<size_t N>
struct ConstexprString {
    constexpr ConstexprString(const char (&str)[N]) noexcept {
        std::copy_n(str, N, value);
    }

    char value[N];

    operator std::string() const noexcept {
        return std::string{value};
    }

    template<size_t M>
    constexpr std::strong_ordering operator<=>(ConstexprString<M> const &other) const noexcept {
        auto min = std::min(M, N);
        for (size_t i = 0; i < min; ++i) {
            std::strong_ordering cmp = this->value[i] <=> other.value[i];
            if (cmp != std::strong_ordering::equal)
                return cmp;
        }
        std::strong_ordering cmp = N <=> M;
        return cmp;
    }
};

/**
 * Registry for Literal datatype implementations.
 * Data types are registered by defining implementing and specializing members of RegisteredDatatype.
 * @see RegisteredDatatype
 */
class DatatypeRegistry {
public:
    /**
     * Constructs an instance of a type from a string.
     */
    using factory_fptr_t = std::any (*)(std::string_view);
    using to_string_fptr_t = std::string (*)(const std::any &);

    struct DatatypeEntry {
        std::string name;
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
    template<typename datatype_info>
    inline static void add();

    /**
     * Register an datatype manually
     * @param datatype_iri datatypes iri
     * @param factory_fptr factory function to construct an instance from a string
     * @param to_string_fptr converts type instance to its string representation
     */
    inline static void add(std::string datatype_iri, factory_fptr_t factory_fptr, to_string_fptr_t to_string_fptr) {
        auto &registry = DatatypeRegistry::get_mutable();
        auto found = std::find_if(registry.begin(), registry.end(), [&](const auto &entry) { return entry.name == datatype_iri; });
        if (found == registry.end()) {
            registry.push_back({datatype_iri, factory_fptr, to_string_fptr});
            std::sort(registry.begin(), registry.end(),
                      [](const auto &left, const auto &right) { return left.name < right.name; });
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
                                      [](const auto &left, const auto &right) { return left.name < right.name; });
        if (found != registry.end() and found->name == datatype_iri) {
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
                                      [](const auto &left, const auto &right) { return left.name < right.name; });
        if (found != registry.end() and found->name == datatype_iri) {
            return found->to_string_fptr;
        } else {
            return nullptr;
        }
    }
};


/**
 * To register a datatype, datatype_iri and from_string must be defined for `RegisteredDatatype<datatype_t>`.
 * If datatype_t does not overload `operator<<`, to_string(const datatype_t &value) must be specialized.
 * @tparam datatype_t datatype that is being registered
 */
template<typename datatype_t, ConstexprString type_iri>
struct RegisteredDatatype {
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
     * Datatype iri
     */
    constexpr static char *datatype_iri_cstr() noexcept {
        return type_iri.value;
    }
    /**
     * Datatype iri
     */
    inline static std::string datatype_iri() noexcept {
        return (std::string) type_iri;
    }
    /**
     * Factory function that parses a string representing datatype_t and builds an instance of datatype_t
     * @return instance of datatype_t
     */
    inline static datatype_t from_string(std::string_view) {
        //If this implementation is used the user forgot to provide their own.
        static_assert(always_false_v<datatype_t>, "'from_string' is not implemented for this type!");
    }
    /**
     * Returns string representation of a datatype_t.
     * @param value an datatype_t instance
     * @return <div>value</div>'s canonical string representation
     */
    inline static std::string to_string(const datatype_t &value) {

        std::stringstream str_s;
        str_s << value;
        return str_s.str();
    }

    /**
     * Exposes template parameter datatype_t.
     */
    typedef datatype_t datatype;

private:
    inline static std::nullptr_t init();
    inline static const auto dummy = init();

    // Force `dummy` to be instantiated, even though it's unused.
    static constexpr std::integral_constant<decltype(&dummy), &dummy> dummy_helper{};
};
template<typename xsd_datatype_t, ConstexprString xsd_string>
std::nullptr_t RegisteredDatatype<xsd_datatype_t, xsd_string>::init() {
    DatatypeRegistry::add<RegisteredDatatype<xsd_datatype_t, xsd_string>>();
    return nullptr;
}

template<typename datatype_info>
inline void DatatypeRegistry::add() {
    DatatypeRegistry::add(
            datatype_info::datatype_iri(),
            [](std::string_view string_repr) -> std::any {
                return std::any(datatype_info::from_string(string_repr));
            },
            [](const std::any &value) -> std::string {
                return datatype_info::to_string(std::any_cast<typename datatype_info::datatype>(value));
            });
}
}  // namespace rdf4cpp::rdf::datatypes

#endif  //RDF4CPP_DATATYPEREGISTRY_HPP