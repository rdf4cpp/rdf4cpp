#ifndef RDF4CPP_RDF_PARSER_PARSINGSTATE_HPP
#define RDF4CPP_RDF_PARSER_PARSINGSTATE_HPP

#include <rdf4cpp/bnode_mngt/NodeScope.hpp>
#include <rdf4cpp/bnode_mngt/reference_backends/scope_manager/ReferenceNodeScopeManager.hpp>
#include <rdf4cpp/bnode_mngt/reference_backends/scope_manager/SingleNodeScopeManager.hpp>
#include <rdf4cpp/IRIFactory.hpp>

#include <dice/sparse-map/sparse_map.hpp>
#include <dice/hash.hpp>

#include <string>
#include <string_view>
#include <optional>

namespace rdf4cpp::parser {

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
    using blank_node_generator_type = bnode_mngt::NodeGenerator;
    using blank_node_scope_type = bnode_mngt::NodeScope;
    using node_storage_type = storage::DynNodeStorage;
    using blank_node_scope_manager_type = bnode_mngt::INodeScopeManager;

    /**
     * The initial prefixes the parser has knowledge of
     * @note default value is an empty map
     */
    IRIFactory iri_factory{};

    /**
     * The node storage to put the parsed quads into
     */
    node_storage_type node_storage = storage::default_node_storage;

    blank_node_generator_type *blank_node_generator = &bnode_mngt::NodeGenerator::default_instance();
    blank_node_scope_manager_type *blank_node_scope_manager = &bnode_mngt::ReferenceNodeScopeManager::default_instance();
};

}  //namespace rdf4cpp::parser

#endif  //RDF4CPP_RDF_PARSER_PARSINGSTATE_HPP
