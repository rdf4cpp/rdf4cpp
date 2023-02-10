#ifndef RDF4CPP_SPECIALIZEDLITERALBACKEND_HPP
#define RDF4CPP_SPECIALIZEDLITERALBACKEND_HPP

#include <rdf4cpp/rdf/util/Any.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>
#include <rdf4cpp/rdf/storage/node/view/LiteralBackendView.hpp>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

template<datatypes::FixedIdLiteralDatatype T>
struct SpecializedLiteralBackend {
    using View = view::ValueLiteralBackendView;
    using Type = T;
private:
    size_t hash_;
    static size_t calculate_hash(T::cpp_type const &value) noexcept {
        return util::robin_hood::hash<typename T::cpp_type>{}(value);
    }

public:
    static constexpr identifier::LiteralType datatype = T::fixed_id;
    T::cpp_type value;


    explicit SpecializedLiteralBackend(T::cpp_type const &value) noexcept : hash_{calculate_hash(value)},
                                                                            value{value} {
    }

    explicit SpecializedLiteralBackend(T::cpp_type &&value) noexcept : hash_{calculate_hash(value)},
                                                                       value{std::move(value)} {
    }

    explicit SpecializedLiteralBackend(View const &view) noexcept : value{view.value.get_unchecked<typename T::cpp_type>()} {
        assert(view.datatype == SpecializedLiteralBackend::datatype);
        this->hash_ = calculate_hash(value);
    }

    [[nodiscard]] size_t hash() const noexcept {
        return hash_;
    }

    explicit operator View() const noexcept {
        return view::ValueLiteralBackendView{
                .datatype = SpecializedLiteralBackend::datatype,
                .value = rdf::util::Any{this->value}};
    }

public:
    struct Equal {
        using is_transparent = void;

        bool operator()(SpecializedLiteralBackend const *lhs, SpecializedLiteralBackend const *rhs) const noexcept {
            return lhs == rhs;
        }
        bool operator()(View const &lhs, SpecializedLiteralBackend const *rhs) const noexcept {
            assert(lhs.datatype == SpecializedLiteralBackend::datatype);
            return lhs.value.get_unchecked<typename T::cpp_type>() == rhs->value;
        }

        bool operator()(SpecializedLiteralBackend const *lhs, View const &rhs) const noexcept {
            assert(SpecializedLiteralBackend::datatype == rhs.datatype);
            return lhs->value == rhs.value.get_unchecked<typename T::cpp_type>();
        }
    };

    struct Hash {
        [[nodiscard]] size_t operator()(SpecializedLiteralBackend const *x) const noexcept {
            return x->hash();
        }
        [[nodiscard]] size_t operator()(View const &x) const noexcept {
            assert(x.datatype == SpecializedLiteralBackend::datatype);
            return calculate_hash(x.value.get_unchecked<typename T::cpp_type>());
        }
    };
};

}

#endif  //RDF4CPP_SPECIALIZEDLITERALBACKEND_HPP
