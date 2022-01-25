#ifndef RDF4CPP_ISOLUTIONSEQUENCEBACKEND_HPP
#define RDF4CPP_ISOLUTIONSEQUENCEBACKEND_HPP

#include <rdf4cpp/rdf/Quad.hpp>
#include <rdf4cpp/rdf/query/QuadPattern.hpp>
#include <rdf4cpp/rdf/query/Solution.hpp>

#include <iostream>
#include <set>
#include <utility>

namespace rdf4cpp::rdf::storage::tuple {

class ISolutionSequenceBackend {
protected:
    using QuadPattern = query::QuadPattern;
    using Solution = query::Solution;
    // TODO: generalize SolutionSequence to support not only QuadPattern but any SPARQL backend
    QuadPattern pattern_{};


public:
    explicit ISolutionSequenceBackend(QuadPattern pattern);
    ISolutionSequenceBackend() = default;

    virtual ~ISolutionSequenceBackend() = 0;
    virtual const QuadPattern &pattern();

    size_t variable_count() const;

    struct const_iterator {
        // from https://stackoverflow.com/questions/35866041/returning-different-iterators-with-virtual-derived-methods
        using value_type = Solution;

        struct I_const_iterator {
            virtual void next(int n) = 0;
            [[nodiscard]] virtual const value_type &deref() const = 0;
            [[nodiscard]] virtual bool equal(const void *other) const = 0;
            [[nodiscard]] virtual std::unique_ptr<I_const_iterator> clone() const = 0;
            [[nodiscard]] virtual const std::type_info &type() const = 0;
            [[nodiscard]] virtual const void *address() const = 0;
            virtual ~I_const_iterator() = default;
        };

        template<class Iter>
        requires std::forward_iterator<Iter>
        struct const_iterator_impl : I_const_iterator {
            explicit const_iterator_impl(Iter iter) : _iter(iter) {}

            void next(int n) override { _iter = std::next(_iter, n); }
            [[nodiscard]] const value_type &deref() const override { return *_iter; }
            [[nodiscard]] bool equal(const void *rp) const override { return _iter == static_cast<const const_iterator_impl *>(rp)->_iter; }
            [[nodiscard]] std::unique_ptr<I_const_iterator> clone() const override { return std::make_unique<const_iterator_impl>(*this); }
            [[nodiscard]] const std::type_info &type() const override { return typeid(_iter); }
            [[nodiscard]] const void *address() const override { return this; }


            Iter _iter;
        };

        std::unique_ptr<I_const_iterator> _impl;

    public:
        // interface

        template<class Iter>
        requires std::forward_iterator<Iter>
        explicit const_iterator(Iter iter) : _impl(std::make_unique<const_iterator_impl<Iter>>(iter)) {}

        const_iterator(const const_iterator &r) : _impl(r._impl->clone()){};

        const value_type &operator*() const;

        const_iterator &operator++();

        bool operator==(const const_iterator &r) const;

        bool operator!=(const const_iterator &r) const;
    };

    virtual const_iterator begin() const = 0;

    virtual const_iterator end() const = 0;
};
}  // namespace rdf4cpp::rdf::storage::tuple


#endif  //RDF4CPP_ISOLUTIONSEQUENCEBACKEND_HPP
