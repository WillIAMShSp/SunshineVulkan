cmake_minimum_required(VERSION 3.25)

project(SunshineVulkan)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

find_package(Vulkan REQUIRED)

add_executable(SunshineVulkan src/main.cpp)

