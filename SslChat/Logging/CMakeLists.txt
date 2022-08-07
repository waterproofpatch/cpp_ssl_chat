cmake_minimum_required(VERSION 2.8)

project(libtest_project)

add_library(logging STATIC logging.cpp)

install(TARGETS logging DESTINATION lib)
install(FILES logging.hpp DESTINATION include)