#ifndef RDF4CPP_CLOSEDNAMESPACE_HPP
#define RDF4CPP_CLOSEDNAMESPACE_HPP

#include <rdf4cpp/rdf/Namespace.hpp>

namespace rdf4cpp::rdf {

namespace detail {
template<typename T>
concept StringRange = std::ranges::range<T> and std::convertible_to<std::ranges::range_value_t<T>, std::string>;
}

class ClosedNamespace : public Namespace {

public:
    template<detail::StringRange Suffixes>
    ClosedNamespace(std::string_view namespace_iri, Suffixes all_suffixes, NodeStorage &node_storage)
        : Namespace(namespace_iri, node_storage) {
        for (auto const &suffix : all_suffixes)
            this->cache_[suffix] = IRI{namespace_iri_ + suffix, node_storage}.backend_handle();
    }

    IRI operator+(const std::string &suffix) override;

    void clear() override;
};

}  // namespace rdf4cpp::rdf

#endif  //RDF4CPP_CLOSEDNAMESPACE_HPP
