#ifndef RDF4CPP_DATASETSTORAGE_HPP
#define RDF4CPP_DATASETSTORAGE_HPP
#include <rdf4cpp/rdf/storage/tuple/IDatasetBackend.hpp>
#include <utility>

namespace rdf4cpp::rdf::storage::tuple {

class DatasetStorage {
    using PatternSolutions = rdf4cpp::rdf::query::SolutionSequence;
    using QuadPattern = rdf4cpp::rdf::query::QuadPattern;

    friend IDatasetBackend;
    std::shared_ptr<IDatasetBackend> backend_{};

    DatasetStorage() = default;

    explicit DatasetStorage(std::shared_ptr<IDatasetBackend> dataset_backend);
    static inline std::once_flag default_init_once_flag;
    static DatasetStorage default_instance_;

public:
    static DatasetStorage &primary_instance();

    template<typename BackendImpl, typename... Args>
    static DatasetStorage new_instance(Args... args) {
        return DatasetStorage(std::make_shared<BackendImpl>(args...));
    }

    static DatasetStorage new_instance();

    void add(const Quad &quad);

    [[nodiscard]] bool contains(const Quad &quad) const;
    [[nodiscard]] size_t size() const;
    [[nodiscard]] PatternSolutions match(const QuadPattern &quad_pattern) const;
    [[nodiscard]] size_t size(const IRI &graph_name) const;

    using const_iterator = IDatasetBackend::const_iterator;

    [[nodiscard]] const_iterator begin() const;
    [[nodiscard]] const_iterator end() const;

    [[nodiscard]] node::NodeStorage &node_storage() const;
};
}  // namespace rdf4cpp::rdf::storage::tuple

#endif  //RDF4CPP_DATASETSTORAGE_HPP
