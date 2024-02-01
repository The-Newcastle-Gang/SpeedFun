cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(CSC8503 CXX)

set(CMAKE_CXX_STANDARD 23)
set(ASSET_ROOT "${CMAKE_SOURCE_DIR}/Assets/" CACHE STRING "" FORCE)
add_compile_definitions(ASSETROOTLOCATION="${ASSET_ROOT}")


add_subdirectory(NCLCoreClasses)
add_subdirectory(CSC8503CoreClasses)
add_subdirectory(OpenGLRendering) 
add_subdirectory(Replicated)
add_subdirectory(Client)
add_subdirectory(Server) 
