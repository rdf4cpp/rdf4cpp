#ifndef RDF4CPP_SPECIALIZEDLITERALBACKEND_HPP
#define RDF4CPP_SPECIALIZEDLITERALBACKEND_HPP

#include <rdf4cpp/rdf/util/Any.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>
#include <rdf4cpp/rdf/storage/node/view/LiteralBackendView.hpp>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

template<typename T>
class SpecializedLiteralBackend {
    identifier::LiteralType datatype_;
    T value_;
    size_t hash_;

public:
    using View = view::AnyBackendView;

    SpecializedLiteralBackend(T value, identifier::LiteralType datatype) noexcept : datatype_{datatype},
                                                                                    value_{std::move(value)},
                                                                                    hash_{View(*this).hash()} {
    }

    explicit SpecializedLiteralBackend(view::AnyBackendView const &view) noexcept : datatype_{view.datatype},
                                                                                    value_{view.value.get_unchecked<T>()},
                                                                                    hash_{view.hash()} {
    }

    [[nodiscard]] T const &value() const noexcept {
        return value_;
    }

    [[nodiscard]] identifier::LiteralType datatype() const noexcept {
        return datatype_;
    }

    [[nodiscard]] size_t hash() const noexcept {
        return hash_;
    }

    explicit operator view::AnyBackendView() const noexcept {
        return view::AnyBackendView {
                .datatype = this->datatype_,
                .value = rdf::util::Any{this->value_}};
    }
};

}

#endif  //RDF4CPP_SPECIALIZEDLITERALBACKEND_HPP
