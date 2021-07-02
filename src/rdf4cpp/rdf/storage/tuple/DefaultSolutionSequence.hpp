#ifndef RDF4CPP_DEFAULTSOLUTIONSEQUENCE_HPP
#define RDF4CPP_DEFAULTSOLUTIONSEQUENCE_HPP
#include <rdf4cpp/rdf/storage/tuple/SolutionSequenceBackend.hpp>

namespace rdf4cpp::rdf::storage::tuple {

class DefaultSolutionSequence : public SolutionSequenceBackend {
    using QuadPattern = query::QuadPattern;
    using SolutionSequence = query::SolutionSequence;
    using Solution = query::Solution;
    std::set<Quad> const *quads_{};


public:
    DefaultSolutionSequence() = default;
    DefaultSolutionSequence(QuadPattern pattern, std::set<Quad> const *quads) : SolutionSequenceBackend(pattern), quads_{quads} {
    }
    ~DefaultSolutionSequence() override = default;

    [[nodiscard]] SolutionSequenceBackend::const_iterator begin() const override {
        return SolutionSequenceBackend::const_iterator{const_iterator{quads_, pattern_}};
    }
    [[nodiscard]] SolutionSequenceBackend::const_iterator end() const override {
        return SolutionSequenceBackend::const_iterator{const_iterator{pattern_, quads_}};
    }

    struct const_iterator {
        using difference_type = std::ptrdiff_t;
        using value_type = Solution;
        std::set<Quad>::const_iterator iter_;
        std::set<Quad>::const_iterator end_;
        QuadPattern pattern_;
        mutable Solution solution_;


        const_iterator() = default;
        const_iterator(std::set<rdf4cpp::rdf::Quad> const *data, const QuadPattern &pattern)
            : iter_(data->begin()), end_(data->end()), pattern_(pattern),
              solution_{pattern} {
            if (not ended())
                forward_to_solution();
        }

        const_iterator(const QuadPattern &pattern, std::set<rdf4cpp::rdf::Quad> const *data)
            : iter_(data->end()), end_(data->end()), pattern_(pattern) {}

        bool ended() const {
            return iter_ == end_;
        }
        void forward_to_solution() {
            while (not is_solution()) {
                if (not ended())
                    iter_++;
                else
                    break;
            }
        }
        bool is_solution() const {
            auto pattern_it = pattern_.begin();
            size_t solution_pos = 0;
            for (const auto &entry : *iter_) {
                const auto &pattern_entry = *pattern_it;
                if (pattern_entry.is_variable()) {
                    solution_[solution_pos++] = entry;
                } else {
                    if (entry != pattern_entry)
                        return false;
                }
                pattern_it++;
            }
            return true;
        }

        const value_type &operator*() const {
            return solution_;
        }

        const_iterator &operator++() {
            iter_++;
            forward_to_solution();
            return *this;
        }

        const_iterator operator++(int) & {
            auto copy = const_iterator(*this);
            iter_++;
            forward_to_solution();
            return copy;
        }

        bool operator==(const const_iterator &r) const {
            return this->iter_ == r.iter_;
        }

        bool operator!=(const const_iterator &r) const {
            return not(*this == r);
        };
    };
};

}  // namespace rdf4cpp::rdf::storage::tuple

#endif  //RDF4CPP_DEFAULTSOLUTIONSEQUENCE_HPP
