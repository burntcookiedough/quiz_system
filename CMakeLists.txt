cmake_minimum_required(VERSION 3.10)
project(QuizSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directories (so we can use #include <json.hpp>)
include_directories(include)

# Find all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Depending on platform, compile cpp-httplib properly (needs ws2_32 on Windows)
add_executable(QuizSystem ${SOURCES})

if(WIN32)
    target_link_libraries(QuizSystem ws2_32)

    # When compiling with MinGW, statically link the C++ runtime so the .exe
    # does not depend on libgcc_s_seh-1.dll, libstdc++-6.dll, or libwinpthread-1.dll.
    # This makes the .exe fully standalone and runnable on any Windows PC.
    if(MINGW)
        target_link_options(QuizSystem PRIVATE -static -static-libgcc -static-libstdc++)
    endif()
endif()
