#ifndef RDF4CPP_RDF_STORAGE_REFERENCENODESTORAGE_SPECIALIZEDLITERALBACKEND_HPP
#define RDF4CPP_RDF_STORAGE_REFERENCENODESTORAGE_SPECIALIZEDLITERALBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>
#include <rdf4cpp/rdf/storage/node/view/LiteralBackendView.hpp>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

template<datatypes::FixedIdLiteralDatatype T>
struct SpecializedLiteralBackend {
    using View = view::ValueLiteralBackendView;
    using Type = T;
    using Id = identifier::LiteralID;

    size_t hash;
    static constexpr identifier::LiteralType datatype = T::fixed_id;
    typename T::cpp_type value;

    explicit SpecializedLiteralBackend(View const &view) noexcept : hash{view.hash<Type>()},
                                                                    value{std::any_cast<typename T::cpp_type>(view.value)} {
        assert(view.datatype == SpecializedLiteralBackend::datatype);
    }

    explicit operator View() const noexcept {
        return View{.datatype = datatype,
                    .value = value};
    }

    static identifier::NodeID to_node_id(Id const id, View const view) noexcept {
        return identifier::NodeID{id, view.datatype};
    }

    static Id to_backend_id(identifier::NodeID const id) noexcept {
        return id.literal_id();
    }

    struct Equal {
        using is_transparent = void;

        bool operator()(View const &lhs, SpecializedLiteralBackend const &rhs) const noexcept {
            assert(lhs.datatype == SpecializedLiteralBackend::datatype);
            return lhs.eq<Type>(rhs.value);
        }

        bool operator()(SpecializedLiteralBackend const &lhs, View const &rhs) const noexcept {
            assert(SpecializedLiteralBackend::datatype == rhs.datatype);
            return rhs.eq<Type>(lhs.value);
        }
    };

    struct Hash {
        using is_transparent = void;

        [[nodiscard]] size_t operator()(View const &x) const noexcept {
            assert(x.datatype == SpecializedLiteralBackend::datatype);
            return x.hash<Type>();
        }
    };
};

}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage

#endif  //RDF4CPP_RDF_STORAGE_REFERENCENODESTORAGE_SPECIALIZEDLITERALBACKEND_HPP
