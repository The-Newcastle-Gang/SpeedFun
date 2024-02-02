//
// Created by c3042750 on 30/01/2024.
//
#include <iostream>
#include "Replicated.h"

#include "Window.h"

#include "Debug.h"

#include "StateMachine.h"
#include "NavigationMesh.h"

#include "TutorialGame.h"

#include "ClientGameStateMachine.h"
using namespace NCL;
using namespace CSC8503;


void RunTutorialGame(Window* w)
{
    w->ShowOSPointer(false);
    w->LockMouseToWindow(true);

    TutorialGame* g = new TutorialGame();
    w->GetTimer()->GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!
    while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE)) {
        float dt = w->GetTimer()->GetTimeDeltaSeconds();
        if (dt > 0.1f) {
            std::cout << "Skipping large time delta" << std::endl;
            continue; //must have hit a breakpoint or something to have a 1 second frame time!
        }
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

        g->UpdateGame(dt);
    }
    Window::DestroyGameWindow();
}

void RunClientGameStateMachine(Window* w)
{
    w->GetTimer()->GetTimeDeltaSeconds(); 
    ClientGameStateMachine* client = new ClientGameStateMachine;

    while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE)) {
        float dt = w->GetTimer()->GetTimeDeltaSeconds();
        if (dt > 0.1f) {
            std::cout << "Skipping large time delta" << std::endl;
            continue;
        }
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
        client->Update(dt);
    }
    Window::DestroyGameWindow();
}


int main() {
    Window*w = Window::CreateGameWindow("CSC8503 Game technology!", 1280, 720);

    if (!w->HasInitialised()) {
        return -1;
    }

    //RunTutorialGame(w);
    RunClientGameStateMachine(w);

}
