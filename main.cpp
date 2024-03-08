//
// Created by c3042750 on 13/02/2024.
//

#include "Client/Client.h"
#include "Server/Server.h"
#include <thread>

using namespace NCL;
using namespace CSC8503;
bool debugMode =false;


std::atomic<bool> SHOULDSERVERSTART = false;

void ServerThreadFunction() {
    auto server = new Server();
    auto timer = GameTimer();
    timer.Tick();

    while (!SHOULDSERVERSTART) {
        std::this_thread::yield();
    }

    server->InitStateMachine();

    while (true) {
        std::this_thread::yield();
        timer.Tick();
        float dt = timer.GetTimeDeltaSeconds();

        if (dt > 0.1f) {
            std::cout << "Skipping large time delta" << std::endl;
            continue;
        }

        server->UpdateServer(dt);
    }
    delete server;
}

int main() {
    Window *w = Window::CreateGameWindow("CSC8508 SpeedFun!", 1280, 720);

    if (!w->HasInitialised()) {
        return -1;
    }



    auto client = new Client(SHOULDSERVERSTART);


    Window::GetWindow()->LockMouseToWindow(false);
    Window::GetWindow()->ShowOSPointer(true);

    std::thread t1(ServerThreadFunction);
    t1.detach();

    // Clear timer so there's no large dt. Get time delta doesn't work.
    w->UpdateWindow();


    while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE)) {
        float dt = w->GetTimer()->GetTimeDeltaSeconds();
        if (dt > 0.1f) {
            std::cout << "Skipping large time delta" << std::endl;
            //continue;
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
            debugMode = !debugMode;
        }

        w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));

        if(!debugMode){
            client->Update(dt);
        }

    }
    delete client;
    Window::DestroyGameWindow();
}