add_library(imgsoa
        imagesoa.cpp
        imagesoa.hpp
        ../utest-imgsoa/utest-imgsoa.cpp
)
target_link_libraries(imgsoa PUBLIC common PRIVATE Microsoft.GSL::GSL)