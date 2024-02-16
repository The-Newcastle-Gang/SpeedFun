//
// Created by idhant on 16-02-2024.
//

#include "GrapplingState.h"

GrapplingState::GrapplingState(PhysicsObject *physObj) {

}

void GrapplingState::Update(float dt){
    std::cout << "in grappling state \n";
    time += dt;
    if(time> 2.0f){
        isGrappling =false;
    }

}
void GrapplingState::OnEnter() {
    isGrappling = true;


}

bool GrapplingState::HasGrappleCompleted() {
    return !isGrappling;
}