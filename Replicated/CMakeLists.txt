cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
project(replicated)


add_library(replicated STATIC Replicated.cpp
        "LevelReader.cpp"
        "LevelBuilder.cpp"
        "Component.h"
        "TestComponent.h"
        "PlayerPhysComponent.cpp"
        "GrappleComponent.cpp"
        "InputListener.cpp"
        "SafeQueue.cpp")

target_include_directories(${PROJECT_NAME} PUBLIC ../NCLCoreClasses ../CSC8503CoreClasses ../OpenGLRendering ./)
target_link_libraries(${PROJECT_NAME} PUBLIC NCLCoreClasses CSC8503CoreClasses OpenGLRendering)