add_library(imgaos
        imageaos.cpp
        imageaos.hpp
        ../utest-imgaos/one_test.cpp
)
target_link_libraries(imgaos PUBLIC common PRIVATE Microsoft.GSL::GSL)