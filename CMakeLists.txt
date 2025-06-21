cmake_minimum_required(VERSION 3.15)
project(QuantumConsciousnessNetwork)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Eigen library
find_package(Eigen3 3.3 REQUIRED CONFIG)

# Handle ASIO (header-only)
find_path(ASIO_INCLUDE_DIR asio.hpp
    PATHS
    /usr/local/include
    /usr/include
    ${CMAKE_SOURCE_DIR}/third_party/asio/include
)

if(NOT ASIO_INCLUDE_DIR)
    message(STATUS "ASIO not found, downloading...")
    include(FetchContent)
    FetchContent_Declare(
        asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
        GIT_TAG asio-1-28-0
    )
    FetchContent_MakeAvailable(asio)
    set(ASIO_INCLUDE_DIR ${asio_SOURCE_DIR}/asio/include)
endif()

include_directories(
    include 
    ${EIGEN3_INCLUDE_DIRS}
    ${ASIO_INCLUDE_DIR}
)

file(GLOB SOURCES "src/*.cpp" "src/*.cxx" "src/*.cc")

add_executable(qcn ${SOURCES})

# Enable colored output
if(UNIX)
    target_compile_options(qcn PRIVATE -DCOLOR_OUTPUT)
endif()