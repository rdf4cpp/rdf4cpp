#include "CompensatedSum.hpp"

#include <rdf4cpp/datatypes/registry/DatatypeRegistry.hpp>
#include <rdf4cpp/datatypes/xsd/integers/signed/Integer.hpp>

namespace rdf4cpp {

CompensatedSum::CompensatedSum(storage::DynNodeStoragePtr node_storage)
    : node_storage_{node_storage} {
}

datatypes::registry::DatatypeRegistry::DatatypeEntry const *CompensatedSum::datatype_entry(IRI const &datatype) {
    if (datatype.null()) {
        return nullptr;  // the null-value, let the deferred ops propagate it
    }

    if (datatype != cached_datatype_) {
        cached_datatype_ = datatype;
        cached_entry_ = datatypes::registry::DatatypeRegistry::get_entry(datatype);
    }

    return cached_entry_;
}

bool CompensatedSum::is_exact(IRI const &datatype) {
    auto const *e = datatype_entry(datatype);
    return e != nullptr && e->numeric_ops.has_value() && e->numeric_ops->is_exact;
}

bool CompensatedSum::is_inf(DeferredValue const &value) {
    auto const *e = datatype_entry(value.second);
    return e != nullptr && e->numeric_ops.has_value() && e->numeric_ops->is_impl() && e->numeric_ops->get_impl().is_inf_fptr(value.first);
}

void CompensatedSum::add(Literal const &lit) {
    this->add(make_deferred_from_literal(lit));
}

void CompensatedSum::add(DeferredValue const &value) {
    if (!sum_.has_value()) {
        // the first value seeds the sum; adding it to a "0"^^xsd:integer instead would poison
        // owl:rational and owl:real, which have no common numeric type with xsd:integer
        sum_ = value;
        compensating_ = !is_exact(value.second);
        return;
    }

    if (!compensating_ && is_exact(value.second)) {
        // arbitrary precision, so there is no rounding error to carry
        sum_ = numeric_add_deferred(*sum_, value, node_storage_);
        return;
    }

    compensating_ = true;

    auto const t = numeric_add_deferred(*sum_, value, node_storage_);

    // Neumaier: accumulate what the additions lose instead of correcting the next value with it,
    // which keeps the correction even if that value dwarfs the running total. The loss is Knuth's
    // two-sum, exact for either order of magnitude, so no comparison of the two is needed.
    // Skipped once the total is infinite: its loss would be inf - inf = NaN and poison the sum.
    if (!is_inf(t)) {
        auto const z = numeric_sub_deferred(t, *sum_, node_storage_);
        auto const loss = numeric_add_deferred(numeric_sub_deferred(*sum_, numeric_sub_deferred(t, z, node_storage_), node_storage_),
                                               numeric_sub_deferred(value, z, node_storage_),
                                               node_storage_);

        comp_ = comp_.second.null() ? loss : numeric_add_deferred(comp_, loss, node_storage_);
    }

    sum_ = t;
}

Literal CompensatedSum::value() const {
    if (!sum_.has_value()) {
        return Literal::make_typed_from_value<datatypes::xsd::Integer>(0);
    }

    if (comp_.second.null()) {
        return materialize_deferred(*sum_, node_storage_);  // nothing was lost (yet)
    }

    return materialize_deferred(numeric_add_deferred(*sum_, comp_, node_storage_), node_storage_);
}

}  // namespace rdf4cpp
