cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
project(server)

add_executable(server
Server.cpp
main.cpp
"States/GameStateBusy.cpp"
"States/GameStateRunning.cpp"
"States/GameStateWaitingPlayers.cpp"
"ServerMachine.cpp"
)

target_include_directories(server PUBLIC
../NCLCoreClasses
../CSC8503CoreClasses
../OpenGLRendering
../Replicated
./
./States)
target_link_libraries(server PUBLIC
NCLCoreClasses
CSC8503CoreClasses
OpenGLRendering
replicated
enet
)
