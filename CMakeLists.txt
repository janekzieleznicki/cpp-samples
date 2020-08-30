cmake_minimum_required(VERSION 3.17)
project(cpp-samples)


include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_subdirectory(executor)
add_subdirectory(simple20)
add_subdirectory(emplace-vs-push)
add_subdirectory(why-it-works)
