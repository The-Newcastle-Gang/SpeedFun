cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
project(replicated)

add_library(replicated STATIC Replicated.cpp "ServerGameStateMachine.h" "../Client/ClientGameStateMachine.h" "../Client/ClientGameStateMachine.cpp" "ServerGameStateMachine.cpp")
target_include_directories(${PROJECT_NAME} PUBLIC ../NCLCoreClasses ../CSC8503CoreClasses ../OpenGLRendering ./)
target_link_libraries(${PROJECT_NAME} PUBLIC NCLCoreClasses CSC8503CoreClasses OpenGLRendering)