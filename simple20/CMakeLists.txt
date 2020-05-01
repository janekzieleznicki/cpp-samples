project(simple20)

# Will compile only under clang due to limisted corutines support

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
if(COMPILER_SUPPORTS_MARCH_NATIVE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()

add_executable(${PROJECT_NAME} Main.cpp)
# target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})
# target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_compile_options(${PROJECT_NAME} PUBLIC
-std=c++2a
-fcoroutines
-Wall
-Wextra
)
# set( CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -stdlib=libc++")