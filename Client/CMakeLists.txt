cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
project(client)

add_executable(client main.cpp Client.cpp Renderer.cpp TutorialGame.cpp)
target_include_directories(client PUBLIC ../NCLCoreClasses ../CSC8503CoreClasses ../OpenGLRendering ./ ../Replicated)
target_link_libraries(client PUBLIC NCLCoreClasses CSC8503CoreClasses OpenGLRendering replicated)