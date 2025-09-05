add_library(libdb libdb.cpp)
add_library(db::libdb ALIAS libdb)

set_target_properties(libdb PROPERTIES OUTPUT_NAME db)

target_compile_features(libdb PUBLIC cxx_std_17)

target_include_directories(
        libdb
        PRIVATE include
        PUBLIC ${CMAKE_SOURCE_DIR}/include
)