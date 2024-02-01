//
// Created by c3042750 on 30/01/2024.
//

#pragma comment(lib, "winmm.lib")

#include "GameTimer.h"
#include "Server.h"

#include <iostream>
#include <thread>


int main() {
    auto timer = GameTimer();
    auto server = std::make_unique<Server>();
    server->ServerInit();
    //Resets delta time so one massive from at the start doesn't go off.
    timer.Tick();

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
}