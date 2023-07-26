cmake_minimum_required(VERSION 3.0)

project(Logger)

set(CMAKE_CXX_STANDARD 11)

find_package(Boost REQUIRED COMPONENTS system filesystem)

# Add the executable target and its source files
add_executable(MyExecutable
    src/test.cpp
)

# Include directories for the target
target_include_directories(MyExecutable PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(MyExecutable PRIVATE Boost::system Boost::filesystem)

# Optional: Find and link against the Boost library
# If you have installed Boost to a custom location, you may need to set BOOST_ROOT.
