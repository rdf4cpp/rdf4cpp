#ifndef RDF4CPP_SOLUTIONSEQUENCE_HPP
#define RDF4CPP_SOLUTIONSEQUENCE_HPP

#include <rdf4cpp/rdf/Quad.hpp>
#include <rdf4cpp/rdf/query/QuadPattern.hpp>
#include <rdf4cpp/rdf/query/Solution.hpp>
#include <rdf4cpp/rdf/storage/tuple/ISolutionSequenceBackend.hpp>

#include <iostream>
#include <set>
#include <utility>

namespace rdf4cpp::rdf::query {

class SolutionSequence {
protected:
    std::shared_ptr<storage::tuple::ISolutionSequenceBackend> backend_;

    explicit SolutionSequence(std::shared_ptr<storage::tuple::ISolutionSequenceBackend> backend) : backend_(std::move(backend)) {}

public:
    // TODO: constructors
    template<typename BackendImpl, typename... Args>
    static inline SolutionSequence new_instance(Args... args) {
        return SolutionSequence(
                std::make_shared<BackendImpl>(args...));
    }

    const QuadPattern &pattern();

    [[nodiscard]] size_t variable_count() const;

    using const_iterator = storage::tuple::ISolutionSequenceBackend::const_iterator;

    [[nodiscard]] const_iterator begin() const;

    [[nodiscard]] const_iterator end() const;
};

}  // namespace rdf4cpp::rdf::query

#endif  //RDF4CPP_SOLUTIONSEQUENCE_HPP
