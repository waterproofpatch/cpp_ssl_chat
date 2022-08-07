cmake_minimum_required(VERSION 2.8)

project(libtest_project)

add_library(Logging STATIC logging.cpp)

install(TARGETS Logging DESTINATION lib)
install(FILES logging.hpp DESTINATION include)