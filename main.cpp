//
// Created by c3042750 on 13/02/2024.
//

#include "Client/Client.h"
#include "Server/Server.h"
#include <thread>

using namespace NCL;
using namespace CSC8503;
bool debugMode =false;


std::atomic<int> G_Should_Close = -1;

void ServerThreadFunction() {
    auto server = new Server();
    auto timer = GameTimer();
    timer.Tick();

    while (G_Should_Close != 0) {
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
    G_Should_Close = 1;
}

int main() {
    Window *w = Window::CreateGameWindow("CSC8508 SpeedFun!", 1280, 720);

    if (!w->HasInitialised()) {
        return -1;
    }

    // Clear timer so there's no large dt. Get time delta doesn't work.
    w->UpdateWindow();

    auto client = new Client();


    Window::GetWindow()->LockMouseToWindow(false);
    Window::GetWindow()->ShowOSPointer(true);

    std::thread t1(ServerThreadFunction);
    t1.detach();


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
            debugMode = !debugMode;
        }

        w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));

        if(!debugMode){
            client->Update(dt);
        }

    }

    G_Should_Close = 0;

    while (G_Should_Close == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    delete client;
    Window::DestroyGameWindow();
}