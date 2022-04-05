add_executable(libcppxsd-tests 
    src/schema.cpp
)
catch_discover_tests(libcppxsd-tests)
target_link_libraries(libcppxsd-tests 
    PRIVATE 
        cppxsd::cppxsd
        Catch2::Catch2WithMain
)

set_target_properties(libcppxsd-tests PROPERTIES 
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    RUNTIME_OUTPUT_DIRECTORY "${shared_output_dir}"
)
