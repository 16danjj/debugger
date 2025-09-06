add_executable(db db.cpp)
target_link_libraries(db PRIVATE db::libdb PkgConfig::libedit)

include(GNUInstallDirs)
install(
    TARGETS db 
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)