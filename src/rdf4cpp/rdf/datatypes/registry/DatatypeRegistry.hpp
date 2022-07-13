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
    struct ConversionResult {
        std::string_view converted_iri;
        std::any converted_value;
    };

    /**
     * Constructs an instance of a type from a string.
     */
    using factory_fptr_t = std::any (*)(std::string_view);
    using to_string_fptr_t = std::string (*)(const std::any &);
    using ebv_fptr_t = bool (*)(std::any const &);

    using promote_fptr_t = ConversionResult (*)(std::any const &);
    using into_supertype_fptr_t = ConversionResult (*)(std::any const &);

    using unop_fptr_t = std::any (*)(std::any const &);
    using binop_fptr_t = std::any (*)(std::any const &, std::any const &);

    struct NumericOps {
        binop_fptr_t add_fptr; // a + b
        binop_fptr_t sub_fptr; // a - b
        binop_fptr_t mul_fptr; // a * b
        binop_fptr_t div_fptr; // a / b

        unop_fptr_t pos_fptr; // +a
        unop_fptr_t neg_fptr; // -a
    };

    struct DatatypeEntry {
        std::string datatype_iri;        // datatype IRI string
        factory_fptr_t factory_fptr;     // construct from string
        to_string_fptr_t to_string_fptr; // convert to string

        ebv_fptr_t ebv_fptr; // convert to effective boolean value

        std::optional<NumericOps> numeric_ops;

        promote_fptr_t promote_fptr; // type promotion
        unsigned promote_rank;       // number of times this type can be promoted

        into_supertype_fptr_t into_supertype_fptr; // conversion to supertype
        unsigned subtype_rank;                     // number of supertypes above in hierarchy, 0 means no supertype

        inline static DatatypeEntry for_search(std::string_view const datatype_iri) {
            return DatatypeEntry{
                    std::string{datatype_iri},
                    nullptr,
                    nullptr,
                    nullptr,
                    std::nullopt,
                    nullptr,
                    0,
                    nullptr,
                    0};
        }
    };

    using registered_datatypes_t = std::vector<DatatypeEntry>;

private:
    inline static registered_datatypes_t &get_mutable() {
        static registered_datatypes_t registry_;
        return registry_;
    }

    /**
     * Tries to find the datatype corresponding to datatype_iri
     * if found `f` is applied to it to form the function result.
     *
     * @param datatype_iri the datatype to search
     * @param f function that maps a DatatypeEntry to (typically) a member of it
     * @return the result of `f` applied to the DatatypeEntry, if it exists, else nullopt
     */
    template<typename Map>
        requires std::invocable<Map, DatatypeEntry const &>
    static std::optional<std::invoke_result_t<Map, DatatypeEntry const &>> find_map_entry(std::string_view const datatype_iri, Map f) {
        auto const &registry = registered_datatypes();
        auto found = std::lower_bound(registry.begin(), registry.end(),
                                      DatatypeEntry::for_search(datatype_iri),
                                      [](const auto &left, const auto &right) { return left.datatype_iri < right.datatype_iri; });

        if (found != registry.end() and found->datatype_iri == datatype_iri) {
            return f(*found);
        } else {
            return std::nullopt;
        }
    }

    /**
     * Creates NumericOps based on a NumericLiteralDatatype
     * by generating type-erased versions of all necessary functions (add, sub, ...).
     */
    template<datatypes::NumericLiteralDatatype datatype_info>
    inline static NumericOps make_numeric_ops();
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
    inline static void add(DatatypeEntry entry_to_add){
        auto &registry = DatatypeRegistry::get_mutable();
        auto found = std::find_if(registry.begin(), registry.end(), [&](const auto &entry) { return entry.datatype_iri == entry_to_add.datatype_iri; });
        if (found == registry.end()) {
            registry.push_back(entry_to_add);

            std::sort(registry.begin(), registry.end(),
                      [](const auto &left, const auto &right) { return left.datatype_iri < right.datatype_iri; });
        } else {
            *found = entry_to_add;
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
     * Get the database entry for a datatype_iri.
     * @param datatype_iri datatype IRI string
     * @return if available database entry else nullopt
     */
    inline static std::optional<DatatypeEntry> get_entry(std::string_view const datatype_iri) {
        return find_map_entry(datatype_iri, [](auto const &entry) { return entry; });
    }

    /**
     * Get a factory_fptr_t for a datatype IRI std::string. The factory_fptr_t can be used like `std::any type_instance = factory_fptr("types string repressentation")`.
     * @param datatype_iri datatype IRI std::string
     * @return function pointer or nullptr
     */
    inline static factory_fptr_t get_factory(std::string_view datatype_iri) {
        auto const res = find_map_entry(datatype_iri, [](auto const &entry) {
            return entry.factory_fptr;
        });

        return res.has_value() ? *res : nullptr;
    }

    /**
     * Get a to_string function for a datatype IRI std::string. The factory_fptr_t can be used like `std::string str_repr = to_string_fptr(any_typed_arg)`.
     * @param datatype_iri datatype IRI std::string
     * @return function pointer or nullptr
     */
    inline static to_string_fptr_t get_to_string(std::string_view datatype_iri) {
        auto const res = find_map_entry(datatype_iri, [](auto const &entry) {
            return entry.to_string_fptr;
        });

        return res.has_value() ? *res : nullptr;
    }

    /**
     * Try to get the numerical ops function table for a datatype IRI.
     * Returns nullopt if the datatype is not numeric, or does not exist.
     *
     * @param datatype_iri datatype IRI string
     * @return if available a structure containing function pointers for all numeric ops
     */
    inline static std::optional<NumericOps> get_numerical_ops(std::string_view const datatype_iri) {
        auto const res = find_map_entry(datatype_iri, [](auto const &entry) {
            return entry.numeric_ops;
        });

        return res.has_value() ? *res : std::nullopt;
    }

    /**
     * Try to get the logical ops function table for a datatype IRI.
     * Returns nullptr if the datatype is not logical, or does not exist.
     *
     * @param datatype_iri datatype IRI string
     * @return if available a structure containing function pointers for all logical ops
     */
    inline static ebv_fptr_t get_ebv(std::string_view const datatype_iri) {
        auto const res = find_map_entry(datatype_iri, [](auto const &entry) {
            return entry.ebv_fptr;
        });

        return res.has_value() ? *res : nullptr;
    }

    /**
     * Try to get the type promotion function for datatype_iri.
     * @param datatype_iri datatype IRI string
     * @return if available the promotion function else nullptr
     */
    inline static promote_fptr_t get_promote(std::string_view const datatype_iri) {
        auto const res = find_map_entry(datatype_iri, [](auto const &entry) {
            return entry.promote_fptr;
        });

        return res.has_value() ? *res : nullptr;
    }

    /**
     * Get the promotion rank for datatype_iri.
     * @param datatype_iri datatype IRI string
     * @return promotion rank if datatype exists else 0
     */
    inline static unsigned get_promote_rank(std::string_view const datatype_iri) {
        auto const res = find_map_entry(datatype_iri, [](auto const &entry) {
            return entry.promote_rank;
        });

        return res.has_value() ? *res : 0;
    }

    /**
     * Try to get the type supertype conversion function for datatype_iri.
     * @param datatype_iri datatype IRI string
     * @return if available the conversion function else nullptr
     */
    inline static into_supertype_fptr_t get_into_supertype(std::string_view const datatype_iri) {
        auto const res = find_map_entry(datatype_iri, [](auto const &entry) {
            return entry.into_supertype_fptr;
        });

        return res.has_value() ? *res : nullptr;
    }

    /**
     * Get the subtype rank for datatype_iri.
     * @param datatype_iri datatype IRI string
     * @return subtype rank if datatype exists else 0
     */
    inline static unsigned get_subtype_rank(std::string_view const datatype_iri) {
        auto const res = find_map_entry(datatype_iri, [](auto const &entry) {
            return entry.subtype_rank;
        });

        return res.has_value() ? *res : 0;
    }
};


/**
 * To register a datatype, at least a LiteralDatatypeImpl instantiation must be provided.
 * If LiteralDatatype_t::cpp_type does not overload `operator<<`, the to_string(const datatype_t &value) must be specialized.
 * @tparam LiteralDatatype_t datatype that is being registered
 */
template<datatypes::LiteralDatatype LiteralDatatype_t>
inline void DatatypeRegistry::add() {
    auto const num_ops = []() -> std::optional<NumericOps> {
        if constexpr (datatypes::NumericLiteralDatatype<LiteralDatatype_t>) {
            return make_numeric_ops<LiteralDatatype_t>();
        } else {
            return std::nullopt;
        }
    }();

    auto const ebv_fptr = []() -> ebv_fptr_t {
        if constexpr (datatypes::LogicalLiteralDatatype<LiteralDatatype_t>) {
            return [](std::any const &operand) -> bool {
                auto const &operand_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(operand);
                return LiteralDatatype_t::effective_boolean_value(operand_val);
            };
        } else {
            return nullptr;
        }
    }();

    auto const promote_fptr = []() -> promote_fptr_t {
        if constexpr (datatypes::PromotableLiteralDatatype<LiteralDatatype_t>) {
            return [](std::any const &value) -> ConversionResult {
                return ConversionResult{
                        LiteralDatatype_t::promoted::identifier,
                        LiteralDatatype_t::promote(std::any_cast<typename LiteralDatatype_t::cpp_type>(value))};
            };
        } else {
            return nullptr;
        }
    }();

    auto const promote_rank = detail_rank::DatatypePromotionRank<LiteralDatatype_t::identifier>::value;

    auto const into_supertype_fptr = []() -> into_supertype_fptr_t {
        if constexpr (datatypes::SubtypedLiteralDatatype<LiteralDatatype_t>) {
            return [](std::any const &value) -> ConversionResult {
                return ConversionResult{
                        LiteralDatatype_t::supertype::identifier,
                        LiteralDatatype_t::into_supertype(std::any_cast<typename LiteralDatatype_t::cpp_type>(value))};
            };
        } else {
            return nullptr;
        }
    }();

    auto const subtype_rank = detail_rank::DatatypeSubtypeRank<LiteralDatatype_t::identifier>::value;

    DatatypeRegistry::add(DatatypeEntry {
            std::string{ LiteralDatatype_t::identifier },
            [](std::string_view string_repr) -> std::any {
                return std::any(LiteralDatatype_t::from_string(string_repr));
            },
            [](const std::any &value) -> std::string {
                return LiteralDatatype_t::to_string(std::any_cast<typename LiteralDatatype_t::cpp_type>(value));
            },
            ebv_fptr,
            num_ops,
            promote_fptr,
            promote_rank,
            into_supertype_fptr,
            subtype_rank});
}

template<datatypes::NumericLiteralDatatype LiteralDatatype_t>
inline DatatypeRegistry::NumericOps DatatypeRegistry::make_numeric_ops() {
    return NumericOps{
            // a + b
            [](std::any const &lhs, std::any const &rhs) -> std::any {
                auto const &lhs_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(lhs);
                auto const &rhs_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(rhs);

                return LiteralDatatype_t::add(lhs_val, rhs_val);
            },
            // a - b
            [](std::any const &lhs, std::any const &rhs) -> std::any {
                auto const &lhs_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(lhs);
                auto const &rhs_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(rhs);

                return LiteralDatatype_t::sub(lhs_val, rhs_val);
            },
            // a * b
            [](std::any const &lhs, std::any const &rhs) -> std::any {
                auto const &lhs_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(lhs);
                auto const &rhs_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(rhs);

                return LiteralDatatype_t::mul(lhs_val, rhs_val);
            },
            // a / b
            [](std::any const &lhs, std::any const &rhs) -> std::any {
                auto const &lhs_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(lhs);
                auto const &rhs_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(rhs);

                return LiteralDatatype_t::div(lhs_val, rhs_val);
            },
            // +a
            [](std::any const &operand) -> std::any {
                auto const &operand_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(operand);
                return LiteralDatatype_t::pos(operand_val);
            },
            // -a
            [](std::any const &operand) -> std::any {
                auto const &operand_val = std::any_cast<typename LiteralDatatype_t::cpp_type>(operand);
                return LiteralDatatype_t::neg(operand_val);
            }
    };
}

}  // namespace rdf4cpp::rdf::datatypes::registry
#endif  //RDF4CPP_DATATYPEREGISTRY_HPP
