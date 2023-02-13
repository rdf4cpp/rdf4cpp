#ifndef RDF4CPP_RDF_STORAGE_REFERENCENODESTORAGE_SPECIALIZEDLITERALBACKEND_HPP
#define RDF4CPP_RDF_STORAGE_REFERENCENODESTORAGE_SPECIALIZEDLITERALBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>
#include <rdf4cpp/rdf/storage/node/view/LiteralBackendView.hpp>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

template<datatypes::FixedIdLiteralDatatype T>
struct SpecializedLiteralBackend {
    using View = view::ValueLiteralBackendView;
    using Type = T;
private:
    size_t hash_;
    static size_t calculate_hash(typename T::cpp_type const &value) noexcept {
        return util::robin_hood::hash<typename T::cpp_type>{}(value);
    }

public:
    static constexpr identifier::LiteralType datatype = T::fixed_id;
    typename T::cpp_type value;


    explicit SpecializedLiteralBackend(typename T::cpp_type const &value) noexcept : hash_{calculate_hash(value)},
                                                                            value{value} {
    }

    explicit SpecializedLiteralBackend(typename T::cpp_type &&value) noexcept : hash_{calculate_hash(value)},
                                                                       value{std::move(value)} {
    }

    explicit SpecializedLiteralBackend(View const &view) noexcept : value{std::any_cast<typename T::cpp_type>(view.value)} {
        assert(view.datatype == SpecializedLiteralBackend::datatype);
        this->hash_ = calculate_hash(value);
    }

    [[nodiscard]] size_t hash() const noexcept {
        return hash_;
    }

    explicit operator View() const noexcept {
        return view::ValueLiteralBackendView{
                .datatype = SpecializedLiteralBackend::datatype,
                .value = std::any{this->value}};
    }

public:
    struct Equal {
        using is_transparent = void;

        bool operator()(SpecializedLiteralBackend const *lhs, SpecializedLiteralBackend const *rhs) const noexcept {
            return lhs == rhs;
        }
        bool operator()(View const &lhs, SpecializedLiteralBackend const *rhs) const noexcept {
            assert(lhs.datatype == SpecializedLiteralBackend::datatype);
            return std::any_cast<typename T::cpp_type>(lhs.value) == rhs->value;
        }

        bool operator()(SpecializedLiteralBackend const *lhs, View const &rhs) const noexcept {
            assert(SpecializedLiteralBackend::datatype == rhs.datatype);
            return lhs->value == std::any_cast<typename T::cpp_type>(rhs.value);
        }
    };

    struct Hash {
        [[nodiscard]] size_t operator()(SpecializedLiteralBackend const *x) const noexcept {
            return x->hash();
        }
        [[nodiscard]] size_t operator()(View const &x) const noexcept {
            assert(x.datatype == SpecializedLiteralBackend::datatype);
            return calculate_hash(std::any_cast<typename T::cpp_type>(x.value));
        }
    };
};

}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage

#endif  //RDF4CPP_RDF_STORAGE_REFERENCENODESTORAGE_SPECIALIZEDLITERALBACKEND_HPP
