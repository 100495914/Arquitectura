# Add to this list all files related to common library
add_library(common
        progargs.cpp
        binaryio.cpp
        binaryio.hpp
        progargs.hpp
        PPMMetadata.hpp
        getPPMMetadata.cpp
        imtool_soa_aux.cpp
        imtool_soa_aux.hpp
        misc.cpp
        misc.hpp
        ../ftest-soa/ftest.cpp
)
# Use this line only if you have dependencies from this library to GSL
target_link_libraries(common PRIVATE Microsoft.GSL::GSL)
