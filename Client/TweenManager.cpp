//
// Created by jdhyd on 2/12/2024.
//

#include "TweenManager.h"


TweenManager::~TweenManager() {
    for (auto t : tweens) {
        delete t;
    }
}


void TweenManager::Update(float dt) {
    for (auto i = tweens.begin(); i != tweens.end();) {
        auto tween = (*i);
        tween->Update(dt);
        if (tween->shouldDie) {
            tween->Finish();
            delete tween;
            i = tweens.erase(i);
        } else {
            i++;
        }
    }
}
