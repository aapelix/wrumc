add_library(wrumc-shared STATIC
    net/msg.cpp
)

target_include_directories(wrumc-shared PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(wrumc-shared PUBLIC msgpack)
