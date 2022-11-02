cmake_minimum_required(VERSION 3.10)
# specify the C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 20)

project(SslChat VERSION 1.0)

# https://hackingcpp.com/cpp/libs/fmt.html
add_subdirectory(fmt)

add_subdirectory(Logging)
add_subdirectory(Client)
add_subdirectory(Server)
add_subdirectory(Cli)
add_subdirectory(Ssl)