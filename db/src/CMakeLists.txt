add_library(libdb libdb.cpp)
add_library(db::libdb ALIAS libdb)

set_target_properties(libdb PROPERTIES OUTPUT_NAME db)

target_compile_features(libdb PUBLIC cxx_std_17)

target_include_directories(
        libdb
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        PRIVATE
            ${CMAKE_SOURCE_DIR}/src/include
)

include(GNUInstallDirs)
install(TARGETS libdb
    EXPORT db-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}

)

install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT db-targets
    FILE db-config.cmake
    NAMESPACE db::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/db
)