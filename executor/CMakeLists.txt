cmake_minimum_required(VERSION 3.15)
project(cpp20_testing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
if(COMPILER_SUPPORTS_MARCH_NATIVE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()
find_package(Threads REQUIRED)

add_executable(executor-mutex executor-mutex.cpp)
target_link_libraries(executor-mutex Threads::Threads ${CONAN_LIBS})

add_executable(executor-atomic executor-atomic.cpp)
target_link_libraries(executor-atomic Threads::Threads ${CONAN_LIBS})