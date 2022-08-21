cmake_minimum_required(VERSION 2.8)

add_subdirectory(Private)

add_library(Logging STATIC logging.cpp)

include_directories(Logging PRIVATE 
"${PROJECT_BINARY_DIR}"
"${PROJECT_SOURCE_DIR}/Private")

target_link_libraries(Logging Private)

install(TARGETS Logging DESTINATION lib)
install(FILES logging.hpp DESTINATION include)