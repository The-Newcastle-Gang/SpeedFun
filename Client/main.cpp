//
// Created by c3042750 on 30/01/2024.
//
#include <iostream>

#include "Client.h"
#include "Window.h"
#include "TutorialGame.h"
#include "PushdownMachine.h"
#include "LevelReader.h"

using namespace NCL;
using namespace CSC8503;


int main() {
    Window*w = Window::CreateGameWindow("CSC8503 Game technology!", 1280, 720);

    if (!w->HasInitialised()) {
        return -1;
    }
    // Clear timer so there's no large dt. Get time delta doesn't work.
    w->UpdateWindow();

    auto client = new Client();

    Window::GetWindow()->ShowOSPointer(true);
    Window::GetWindow()->LockMouseToWindow(false);

    while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE)) {
        float dt = w->GetTimer()->GetTimeDeltaSeconds();
        if (dt > 0.1f) {
            std::cout << "Skipping large time delta" << std::endl;
            continue;
        }

        client->Update(dt);
    }
    Window::DestroyGameWindow();

}
