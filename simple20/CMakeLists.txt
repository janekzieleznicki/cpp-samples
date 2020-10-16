project(simple20)

# Will compile only under clang due to limisted corutines support

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
if(COMPILER_SUPPORTS_MARCH_NATIVE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()
find_package(Threads)
add_executable(${PROJECT_NAME} Main.cpp)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
# target_link_libraries(${PROJECT_NAME} Threads::Threads ${CONAN_LIBS})
target_compile_options(${PROJECT_NAME} PUBLIC
-Wall
-Wextra
)
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_link_options(${PROJECT_NAME} PUBLIC -lc++)
    target_compile_options(${PROJECT_NAME} PUBLIC
    -stdlib=libc++
    -fcoroutines-ts
    )
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    # set(CMAKE_EXE_LINKER_FLAGS "-lc++abi")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${PROJECT_NAME} PUBLIC
    -fcoroutines
    )
endif()