#include "BlankNodeIdManagement_metall_common.hpp"

int main(int argc, char **argv) {
    assert(argc >= 2);

    std::cout << "phase 2:" << std::endl;

    auto const *path = argv[1];

    { // restore previous node storage state (have no persistent node storage here)
        BlankNode b1{"0"};
        BlankNode b2{"1"};
    }

    {
        metall::manager manager{metall::open_only, path};

        auto generator_impl_ptr = std::get<0>(manager.find<PersistableGenerator>(generator_name));
        NodeGenerator generator = NodeGenerator::new_instance_with_generator<PersistableGeneratorFrontend>(generator_impl_ptr);

        BlankNode bnode = generator.generate_node().as_blank_node();
        std::cout << bnode.backend_handle().raw() << " " << bnode.identifier() << std::endl;
        assert(bnode.identifier() == "2");

        auto scope_id = *std::get<0>(manager.find<identifier::NodeScopeID>(scope_id_name));
        auto scope_impl_ptr = std::get<0>(manager.find<PersistableScope>(scope_name));
        NodeScope scope = NodeScope::new_instance_at<PersistableScopeFrontent>(scope_id, scope_impl_ptr);

        BlankNode bnode2 = scope.get_or_generate_node("abc", generator).as_blank_node();
        std::cout << bnode2.backend_handle().raw() << " " << bnode2.identifier() << std::endl;
        assert(bnode2.identifier() == "1");

        BlankNode bnode3 = scope.get_or_generate_node("def", generator).as_blank_node();
        std::cout << bnode3.backend_handle().raw() << " " << bnode3.identifier() << std::endl;
        assert(bnode3.identifier() == "3");
    }

    metall::manager::remove(path);
}
