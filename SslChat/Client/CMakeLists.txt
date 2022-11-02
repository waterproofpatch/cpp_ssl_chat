add_executable(Client client_main.cpp openConnection.cpp run.cpp)

target_link_libraries(Client Ssl fmt::fmt Logging pthread Cli)
include_directories(Client PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           "${PROJECT_SOURCE_DIR}/Ssl"
                           "${PROJECT_SOURCE_DIR}/Logging"
                           "${PROJECT_SOURCE_DIR}/Cli"
                           )

# needs to be ... not at the top
find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
  # Add the include directories for compiling
  # message("OpenSSL include dir: ${OPENSSL_INCLUDE_DIR} for project ${PROJECT_NAME}")
  target_include_directories(Client PUBLIC ${OPENSSL_INCLUDE_DIR})

  # Add the static lib for linking
  target_link_libraries(Client OpenSSL::SSL OpenSSL::Crypto)

  message(STATUS "Found OpenSSL ${OPENSSL_VERSION}")

else()

  message(STATUS "OpenSSL Not Found")

endif()