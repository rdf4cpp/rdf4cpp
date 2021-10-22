#ifndef RDF4CPP_NTRIPLEPARSER_HPP
#define RDF4CPP_NTRIPLEPARSER_HPP

#include <rdf4cpp/rdf/Statement.hpp>
#include <rdf4cpp/rdf/storage/node/NodeStorage.hpp>

namespace rdf4cpp::rdf::parser {

class NTripleParser {
    std::string file_name_;

    storage::node::NodeStorage node_storage_;

public:
    NTripleParser(const std::string &fileName, const storage::node::NodeStorage &nodeStorage = storage::node::NodeStorage::primary_instance())
        : file_name_(fileName), node_storage_(nodeStorage) {}

    class iterator {
        bool ended_ = true;
        std::string file_name_;

        storage::node::NodeStorage node_storage_;

        Statement current_statement_;


    public:
        iterator() = default;

        Statement next_statement() {
            // must set ended_=false if ended
            return {};
        }

        iterator(const std::string &fileName, const storage::node::NodeStorage &nodeStorage) : file_name_(fileName), node_storage_(nodeStorage) {
            current_statement_ = next_statement();
        }

        iterator &operator++() {
            current_statement_ = next_statement();
            return *this;
        }

        iterator operator++(int) {
            auto old = *this;
            ++(*this);
            return old;
        }

        const Statement &operator*() const {
            return current_statement_;
        }


        operator bool() {
            return not ended_;
        }
    };

    iterator begin() {
        return iterator{file_name_, node_storage_};  // ...
    }

    bool end() {
        return false;
    }
};
}  // namespace rdf4cpp::rdf::parser

#endif  //RDF4CPP_NTRIPLEPARSER_HPP
