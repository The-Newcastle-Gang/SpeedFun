cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
project(server)

add_library(server
        Server.cpp
        GameServer.cpp
        States/RunningState.cpp
        States/WaitingState.cpp
        ServerThread.cpp
        Components/PlayerMovement.cpp

 "Components/Spring.h" "Components/Spring.cpp")

target_include_directories(server PUBLIC
        ../NCLCoreClasses
        ../CSC8503CoreClasses
        ../OpenGLRendering
        ../Replicated
        ./
        ./States
        ../enet/include
        ./Components
)

target_link_libraries(server PUBLIC
        NCLCoreClasses
        CSC8503CoreClasses
        OpenGLRendering
        replicated
        enet
)
