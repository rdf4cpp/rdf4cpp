#include "BlankNodeIdManagement_metall_common.hpp"

int main(int argc, char **argv) {
    assert(argc >= 2);

    std::cout << "phase 1:" << std::endl;

    auto const *path = argv[1];

    { // create segment
        metall::manager manager{metall::create_only, path};
    }

    { // open segment
        metall::manager manager{metall::open_only, path};

        auto generator_impl_ptr = manager.construct<PersistableGenerator>(generator_name)();
        NodeGenerator generator = NodeGenerator::new_instance_with_generator<PersistableGeneratorFrontend>(generator_impl_ptr);

        BlankNode bnode = generator.generate_node().as_blank_node();
        std::cout << bnode.backend_handle().id_to_underlying() << " " << bnode.identifier() << std::endl;
        assert(bnode.identifier() == "0");


        auto scope_impl_ptr = manager.construct<PersistableScope>(scope_name)(manager.get_allocator());
        NodeScope scope = NodeScope::new_instance<PersistableScopeFrontent>(scope_impl_ptr);
        manager.construct<identifier::NodeScopeID>(scope_id_name)(scope.id()); // save id for later

        BlankNode bnode2 = scope.get_or_generate_node("abc", generator).as_blank_node();
        std::cout << bnode2.backend_handle().id_to_underlying() << " " << bnode2.identifier() << std::endl;
        assert(bnode2.identifier() == "1");
    }
}
