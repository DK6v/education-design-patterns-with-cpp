cmake_minimum_required(VERSION 3.25)

add_definitions(-std=c++20)
add_compile_options(-Wall -Wextra -pedantic -O2 -std=c++20)

project(example CXX)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Common files
file(GLOB SOURCES "lib/common/*.cpp")
add_library(common STATIC ${SOURCES})

# Google test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.15.2
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
set(BUILD_GTEST ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Google benchmark
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG        v1.9.1
)
FetchContent_MakeAvailable(benchmark)
FetchContent_GetProperties(benchmark SOURCE_DIR BENCHMARK_INCLUDE_DIR)

# Prepare targets
function(ts_target_stop retvar)
    file(GLOB SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "*.cpp")
    foreach(file IN ITEMS ${SOURCES})
        string(REGEX MATCH "^[0-9A-Z]+" match ${file})
        if(NOT ${match} STREQUAL "")
            list(APPEND ${retvar} ${match})
        endif()
    endforeach()
    list(SORT ${retvar})
    list(REMOVE_DUPLICATES ${retvar})
    return(PROPAGATE ${retvar})
endfunction()

function(ts_add_executable target)
    file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "./${target}_*.cpp")
    if(SOURCES)
        list(JOIN SOURCES " " SOURCES_STR)
        message("Target ${target}: " ${SOURCES_STR})

        add_executable(${target} EXCLUDE_FROM_ALL ${SOURCES})
        set_target_properties(${target} PROPERTIES OUTPUT_NAME "run")

        # Set the paths to the header files
        target_include_directories(${target} PRIVATE "include" "lib/common")

        # Compile source files compile with -S, 
        # that will output a something.s file with the assembly code.
        # Add option -masm=intel to make output more readable.
        # set_source_files_properties(${SOURCES} PROPERTIES COMPILE_FLAGS "-S -masm=intel")

        # Link libraries
        target_link_libraries(${target}
            common
            gtest gtest_main    # Google test and mocks
            benchmark           # Google benchmark
            rt                  # librt, realtime extensions library
            Threads::Threads    # pthread library
        )

    endif()
endfunction()

project(Examples)

ts_target_stop(targets)
# message("Targets: ${targets}")
foreach(target IN ITEMS ${targets})
    ts_add_executable("${target}")
endforeach()
