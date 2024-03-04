#include "Graph.hpp"
#include <rdf4cpp/rdf/Dataset.hpp>
#include <rdf4cpp/rdf/writer/TryWrite.hpp>
#include <rdf4cpp/rdf/writer/SerializationState.hpp>

#include <utility>

namespace rdf4cpp::rdf {

storage::node::identifier::NodeBackendID Graph::to_node_id(Node node) noexcept {
    return node.backend_handle().id();
}

Node Graph::to_node(storage::node::identifier::NodeBackendID id) const noexcept {
    return Node{storage::node::identifier::NodeBackendHandle{id, node_storage_}};
}

Graph::Graph(storage::node::DynNodeStorage node_storage) noexcept : node_storage_{node_storage} {
}

void Graph::add(Statement const &stmt_) {
    auto stmt = stmt_.to_node_storage(node_storage_);
    triples_.emplace(to_node_id(stmt.subject()), to_node_id(stmt.predicate()), to_node_id(stmt.object()));
}

bool Graph::contains(Statement const &stmt_) const noexcept {
    auto const stmt = stmt_.try_get_in_node_storage(node_storage_);
    return triples_.contains(triple{to_node_id(stmt.subject()), to_node_id(stmt.predicate()), to_node_id(stmt.object())});
}

Graph::iterator Graph::begin() const noexcept {
    return iterator{triples_.begin(), triples_.end(), this};
}

Graph::sentinel Graph::end() const noexcept {
    return sentinel{};
}

Graph::solution_sequence Graph::match(query::TriplePattern const &triple_pattern) const noexcept {
    return solution_sequence{solution_iterator{triples_.begin(), triples_.end(), this, triple_pattern}};
}

size_t Graph::size() const noexcept {
    return triples_.size();
}

bool Graph::serialize(writer::BufWriterParts const writer) const noexcept {
    for (auto const &[s, p, o] : triples_) {
        Quad q{to_node(s), to_node(p), to_node(o)};
        if (!q.serialize_ntriples(writer)) {
            return false;
        }
    }

    return true;
}

bool Graph::serialize_turtle(writer::SerializationState &state, writer::BufWriterParts const writer) const noexcept {
    for (auto const &[s, p, o] : triples_) {
        Quad q{to_node(s), to_node(p), to_node(o)};
        if (!q.serialize_turtle(state, writer)) {
            return false;
        }
    }

    return true;
}

bool Graph::serialize_turtle(writer::BufWriterParts const writer) const noexcept {
    writer::SerializationState st{};
    if (!st.begin(writer)) {
        return false;
    }

    if (!serialize_turtle(st, writer)) {
        return false;
    }

    return st.flush(writer);
}

std::ostream &operator<<(std::ostream &os, Graph const &graph) {
    writer::BufOStreamWriter w{os};
    graph.serialize(w);
    w.finalize();
    return os;
}

Graph::iterator::iterator(typename triple_storage_type::const_iterator beg, typename triple_storage_type::const_iterator end, Graph const *parent) noexcept : iter_{beg},
                                                                                                                                                              end_{end},
                                                                                                                                                              parent_{parent} {
    if (iter_ != end_) {
        cur_ = Statement{parent_->to_node(iter_->subject), parent_->to_node(iter_->predicate), parent_->to_node(iter_->object)};
    }
}

Graph::iterator &Graph::iterator::operator++() noexcept {
    ++iter_;
    if (iter_ != end_) {
        cur_ = Statement{parent_->to_node(iter_->subject), parent_->to_node(iter_->predicate), parent_->to_node(iter_->object)};
    }

    return *this;
}

Graph::reference Graph::iterator::operator*() const noexcept {
    return cur_;
}

Graph::pointer Graph::iterator::operator->() const noexcept {
    return &cur_;
}

bool Graph::iterator::operator==(Graph::sentinel) const noexcept {
    return iter_ == end_;
}

bool Graph::iterator::operator!=(Graph::sentinel) const noexcept {
    return !(*this == Graph::sentinel{});
}

bool Graph::solution_iterator::check_solution() noexcept {
    auto pat_it = pat_.begin();
    auto out_it = cur_.begin();

    Statement const stmt{parent_->to_node(iter_->subject),
                         parent_->to_node(iter_->predicate),
                         parent_->to_node(iter_->object)};

    for (auto const x : stmt) {
        if (pat_it->is_variable() || *pat_it == x) {
            *out_it = x;
        } else {
            return false;
        }

        ++pat_it;
    }

    return true;
}

void Graph::solution_iterator::forward_to_solution() noexcept {
    while (iter_ != end_) {
        if (check_solution()) {
            return;
        }

        ++iter_;
    }
}

Graph::solution_iterator::solution_iterator(typename triple_storage_type::const_iterator beg, typename triple_storage_type::const_iterator end, Graph const *parent, query::TriplePattern const &pat) noexcept
    : iter_{beg}, end_{end}, parent_{parent}, pat_{pat} {
    forward_to_solution();
}

Graph::solution_iterator &Graph::solution_iterator::operator++() noexcept {
    ++iter_;
    forward_to_solution();
}

Graph::solution_iterator::reference Graph::solution_iterator::operator*() const noexcept {
    return cur_;
}

Graph::solution_iterator::pointer Graph::solution_iterator::operator->() const noexcept {
    return &cur_;
}

bool Graph::solution_iterator::operator==(Graph::sentinel) const noexcept {
    return iter_ == end_;
}

bool Graph::solution_iterator::operator!=(Graph::sentinel) const noexcept {
    return !(*this == Graph::sentinel{});
}

}  // namespace rdf4cpp::rdf
