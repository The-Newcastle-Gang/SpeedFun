cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(CSC8503 CXX)

set(CMAKE_CXX_STANDARD 23)
set(ASSET_ROOT "${CMAKE_SOURCE_DIR}/Assets/" CACHE STRING "" FORCE)
add_compile_definitions(ASSETROOTLOCATION="${ASSET_ROOT}")

if(MSVC)
    add_compile_definitions("NOMINMAX")
    add_compile_options(/wd4293 /wd4018 /wd4244)
endif()

include_directories(enet)
include_directories(entt)

add_subdirectory(raudio)
add_subdirectory(enet)
add_subdirectory(NCLCoreClasses)
add_subdirectory(CSC8503CoreClasses)
add_subdirectory(OpenGLRendering)
add_subdirectory(NotLua)
add_subdirectory(FreetypeRenderer)
add_subdirectory(Replicated)
add_subdirectory(Client)
add_subdirectory(External)
add_subdirectory(Server)