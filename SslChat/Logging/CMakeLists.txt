cmake_minimum_required(VERSION 2.8)

project(SslChat)

add_subdirectory(Private)

add_library(Logging STATIC logging.cpp)

target_include_directories(Logging PRIVATE "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/Private")
target_link_libraries(Logging LoggingPrivate)

install(TARGETS Logging DESTINATION lib)
install(FILES logging.hpp DESTINATION include)