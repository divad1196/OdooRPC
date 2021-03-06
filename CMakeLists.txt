cmake_minimum_required(VERSION 3.15)
project(OdooRPC)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion -Wall -Wextra -pedantic -std=c++11")


file(MAKE_DIRECTORY builds)
# https://cmake.org/cmake/help/latest/variable/CMAKE_SOURCE_DIR.html#variable:CMAKE_SOURCE_DIR
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

FIND_PACKAGE(CURL)
IF(CURL_FOUND)
  INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIR})
  set(CURL_LIBRARY "-lcurl") 
ELSE(CURL_FOUND)
  MESSAGE(FATAL_ERROR "Could not find the CURL library and development files.")
ENDIF(CURL_FOUND)

ADD_LIBRARY(odoorpc
  src/credentials.h src/credentials.cpp
  src/odoorpc.h src/odoorpc.cpp
)
target_link_libraries(odoorpc ${CURL_LIBRARY})


add_executable(
    client
    main.cpp
)
target_link_libraries(client odoorpc)

add_executable(
    jsonrpc
    main2.cpp
)
target_link_libraries(jsonrpc odoorpc)