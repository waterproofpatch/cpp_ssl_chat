# https://hackingcpp.com/cpp/libs/fmt.html
find_package(fmt)

add_library(Ssl STATIC ssl.cpp)
target_link_libraries(Ssl fmt::fmt Logging)
include_directories(Ssl PUBLIC 
                    "${PROJECT_BINARY_DIR}"
                    "${PROJECT_SOURCE_DIR}/Logging")

# needs to be ... not at the top
find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
  # Add the include directories for compiling
  # message("OpenSSL include dir: ${OPENSSL_INCLUDE_DIR} for project ${PROJECT_NAME}")
  target_include_directories(Ssl PUBLIC ${OPENSSL_INCLUDE_DIR})

  # Add the static lib for linking
  target_link_libraries(Ssl OpenSSL::SSL OpenSSL::Crypto)

  message(STATUS "Found OpenSSL ${OPENSSL_VERSION}")

else()

  message(STATUS "OpenSSL Not Found")

endif()