add_executable(db db.cpp)
target_link_libraries(db PRIVATE db::libdb)