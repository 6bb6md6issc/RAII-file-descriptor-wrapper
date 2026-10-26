cmake_minimum_required(VERSION 3.23)
project(RaiiFileDescriptorWrapper LANGUAGES CXX)

add_library(UniqueFd STATIC)

target_sources(UniqueFd
    PRIVATE
        src/unique_fd.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/sys/unique_fd.hpp
)

target_compile_features(UniqueFd PUBLIC cxx_std_20)

add_executable(unique_fd_demo examples/main.cpp)

target_link_libraries(unique_fd_demo
    PRIVATE
        UniqueFd
)