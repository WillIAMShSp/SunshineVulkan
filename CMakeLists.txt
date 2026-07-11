cmake_minimum_required(VERSION 3.25)

project(SunshineVulkan)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

find_package(Vulkan REQUIRED)

add_subdirectory(external/PersistanceVulkan)

add_executable(SunshineVulkan src/main.cpp)

target_link_libraries(SunshineVulkan PRIVATE PersistanceLibrary)

target_include_directories(SunshineVulkan PRIVATE external/PersistanceVulkan/Include)
target_include_directories(SunshineVulkan PRIVATE external/PersistanceVulkan/PersistanceVulkan/src)

target_compile_definitions(SunshineVulkan PRIVATE
RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/res"
)
