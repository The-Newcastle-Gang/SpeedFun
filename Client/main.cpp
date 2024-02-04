//
// Created by c3042750 on 30/01/2024.
//
#include <iostream>
#include <chrono>
#include <thread>
#include <sstream>

#include "Replicated.h"
#include "Client.h"
#include "Window.h"
#include "Debug.h"
#include "StateMachine.h"
#include "NavigationMesh.h"
#include "TutorialGame.h"
#include "PushdownMachine.h"
#include "PushdownState.h"
#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourAction.h"
#include "LevelReader.h"
#include "ClientMachine.h"

using namespace NCL;
using namespace CSC8503;

void RunClientGameStateMachine(Window* w) {
    Window::GetTimer()->GetTimeDeltaSeconds();
    auto client = new Client();
    client->InitClient();

    Window::GetWindow()->LockMouseToWindow(true);

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

        client->Update(dt);
    }
    Window::DestroyGameWindow();
}


int main() {
    Window*w = Window::CreateGameWindow("CSC8503 Game technology!", 1280, 720);

    if (!w->HasInitialised()) {
        return -1;
    }

    RunClientGameStateMachine(w);

}
