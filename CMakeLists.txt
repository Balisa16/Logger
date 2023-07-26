cmake_minimum_required(VERSION 3.0)

project(Logger)

# Set C++ standard version
set(CMAKE_CXX_STANDARD 11)

# Add the executable target and its source files
add_executable(MyExecutable
    src/main.cpp
    src/math/add.cpp
    src/math/subtract.cpp
    src/utils/helper.cpp
)

# Include directories for the target
target_include_directories(MyExecutable PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Optional: Find and link against the Boost library
# If you have installed Boost to a custom location, you may need to set BOOST_ROOT.
# find_package(Boost REQUIRED COMPONENTS system filesystem)
# target_link_libraries(MyExecutable PRIVATE Boost::system Boost::filesystem)
