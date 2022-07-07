set(CMAKE_CXX_STANDARD 20)

add_executable(graphs_and_datasets_example graphs_and_datasets.cpp)
target_link_libraries(graphs_and_datasets_example PRIVATE
        rdf4cpp::rdf4cpp
        )

add_executable(literal_datatypes_example literal_datatypes.cpp)
target_link_libraries(literal_datatypes_example PRIVATE
        rdf4cpp::rdf4cpp
        )

add_executable(rdf_nodes_example rdf_nodes.cpp)
target_link_libraries(rdf_nodes_example PRIVATE
        rdf4cpp::rdf4cpp
        )

add_executable(recurring_partial_date_datatypes_example recurring_partial_date_datatypes.cpp)
target_link_libraries(recurring_partial_date_datatypes_example PRIVATE
        rdf4cpp::rdf4cpp
        )
