#ifndef RDF4CPP_RDF_UTIL_WEAKBLANKNODEIDSCOPE_HPP
#define RDF4CPP_RDF_UTIL_WEAKBLANKNODEIDSCOPE_HPP

#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>

namespace rdf4cpp::rdf::bnode_mngt {

namespace identifier {

struct NodeScopeID {
    static constexpr size_t width = 16;
    using underlying_type = uint16_t;

private:
    underlying_type raw_ : width;

public:
    constexpr NodeScopeID() = default;
    explicit constexpr NodeScopeID(underlying_type raw) : raw_{raw} {}

    [[nodiscard]] constexpr underlying_type to_underlying() const noexcept {
        return raw_;
    }

    constexpr auto operator<=>(NodeScopeID const &) const noexcept = default;
};

} // namespace identifier

struct NodeScope;

struct WeakNodeScope {
private:
    friend struct NodeScope;
    friend struct std::hash<WeakNodeScope>;

    identifier::NodeScopeID backend_index_;
    size_t generation_;

    WeakNodeScope(identifier::NodeScopeID backend_index, size_t generation) noexcept;

public:
    /**
     * @return Identifier of this NodeScope
     */
    [[nodiscard]] identifier::NodeScopeID id() const noexcept;

    /**
     * Tries to upgrade this WeakNodeScope into a NodeScope.
     * This will only succeed if the corresponding INodeScope backend is still alive.
     *
     * @return a NodeScope pointing to the same backend as this, if the backend is still alive, otherwise nullopt
     */
    [[nodiscard]] std::optional<NodeScope> try_upgrade() const noexcept;

    /**
     * Tries to upgrade this WeakNodeScope into a NodeScope.
     * This will only succeed if the corresponding INodeScope is still alive.
     * This function throws on failure to upgrade.
     *
     * @return a NodeScope pointing to the same backend as this, if the backend is still alive
     * @throws std::runtime_error on upgrade failure
     */
    [[nodiscard]] NodeScope upgrade() const;

    /**
     * @return whether this and other are referring to the same backend
     */
    auto operator<=>(WeakNodeScope const &other) const noexcept = default;
};

} // rdf4cpp::rdf::bnode_mngt

template<>
struct std::hash<rdf4cpp::rdf::bnode_mngt::WeakNodeScope> {
    size_t operator()(rdf4cpp::rdf::bnode_mngt::WeakNodeScope const &scope) const noexcept;
};

#endif // RDF4CPP_RDF_UTIL_WEAKBLANKNODEIDSCOPE_HPP
