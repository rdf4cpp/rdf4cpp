#ifndef RDF4CPP_DEFAULTSOLUTIONSEQUENCEBACKEND_HPP
#define RDF4CPP_DEFAULTSOLUTIONSEQUENCEBACKEND_HPP
#include <rdf4cpp/rdf/storage/tuple/ISolutionSequenceBackend.hpp>

#include <dice/sparse-map/sparse_set.hpp>
#include <dice/hash.hpp>

namespace rdf4cpp::rdf::storage::tuple {

class DefaultSolutionSequenceBackend : public ISolutionSequenceBackend {
    using QuadPattern = query::QuadPattern;
    using SolutionSequence = query::SolutionSequence;
    using Solution = query::Solution;
    dice::sparse_map::sparse_set<Quad, dice::hash::DiceHash<Quad>> const *quads_{};

public:
    DefaultSolutionSequenceBackend() = default;
    DefaultSolutionSequenceBackend(QuadPattern pattern, dice::sparse_map::sparse_set<Quad, dice::hash::DiceHash<Quad>> const *quads);
    ~DefaultSolutionSequenceBackend() override = default;

    [[nodiscard]] ISolutionSequenceBackend::const_iterator begin() const override;
    [[nodiscard]] ISolutionSequenceBackend::const_iterator end() const override;

    struct const_iterator {
        using difference_type = std::ptrdiff_t;
        using value_type = Solution;
        dice::sparse_map::sparse_set<Quad, dice::hash::DiceHash<Quad>>::const_iterator iter_;
        dice::sparse_map::sparse_set<Quad, dice::hash::DiceHash<Quad>>::const_iterator end_;
        QuadPattern pattern_;
        mutable Solution solution_;


        const_iterator() = default;
        const_iterator(dice::sparse_map::sparse_set<Quad, dice::hash::DiceHash<Quad>> const *data, const QuadPattern &pattern);

        const_iterator(const QuadPattern &pattern, dice::sparse_map::sparse_set<Quad, dice::hash::DiceHash<Quad>> const *data);

        bool ended() const;
        void forward_to_solution();
        bool is_solution() const;

        const value_type &operator*() const;

        const_iterator &operator++();

        const_iterator operator++(int) &;

        bool operator==(const const_iterator &r) const;

        bool operator!=(const const_iterator &r) const;
    };
};

}  // namespace rdf4cpp::rdf::storage::tuple

#endif  //RDF4CPP_DEFAULTSOLUTIONSEQUENCEBACKEND_HPP
