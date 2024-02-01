//
// Created by c3042750 on 30/01/2024.
//

#include "Client.h"

void Client::InitClient() {

    NetworkBase::Initialise();


    auto address = GetAddress();

    baseClient = std::make_unique<GameClient>();
    baseClient->Connect(address, NetworkBase::GetDefaultPort());

    std::cout << "Client starting up!" << std::endl;


}

std::string Client::GetAddress() {
    // TODO: Replace this with a way to input a certain address.
    return "127.0.0.1";
}

void Client::UpdateClient(float dt) {

}
