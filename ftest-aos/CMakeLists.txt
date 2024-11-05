# ftest-aos/CMakeLists.txt
add_executable(test_scaling test_scaling.cpp)
target_link_libraries(test_scaling PRIVATE imgaos)
