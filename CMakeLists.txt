cmake_minimum_required(VERSION 3.6)

# Fail immediately if not using an out-of-source build
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
  message(FATAL_ERROR
    "In-source builds are not supported.  Please create a build directory "
    "separate from the source directory")
endif()

project(adios-compression-test)

find_package(ADIOS2 REQUIRED)
find_package(MPI REQUIRED)

add_executable(adios-rewrite adios-rewrite.cpp)
target_link_libraries(adios-rewrite adios2::adios2 MPI::MPI_CXX)
