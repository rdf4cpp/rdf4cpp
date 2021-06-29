#include "IDatasetBackend.hpp"

#include <utility>

namespace rdf4cpp::rdf::storage::tuple {

IDatasetBackend::IDatasetBackend(node::NodeStorage node_storage) : node_storage_(std::move(node_storage)){}

IDatasetBackend::~IDatasetBackend() = default;
}