include_directories(${EIGEN3_INCLUDE_DIR} ${orocos_kdl_INCLUDE_DIRS} ${kdl_codyco_INCLUDE_DIRS} )


add_executable(inverse_inertia_test inverse_inertia.cpp)
add_executable(regressor_utils_test regressor_utils_test.cpp)
add_executable(subtree_regressor_test subtree_regressor_test.cpp testhumanoid.cpp)
add_executable(inversedynamics_regressor_test inversedynamics_regressor_test.cpp testhumanoid.cpp)
add_executable(inversedynamics_crba_test inversedynamics_crba_test.cpp testhumanoid.cpp testsinglelink.cpp testsinglejoint.cpp testkukalwr.cpp)
add_executable(comjacobian_test comjacobian_test.cpp testhumanoid.cpp)
add_executable(inversedynamics_different_loops_test inversedynamics_different_loops_test.cpp testhumanoid.cpp)

#set_source_files_properties(sparse_numerical_subspace_test.cpp PROPERTIES COMPILE_FLAGS -std=c++11)
#add_executable(sparse_numerical_subspace_test sparse_numerical_subspace_test.cpp testhumanoid.cpp testdoublejoint.cpp testsinglelink.cpp testsinglejoint.cpp testkukalwr.cpp)

target_link_libraries(inverse_inertia_test ${orocos_kdl_LIBRARIES} kdl-codyco )
target_link_libraries(regressor_utils_test ${orocos_kdl_LIBRARIES} kdl-codyco )
target_link_libraries(subtree_regressor_test ${orocos_kdl_LIBRARIES} kdl-codyco  )
target_link_libraries(inversedynamics_regressor_test ${orocos_kdl_LIBRARIES} kdl-codyco  )
target_link_libraries(inversedynamics_crba_test ${orocos_kdl_LIBRARIES} kdl-codyco  )
target_link_libraries(comjacobian_test ${orocos_kdl_LIBRARIES} kdl-codyco )
target_link_libraries(inversedynamics_different_loops_test ${orocos_kdl_LIBRARIES} kdl-codyco  )
#target_link_libraries(sparse_numerical_subspace_test ${orocos_kdl_LIBRARIES} kdl-codyco  )


add_test(test_inverse_inertia inverse_inertia_test)
add_test(test_regressor_utils regressor_utils_test)
add_test(test_subtree_regressor subtree_regressor_test)
add_test(test_inversedynamics_regressor inversedynamics_regressor_test)
add_test(test_inversedynamics_crba inversedynamics_crba_test)
add_test(test_comjacobian comjacobian_test)
#add_test(test_sparse_numerical_subspace sparse_numerical_subspace_test)
add_test(test_inversedynamics_different_loops inversedynamics_different_loops_test)

#dirl related test
add_executable(dirl_dataset_test dirl_dataset_test.cpp)

target_link_libraries(dirl_dataset_test ${orocos_kdl_LIBRARIES} kdl-codyco )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/dirl_dataset_examples/hrp2_100samples.csv ${CMAKE_CURRENT_BINARY_DIR}/hrp2_100samples.csv)
#First argument of the test: dataset example, second argument: number of expected samples
add_test(test_dirl_dataset_hrp2 dirl_dataset_test hrp2_100samples.csv 100) 
