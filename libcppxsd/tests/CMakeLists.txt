add_executable(libcppxsd-tests 
    src/schema.cpp
    src/simpleType.cpp
    src/restriction.cpp
    src/list.cpp
    src/union.cpp
)
target_link_libraries(libcppxsd-tests 
    PRIVATE 
        fmt::fmt
        cppxsd::cppxsd
        Catch2::Catch2WithMain
)
set_target_properties(libcppxsd-tests PROPERTIES 
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    RUNTIME_OUTPUT_DIRECTORY "${shared_output_dir}"
)
catch_discover_tests(libcppxsd-tests 
    WORKING_DIRECTORY "${shared_output_dir}"
)

file(COPY schemas DESTINATION "${shared_output_dir}")
