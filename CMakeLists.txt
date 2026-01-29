cmake_minimum_required(VERSION 4.0)
project(ALDCCsim)

set(CMAKE_CXX_STANDARD 20)

add_executable(ALDCCsim
        src/main.cpp
        src/solver.cpp
        src/util.cpp
        src/file_io.cpp
        src/bisect.cpp
        src/cli.cpp

        include/element.hpp
        include/util.hpp
        include/solver.hpp
        include/file_io.hpp
        include/bisect.hpp
        include/constants.hpp
        include/context.hpp
        include/cli.hpp
)

target_include_directories(ALDCCsim PRIVATE include)
