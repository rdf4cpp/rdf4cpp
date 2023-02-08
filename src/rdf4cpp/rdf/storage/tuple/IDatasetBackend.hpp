#ifndef RDF4CPP_IDATASETBACKEND_HPP
#define RDF4CPP_IDATASETBACKEND_HPP

#include <rdf4cpp/rdf/Quad.hpp>
#include <rdf4cpp/rdf/query/QuadPattern.hpp>
#include <rdf4cpp/rdf/query/SolutionSequence.hpp>
#include <rdf4cpp/rdf/storage/node/NodeStorage.hpp>

#include <algorithm>
#include <set>

namespace rdf4cpp::rdf::storage::tuple {


class IDatasetBackend {
    using PatternSolutions = rdf4cpp::rdf::query::SolutionSequence;
    using QuadPattern = rdf4cpp::rdf::query::QuadPattern;

protected:
    mutable node::NodeStorage node_storage_;

public:
    explicit IDatasetBackend(node::NodeStorage &node_storage = node::NodeStorage::default_instance());

    virtual ~IDatasetBackend() = 0;
    virtual void add(const Quad &quad) = 0;
    [[nodiscard]] virtual node::NodeStorage &node_storage() const = 0;
    [[nodiscard]] virtual bool contains(const Quad &quad) const = 0;
    [[nodiscard]] virtual size_t size() const = 0;
    [[nodiscard]] virtual PatternSolutions match(const QuadPattern &quad_pattern) const = 0;
    [[nodiscard]] virtual size_t size(const IRI &graph_name) const = 0;

    struct const_iterator {
        // from https://stackoverflow.com/questions/35866041/returning-different-iterators-with-virtual-derived-methods
        using value_type = Quad;

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

        const_iterator(const const_iterator &r);

        const value_type &operator*() const;

        const_iterator &operator++();

        bool operator==(const const_iterator &r) const;

        bool operator!=(const const_iterator &r) const;
    };

    [[nodiscard]] virtual const_iterator begin() const = 0;
    [[nodiscard]] virtual const_iterator end() const = 0;
};


}  // namespace rdf4cpp::rdf::storage::tuple


#endif  //RDF4CPP_IDATASETBACKEND_HPP
