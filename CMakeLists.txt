cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(Server VERSION 1.0)
project(Client VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_executable(Server server_main.cpp)
add_executable(Client client_main.cpp)

target_include_directories(Server PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )
target_include_directories(Client PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )


# needs to be ... not at the top
find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
  # Add the include directories for compiling
  message("OpenSSL include dir: ${OPENSSL_INCLUDE_DIR} for project ${PROJECT_NAME}")
  target_include_directories(${PROJECT_NAME} PUBLIC ${OPENSSL_INCLUDE_DIR})

  # Add the static lib for linking
  target_link_libraries(${PROJECT_NAME} OpenSSL::SSL OpenSSL::Crypto)

  message(STATUS "Found OpenSSL ${OPENSSL_VERSION}")

else()

  message(STATUS "OpenSSL Not Found")

endif()