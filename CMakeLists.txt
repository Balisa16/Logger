cmake_minimum_required(VERSION 3.10)

project(Logger)

set(CMAKE_CXX_STANDARD 11)

find_package(Boost REQUIRED COMPONENTS system filesystem)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${Boost_INCLUDE_DIRS}
)

# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
add_library(${PROJECT_NAME} SHARED
    src/Logger.cc
    include/Logger.hpp)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION "/usr/lib"
    ARCHIVE DESTINATION "/usr/lib"
    RUNTIME DESTINATION "/usr/bin/logger"
    INCLUDES DESTINATION "/usr/include/logger"
)

install(FILES include/Logger.hpp DESTINATION include)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/test)
add_executable(test1 test/test.cpp)
target_link_libraries(test1 PRIVATE 
    ${PROJECT_NAME}
    pthread
    Boost::system 
    Boost::filesystem)
