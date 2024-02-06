cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
project(client)

add_executable(client 
main.cpp 
Client.cpp 
Renderer.cpp 
TutorialGame.cpp 
Font.cpp 
"States/GameStateInGameplay.cpp" 
"States/GameStateInactive.cpp" 
"States/GameStateLoading.cpp" 
"States/GameStateMenu.cpp" 
"ClientGameStateMachine.cpp")

target_include_directories(client PUBLIC ../NCLCoreClasses ../CSC8503CoreClasses ../OpenGLRendering ./ ../Replicated ../External ../FreetypeRenderer/include ./States)
target_link_libraries(client PUBLIC NCLCoreClasses CSC8503CoreClasses OpenGLRendering replicated freetype)