#ifndef RDF4CPP_DATATYPEREGISTRY_HPP
#define RDF4CPP_DATATYPEREGISTRY_HPP

#include <rdf4cpp/rdf/datatypes/LiteralDatatype.hpp>
#include <rdf4cpp/rdf/datatypes/registry/DatatypeConversion.hpp>

#include <algorithm>
#include <any>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
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
    using ebv_fptr_t = bool (*)(std::any const &);

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

        RuntimeConversionTable conversion_table;

        unsigned promote_rank; // number of times this type can be promoted
        unsigned subtype_rank; // number of supertypes above in hierarchy, 0 means no supertype

        inline static DatatypeEntry for_search(std::string_view const datatype_iri) {
            return DatatypeEntry{
                    std::string{datatype_iri},
                    nullptr,
                    nullptr,
                    nullptr,
                    std::nullopt,
                    {},
                    0,
                    0};
        }
    };

private:
    using registered_datatypes_t = std::vector<DatatypeEntry>;

    struct ConversionEntry {
        std::string target_type_iri;
        std::function<std::any(std::any const &)> convert_lhs;
        std::function<std::any(std::any const &)> convert_rhs;
    };

    using conversion_index_t = std::tuple<std::string, std::string>;
    using conversion_hasher_t = util::UnorderedPairHash<void, std::hash<std::string_view>>;
    using conversion_equal_t = util::UnorderedPairEqual<>;
    using conversion_mapping_t = std::unordered_map<conversion_index_t, std::optional<ConversionEntry>, conversion_hasher_t, conversion_equal_t>;

public:
    struct DatatypeConverter {
    private:
        friend class DatatypeRegistry;

        std::reference_wrapper<conversion_mapping_t::value_type const> entry;

        explicit DatatypeConverter(conversion_mapping_t::const_reference entry)
                : entry{ std::cref(entry) }
        {
        }
    public:
        /**
         * @return the iri of the target type of this conversion
         */
        [[nodiscard]]
        std::string_view target_type_iri() const noexcept {
            return entry.get().second->target_type_iri;
        }

        /**
         * converts a value of type origin_datatype
         * into a value of type target_type (with iri target_type_iri())
         *
         * @param origin_datatype_iri the datatype of the to be converted value
         * @param value the value to be converted
         * @return the converted value
         */
        [[nodiscard]]
        std::any convert(std::string_view const origin_datatype_iri, std::any const &value) const noexcept {
            auto const &e = entry.get();

            // necessary because conversions are only stored once for each combination so trying to find
            // a conversion for (lhs, rhs) might yield a ConversionEntry ordered (convert rhs, convert lhs)
            if (origin_datatype_iri == std::get<0>(e.first)) {
                return e.second->convert_lhs(value);
            } else {
                assert(origin_datatype_iri == std::get<1>(e.first));
                return e.second->convert_rhs(value);
            }
        }
    };

private:
    inline static registered_datatypes_t &get_mutable() {
        static registered_datatypes_t registry_;
        return registry_;
    }

    inline static conversion_mapping_t &get_conversion_mappings_mut() {
        static conversion_mapping_t mappings_;
        return mappings_;
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
    inline static std::optional<std::reference_wrapper<DatatypeEntry const>> get_entry(std::string_view const datatype_iri) {
        return find_map_entry(datatype_iri, [](auto const &entry) { return std::cref(entry); });
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

    /**
     * Tries to find a conversion to convert lhs_type_iri and rhs_type_iri into a
     * common type to be used in e.g. numeric calculations.
     *
     * @return if possible a conversion that converts lhs and rhs into a common type
     */
    inline static std::optional<DatatypeConverter> get_common_type_conversion(std::string_view lhs_type_iri, std::string_view rhs_type_iri);
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

    auto const promote_rank = []() -> unsigned {
        if constexpr (datatypes::PromotableLiteralDatatype<LiteralDatatype_t>) {
            return LiteralDatatype_t::promotion_rank;
        } else {
            return 0;
        }
    }();

    auto const subtype_rank = []() -> unsigned {
        if constexpr (datatypes::SubtypedLiteralDatatype<LiteralDatatype_t>) {
            return LiteralDatatype_t::subtype_rank;
        } else {
            return 0;
        }
    }();

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
            make_conversion_table_for<LiteralDatatype_t>().into_runtime_table(),
            promote_rank,
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

inline std::optional<DatatypeRegistry::DatatypeConverter> DatatypeRegistry::get_common_type_conversion(
        std::string_view const lhs_type_iri,
        std::string_view const rhs_type_iri) {

    auto &conversion_mappings = get_conversion_mappings_mut();

    // try to find existing conversion entry
    if (auto it = conversion_mappings.find(std::tie(lhs_type_iri, rhs_type_iri)); it != conversion_mappings.end()) {
        if (it->second.has_value()) {
            return DatatypeConverter{ *it };
        } else {
            return std::nullopt;
        }
    }

    auto const &lhs_entry = get_entry(lhs_type_iri).value().get();
    auto const &rhs_entry = get_entry(rhs_type_iri).value().get();

    // lesser should be type entry of the type with lower subtype rank
    auto const find_conv_impl = [](auto const &lesser, auto const &greater) -> std::optional<std::tuple<RuntimeConversionEntry const &, RuntimeConversionEntry const &>> {
        // initial subtype offset, aka. at which subtype depth/rank to start searching
        auto const init_s_off = greater.subtype_rank - lesser.subtype_rank;

        for (size_t s_off = init_s_off;
             s_off < greater.conversion_table.size() && s_off - init_s_off < lesser.conversion_table.size();
             ++s_off) {

            // get promotions table for given subtype level
            auto const &lesser_s_promotions = lesser.conversion_table[s_off - init_s_off];
            auto const &greater_s_promotions = greater.conversion_table[s_off];

            if (lesser_s_promotions.size() == greater_s_promotions.size()) {
                // same promotion rank
                // so either it is the first conversion or none at this level

                if (lesser_s_promotions[0].target_type_iri == greater_s_promotions[0].target_type_iri) {
                    return std::tie(lesser_s_promotions[0], greater_s_promotions[0]);
                }

                continue;
            }

            if (lesser_s_promotions.size() < greater_s_promotions.size()) {
                // lesser has lower promotion rank
                // => greater needs to be promoted

                assert(lesser_s_promotions.size() > 0);
                auto const p_off = greater_s_promotions.size() - lesser_s_promotions.size();

                auto const &lesser_p_conv = lesser_s_promotions[0];
                auto const &greater_p_conv = greater_s_promotions[p_off];

                if (greater_p_conv.target_type_iri == lesser_p_conv.target_type_iri) {
                    return std::tie(lesser_p_conv, greater_p_conv);
                }
            } else {
                // greater has lower promotion rank
                // => lesser needs to be promoted

                assert(greater_s_promotions.size() > 0);
                auto const p_off = lesser_s_promotions.size() - greater_s_promotions.size();

                auto const &lesser_p_conv = lesser_s_promotions[p_off];
                auto const &greater_p_conv = greater_s_promotions[0];

                if (greater_p_conv.target_type_iri == lesser_p_conv.target_type_iri) {
                    return std::tie(lesser_p_conv, greater_p_conv);
                }
            }
        }

        return std::nullopt;
    };

    // call find_conv_impl with entries in correct order (lesser s rank, greater s rank)
    auto const res = [&]() -> std::optional<std::tuple<RuntimeConversionEntry const &, RuntimeConversionEntry const &>> {
        if (lhs_entry.subtype_rank < rhs_entry.subtype_rank) {
            return find_conv_impl(lhs_entry, rhs_entry);
        } else {
            auto res = find_conv_impl(rhs_entry, lhs_entry);

            if (res.has_value()) {
                // swap values to return pair in correct order (lhs, rhs)
                return std::tie(std::get<1>(*res), std::get<0>(*res));
            }

            return res;
        }
    }();

    if (!res.has_value()) {
        return std::nullopt;
    }

    assert(std::get<0>(*res).target_type_iri == std::get<1>(*res).target_type_iri);
    ConversionEntry conversion {
            std::get<0>(*res).target_type_iri,
            std::get<0>(*res).conversion_fn,
            std::get<1>(*res).conversion_fn};

    // insert found conversion into cache
    auto const [it, _] = conversion_mappings.emplace(std::piecewise_construct,
                                                     std::forward_as_tuple(lhs_type_iri, rhs_type_iri),
                                                     std::forward_as_tuple(conversion));
    return DatatypeConverter{ *it };
}

}  // namespace rdf4cpp::rdf::datatypes::registry
#endif  //RDF4CPP_DATATYPEREGISTRY_HPP
