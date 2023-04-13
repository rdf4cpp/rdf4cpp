#ifndef RDF4CPP_RDF_PARSER_PARSINGSTATE_HPP
#define RDF4CPP_RDF_PARSER_PARSINGSTATE_HPP

#include <rdf4cpp/rdf/bnode_management/NodeScope.hpp>
#include <rdf4cpp/rdf/bnode_management/reference_backends/scope_manager/ReferenceNodeScopeManager.hpp>
#include <rdf4cpp/rdf/storage/util/robin-hood-hashing/robin_hood_hash.hpp>
#include <rdf4cpp/rdf/storage/util/tsl/sparse_map.h>

#include <string>
#include <string_view>
#include <optional>

namespace rdf4cpp::rdf::parser {

/**
 * The publicly known internal state of the IStreamQuadIterator.
 * Values of this type can be used to specify the initial state of IStreamQuadIterators, and
 * therefore states can be reused between different instantiations.
 *
 * @example
 * @code
 * IStreamQuadIterator::state_type state{};
 * {
 *     std::ifstream ifs_1{"some_file.txt"};
 *     IStreamQuadIterator qit_1{ifs_1, ParsingFlags::none(), &state};
 *     // consume qit_1 ...
 * }
 *
 * {
 *     std::ifstream ifs_2{"some_other_file.txt"};
 *     IStreamQuadIterator qit_2{ifs_2, ParsingFlags::none(), &state};
 *     // consume qit_2 with state inherited from qit_1 ...
 * }
 * @endcode
 */
struct ParsingState {
    using prefix_storage_type = rdf4cpp::rdf::storage::util::tsl::sparse_map<
            std::string,
            std::string,
            rdf4cpp::rdf::storage::util::robin_hood::hash<std::string_view>,
            std::equal_to<>>;

    using blank_node_generator_type = util::NodeGenerator;
    using blank_node_scope_type = util::NodeScope;
    using node_storage_type = storage::node::NodeStorage;
    using blank_node_scope_manager_type = util::INodeScopeManager;

    /**
     * The initial prefixes the parser has knowledge of
     * @note default value is an empty map
     */
    prefix_storage_type prefixes{};

    /**
     * The node storage to put the parsed quads into
     */
    node_storage_type node_storage = storage::node::NodeStorage::default_instance();

    blank_node_generator_type *blank_node_generator = &util::NodeGenerator::default_instance();
    blank_node_scope_manager_type *blank_node_scope_manager = &util::ReferenceNodeScopeManager::default_instance();
};

}  //namespace rdf4cpp::rdf::parser

#endif  //RDF4CPP_RDF_PARSER_PARSINGSTATE_HPP
