add_executable(Client client_main.cpp ssl.cpp openConnection.cpp handleMessages.cpp readMessages.cpp)

# https://hackingcpp.com/cpp/libs/fmt.html
find_package(fmt)

target_link_libraries(Client)
target_link_libraries(Client fmt::fmt)
target_link_libraries(Client Logging)

target_include_directories(Client PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )
target_include_directories(Client PUBLIC "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/Logging")

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