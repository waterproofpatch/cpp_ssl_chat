cmake_minimum_required(VERSION 2.8)

# project(SslChat)

find_package(fmt)

add_library(Cli STATIC cliLoop.cpp)
target_link_libraries(Cli fmt::fmt Logging)

include_directories(Cli PUBLIC 
"${PROJECT_BINARY_DIR}" 
"${PROJECT_SOURCE_DIR}"
"${PROJECT_SOURCE_DIR}/Logging")

install(TARGETS Cli DESTINATION lib)
install(FILES cliLoop.hpp DESTINATION include)