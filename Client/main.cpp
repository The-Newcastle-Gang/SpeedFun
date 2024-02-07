//
// Created by c3042750 on 30/01/2024.
//
#include <iostream>

#include "Client.h"
#include "Window.h"
#include "TutorialGame.h"
#include "PushdownMachine.h"
#include "LevelReader.h"
#include "fmod_studio.hpp"

using namespace NCL;
using namespace CSC8503;

void TestFMod()
{
    FMOD_RESULT result;
    FMOD::Studio::System* system = NULL;

    result = FMOD::Studio::System::create(&system); // Create the Studio System object.
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }

    // Initialize FMOD Studio, which will also initialize FMOD Core
    result = system->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}


int main() {  
   
    Window *w = Window::CreateGameWindow("CSC8508 SpeedFun!", 1280, 720);

    if (!w->HasInitialised()) {
        return -1;
    }
    TestFMod();
    // Clear timer so there's no large dt. Get time delta doesn't work.
    w->UpdateWindow();

    auto client = new Client();
  
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
       
      w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));
      client->Update(dt);
    }
    Window::DestroyGameWindow();
}