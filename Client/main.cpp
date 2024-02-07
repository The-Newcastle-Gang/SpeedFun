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
    Window *w = Window::CreateGameWindow("CSC8508 SpeedFun!", 1280, 720);

    if (!w->HasInitialised()) {
        return -1;
    }

    // Clear timer so there's no large dt. Get time delta doesn't work.
    w->UpdateWindow();

    auto client = new Client();
    auto g = new TutorialGame();
  
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

        if(Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)){
            g->UpdateGame(dt);
        }
        if(Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)){
            client->Update(dt);
        }
       
      w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));
    }

    Window::DestroyGameWindow();
}