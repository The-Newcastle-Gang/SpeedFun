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
#include "entt.hpp"

using namespace NCL;
using namespace CSC8503;

struct PlayerValuesComponent {
    int hp;
    float speed;

    PlayerValuesComponent() {
        hp = 100;
        speed = 1.5f;
    }

    PlayerValuesComponent(int c, float a) {
        hp = c;
        speed = a;
    }
};

void OperateOnPlayerComponent(PlayerValuesComponent& epic) {
    epic.hp += 1;
    epic.hp += sqrt(2.0f / 3.0f);
}


void TestECS() {
    entt::registry registry;
    std::cout << "START!" << "\n";
    entt::entity player1 = registry.create();
    entt::entity player2 = registry.create();
    entt::entity player3 = registry.create();
    entt::entity player4 = registry.create();
    entt::entity player5 = registry.create();
    entt::entity player6 = registry.create();
    entt::entity player7 = registry.create();

    registry.emplace<PlayerValuesComponent>(player1,300,4.5f);
    registry.emplace<PlayerValuesComponent>(player4);
    registry.emplace<PlayerValuesComponent>(player5);


    auto epicView = registry.view<PlayerValuesComponent>();

    for (int i = 0; i < 1000000; i++) {
        for (auto entity : epicView) {
            auto& epicComponent = epicView.get<PlayerValuesComponent>(entity);
            OperateOnPlayerComponent(epicComponent);
        }
    }
    std::cout << "END!" << "\n";

}


int main() {


    TestECS();
    Window*w = Window::CreateGameWindow("CSC8503 Game technology!", 1280, 720);

    if (!w->HasInitialised()) {
        return -1;
    }

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


