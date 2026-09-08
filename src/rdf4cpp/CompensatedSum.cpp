#include "CompensatedSum.hpp"

#include <rdf4cpp/datatypes/registry/DatatypeRegistry.hpp>
#include <rdf4cpp/datatypes/xsd/integers/signed/Integer.hpp>

namespace rdf4cpp {

CompensatedSum::CompensatedSum(storage::DynNodeStoragePtr node_storage)
    : node_storage_{node_storage} {
}

bool CompensatedSum::is_exact(IRI const &datatype) {
    if (datatype.null()) {
        return false; // the null-value, let the deferred ops propagate it
    }

    if (datatype == cached_datatype_) {
        return cached_exact_;
    }

    auto const *entry = datatypes::registry::DatatypeRegistry::get_entry(datatypes::registry::DatatypeIDView{datatype});

    cached_datatype_ = datatype;
    cached_exact_ = entry != nullptr && entry->numeric_ops.has_value() && entry->numeric_ops->is_exact;
    return cached_exact_;
}

void CompensatedSum::add(Literal const &lit, uint64_t multiplicity) {
    this->add(make_deferred_from_literal(lit), multiplicity);
}

// value * multiplicity as the sum of value * 2^i over the set bits of multiplicity. Doubling is exact
// in binary floating point (bar overflow, where the product overflows too), so every term is exact and
// only the compensated additions round
void CompensatedSum::add(DeferredLiteral const &value, uint64_t multiplicity) {
    if (multiplicity == 1) {
        add_once(value);
        return;
    }

    add_impl(value, multiplicity);
}

void CompensatedSum::add(DeferredLiteral &&value, uint64_t multiplicity) {
    if (multiplicity == 1) {
        add_once(value);
        return;
    }

    add_impl(std::move(value), multiplicity);
}

void CompensatedSum::add_impl(DeferredLiteral value, uint64_t multiplicity) {
    for (; multiplicity != 0; multiplicity >>= 1u) {
        if ((multiplicity & uint64_t{1}) != 0) {
            add_once(value);
        }
        if (multiplicity > 1) {
            value = numeric_add_deferred(value, value, node_storage_);
        }
    }
}

void CompensatedSum::add_once(DeferredLiteral const &value) {
    if (!sum_.has_value()) {
        // the first value seeds the sum; adding it to a "0"^^xsd:integer instead would poison
        // owl:rational and owl:real, which have no common numeric type with xsd:integer
        sum_ = value;
        compensating_ = !is_exact(value.datatype);
        return;
    }

    if (!compensating_ && is_exact(value.datatype)) {
        // arbitrary precision, so there is no rounding error to carry
        sum_ = numeric_add_deferred(*sum_, value, node_storage_);
        return;
    }

    compensating_ = true;

    // Kahan-Babuska-Neumaier: collect the loss of each addition on the side and add it back in value().
    auto [new_sum, loss] = numeric_add_with_loss_deferred(*sum_, value, node_storage_);

    // a null loss is one there is nothing to account for: the total went infinite, or it is
    // poisoned and sum_ carries that on. comp_ itself is the null-value until the first loss seeds it
    if (!loss.null()) {
        comp_ = comp_.null() ? loss : numeric_add_deferred(comp_, loss, node_storage_);
    }

    sum_ = std::move(new_sum);
}

Literal CompensatedSum::value() const {
    if (!sum_.has_value()) {
        return Literal::make_typed_from_value<datatypes::xsd::Integer>(0);
    }

    auto const final_result = [&] {
        if (comp_.null()) {
            return materialize_deferred(*sum_, node_storage_);  // nothing was lost (yet)
        }

        return materialize_deferred(numeric_add_deferred(*sum_, comp_, node_storage_), node_storage_);
    }();

    if (!final_result.is_numeric()) {
        return Literal{};
    }

    return final_result;
}

}  // namespace rdf4cpp
