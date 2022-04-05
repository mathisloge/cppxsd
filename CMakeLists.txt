cmake_minimum_required(VERSION 3.21)
project(cppxsd LANGUAGES CXX)
include(CTest)

if(BUILD_TESTING)
    # remove if catch2 will release v3 and vcpkg updates
    Include(FetchContent)

    FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.0.0-preview4
    )

    FetchContent_MakeAvailable(Catch2)
    list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
    include(Catch)
endif()

set(shared_output_dir "${CMAKE_CURRENT_BINARY_DIR}/out")

add_subdirectory(libcppxsd)
add_subdirectory(cppxsd)
