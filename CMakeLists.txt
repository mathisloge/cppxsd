cmake_minimum_required(VERSION 3.21)
project(cppxsd LANGUAGES CXX)
include(CTest)


set(shared_output_dir "${CMAKE_CURRENT_BINARY_DIR}/out")

add_subdirectory(libcppxsd)
add_subdirectory(cppxsd)
if(BUILD_TESTING)
    #add_subdirectory(tests)
endif()
