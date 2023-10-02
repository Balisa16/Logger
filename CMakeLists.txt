cmake_minimum_required(VERSION 3.0)

project(Logger)

set(CMAKE_CXX_STANDARD 11)

find_package(Boost REQUIRED COMPONENTS system filesystem)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${Boost_INCLUDE_DIRS}
)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
add_library(${PROJECT_NAME} SHARED
    src/Logger.cc
    include/Logger.hpp)

add_executable(test1 test/test.cc)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/test)

target_link_libraries(test1 PRIVATE 
    ${PROJECT_NAME}
    Boost::system 
    Boost::filesystem)