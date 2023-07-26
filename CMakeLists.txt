cmake_minimum_required(VERSION 3.0)

project(Logger)

set(CMAKE_CXX_STANDARD 11)

find_package(Boost REQUIRED COMPONENTS system filesystem)

# Add the executable target and its source files
add_executable(test1
    src/test1.cpp
)

add_executable(test2
    src/test2.cpp
)

# Include directories for the target
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(test1 PRIVATE Boost::system Boost::filesystem)
target_link_libraries(test2 PRIVATE Boost::system Boost::filesystem)

# Optional: Find and link against the Boost library
# If you have installed Boost to a custom location, you may need to set BOOST_ROOT.
