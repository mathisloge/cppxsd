add_subdirectory(parser)
add_subdirectory(output)

target_sources(libcppxsd PRIVATE 
    cppxsd.cpp
    resolve_qname.cpp
)
