cmake_minimum_required(VERSION 3.17)
project(partition-count)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
                 "${CMAKE_BINARY_DIR}/conan.cmake" 
                 TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_run(CONAN_COMMAND "/home/jzieleznicki/.local/bin/conan"
                CONANFILE conanfile.py  # or relative build/conanfile.txt
                BASIC_SETUP CMAKE_TARGETS
                BUILD missing)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
if(COMPILER_SUPPORTS_MARCH_NATIVE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()

add_library(${PROJECT_NAME}-lib INTERFACE)

target_include_directories(${PROJECT_NAME}-lib INTERFACE sources)

find_package(Threads)



target_compile_features(${PROJECT_NAME}-lib INTERFACE cxx_std_20)
target_link_libraries(${PROJECT_NAME}-lib INTERFACE Threads::Threads ${CONAN_LIBS})
target_compile_options(${PROJECT_NAME}-lib INTERFACE
-std=c++20
-Wall
-Wextra
-fcoroutines
# -fsanitize=address
# -fsanitize=undefined
# -fsanitize=leak
)


add_executable(${PROJECT_NAME} Main.cpp)
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}-lib) 

add_executable(${PROJECT_NAME}-test test/PartitionCountTest.cpp)
target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME}-lib) 

include(CTest)
include(Catch)
catch_discover_tests(${PROJECT_NAME}-test)
