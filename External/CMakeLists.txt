cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
project(external)

add_library(external STATIC "entt.hpp" "tinyobjloader.h" )
target_include_directories(${PROJECT_NAME} PUBLIC ../NCLCoreClasses ../CSC8503CoreClasses ../OpenGLRendering ./)
target_link_libraries(${PROJECT_NAME} PUBLIC NCLCoreClasses CSC8503CoreClasses OpenGLRendering)