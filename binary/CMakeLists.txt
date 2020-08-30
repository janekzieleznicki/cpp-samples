cmake_minimum_required(VERSION 3.15)
project(binary)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
if(COMPILER_SUPPORTS_MARCH_NATIVE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()
find_package(Threads REQUIRED)

file(GLOB_RECURSE HEADER_FILES
       ${PROJECT_SOURCE_DIR}/*.hpp)

file(GLOB_RECURSE SOURCE_FILES
       ${PROJECT_SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} Threads::Threads ${CONAN_LIBS} HelloProvider)