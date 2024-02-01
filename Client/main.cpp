//
// Created by c3042750 on 30/01/2024.
//
#include <iostream>
#include "Replicated.h"

#include "Window.h"

#include "Debug.h"

#include "StateMachine.h"
#include "StateTransition.h"
#include "State.h"

#include "GameServer.h"
#include "GameClient.h"

#include "NavigationGrid.h"
#include "NavigationMesh.h"

#include "TutorialGame.h"

#include "PushdownMachine.h"

#include "PushdownState.h"

#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourAction.h"

#include "ClientGameStateMachine.h"
#include "ServerGameStateMachine.h"
using namespace NCL;
using namespace CSC8503;

#include <chrono>
#include <thread>
#include <sstream>

void TestPathfinding() {
}

void DisplayPathfinding() {
}

/*

The main function should look pretty familar to you!
We make a window, and then go into a while loop that repeatedly
runs our 'game' until we press escape. Instead of making a 'renderer'
and updating it, we instead make a whole game, and repeatedly update that,
instead.

This time, we've added some extra functionality to the window class - we can
hide or show the

*/
int main() {



    Window*w = Window::CreateGameWindow("CSC8503 Game technology!", 1280, 720);

    if (!w->HasInitialised()) {
        return -1;
    }

    w->ShowOSPointer(false);
    w->LockMouseToWindow(true);

    TutorialGame* g = new TutorialGame();
    w->GetTimer()->GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!

    //ClientGameStateMachine* client = new ClientGameStateMachine;
    while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE)) {
        float dt = w->GetTimer()->GetTimeDeltaSeconds();
        if (dt > 0.1f) {
            std::cout << "Skipping large time delta" << std::endl;
            continue; //must have hit a breakpoint or something to have a 1 second frame time!
        }
        //*
        if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR)) {
            w->ShowConsole(true);
        }
        if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT)) {
            w->ShowConsole(false);
        }

        if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::T)) {
            w->SetWindowPosition(0, 0);
        }

        w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));
        //*/
        /*
        if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM1))
        {
            client->SetClientState(ClientStates::LoadingState);
        }
        if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM2))
        {
            client->SetClientState(ClientStates::MenuState);
        }
        client->Update(dt);
        */
        g->UpdateGame(dt);
      
    }
    Window::DestroyGameWindow();
}