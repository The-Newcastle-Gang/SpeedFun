cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
project(client)

add_executable(client
        main.cpp
        Client.cpp
        Renderer.cpp
        TutorialGame.cpp
        ClientGameStateMachine.cpp
        States/GameStateInactive.cpp
        States/GameStateInGameplay.cpp
        States/GameStateLoading.cpp
        States/GameStateMenu.cpp
)

target_include_directories(client PUBLIC
        ../NCLCoreClasses
        ../CSC8503CoreClasses
        ../OpenGLRendering
        ./
        ../Replicated
        ../FreetypeRenderer/Include
        ./States
)

target_link_libraries(client PUBLIC
        NCLCoreClasses
        CSC8503CoreClasses
        OpenGLRendering
        replicated
        freetype
)

target_precompile_headers(${PROJECT_NAME} PRIVATE
        <vector>
        <map>
        <stack>
        <list>
        <set>
        <string>
        <thread>
        <atomic>
        <functional>
        <iostream>
        <chrono>
        <sstream>

        "../NCLCoreClasses/Vector2.h"
        "../NCLCoreClasses/Vector3.h"
        "../NCLCoreClasses/Vector4.h"
        "../NCLCoreClasses/Quaternion.h"
        "../NCLCoreClasses/Plane.h"
        "../NCLCoreClasses/Matrix2.h"
        "../NCLCoreClasses/Matrix3.h"
        "../NCLCoreClasses/Matrix4.h"

        "../NCLCoreClasses/GameTimer.h"
)
