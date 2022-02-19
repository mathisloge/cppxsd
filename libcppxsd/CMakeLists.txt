include(GenerateExportHeader)

find_package(pugixml CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

add_library(libcppxsd "")
add_library(cppxsd::cppxsd ALIAS libcppxsd)
generate_export_header(libcppxsd)
set_target_properties(libcppxsd PROPERTIES 
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    RUNTIME_OUTPUT_DIRECTORY "${shared_output_dir}"
)
target_link_libraries(libcppxsd PRIVATE 
    pugixml::pugixml
    CURL::libcurl
    fmt::fmt
)
target_include_directories(libcppxsd PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:include>
)
add_subdirectory(src)
