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

public:
    static constexpr identifier::LiteralType datatype = T::fixed_id;
    typename T::cpp_type value;

    explicit SpecializedLiteralBackend(View const &view) noexcept : hash_{view.hash<Type>()},
                                                                    value{std::any_cast<typename T::cpp_type>(view.value)} {
        assert(view.datatype == SpecializedLiteralBackend::datatype);
    }

    [[nodiscard]] size_t hash() const noexcept {
        return hash_;
    }

    explicit operator View() const noexcept {
        return View{.datatype = datatype,
                    .value = value};
    }

public:
    struct Equal {
        using is_transparent = void;

        bool operator()(SpecializedLiteralBackend const *lhs, SpecializedLiteralBackend const *rhs) const noexcept {
            return lhs == rhs;
        }

        bool operator()(View const &lhs, SpecializedLiteralBackend const *rhs) const noexcept {
            assert(lhs.datatype == SpecializedLiteralBackend::datatype);
            return lhs.eq<Type>(rhs->value);
        }

        bool operator()(SpecializedLiteralBackend const *lhs, View const &rhs) const noexcept {
            assert(SpecializedLiteralBackend::datatype == rhs.datatype);
            return rhs.eq<Type>(lhs->value);
        }
    };

    struct Hash {
        using is_transparent = void;

        [[nodiscard]] size_t operator()(SpecializedLiteralBackend const *x) const noexcept {
            return x->hash();
        }

        [[nodiscard]] size_t operator()(View const &x) const noexcept {
            assert(x.datatype == SpecializedLiteralBackend::datatype);
            return x.hash<Type>();
        }
    };
};

}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage

#endif  //RDF4CPP_RDF_STORAGE_REFERENCENODESTORAGE_SPECIALIZEDLITERALBACKEND_HPP
