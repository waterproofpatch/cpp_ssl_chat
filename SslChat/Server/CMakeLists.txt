add_executable(Server server_main.cpp ssl.cpp client.cpp initServer.cpp run.cpp handleNewConnection.cpp resetFd.cpp)

# https://hackingcpp.com/cpp/libs/fmt.html
find_package(fmt)

target_link_libraries(Server fmt::fmt Logging pthread Cli)
include_directories(Server PUBLIC 
                    "${PROJECT_BINARY_DIR}"
                    "${PROJECT_SOURCE_DIR}/Logging"
                    "${PROJECT_SOURCE_DIR}/Cli")

# needs to be ... not at the top
find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
  # Add the include directories for compiling
  # message("OpenSSL include dir: ${OPENSSL_INCLUDE_DIR} for project ${PROJECT_NAME}")
  target_include_directories(Server PUBLIC ${OPENSSL_INCLUDE_DIR})

  # Add the static lib for linking
  target_link_libraries(Server OpenSSL::SSL OpenSSL::Crypto)

  message(STATUS "Found OpenSSL ${OPENSSL_VERSION}")

else()

  message(STATUS "OpenSSL Not Found")

endif()