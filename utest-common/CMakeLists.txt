add_executable(utest-common
        progargs_test.cpp
        binaryio_test.cpp)

target_link_libraries(utest-common PRIVATE common GTest::gtest_main Microsoft.GSL::GSL)