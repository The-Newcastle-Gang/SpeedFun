//
// Created by jdhyd on 2/12/2024.
//

#ifndef CSC8503_TWEENMANAGER_H
#define CSC8503_TWEENMANAGER_H

# define PI_VALUE 3.14159265358979323846

#include "entt.hpp"
#include <cmath>

class TweenManager {
public:
    TweenManager() {};

    ~TweenManager();

    template <typename T>
    void CreateTween(const std::function<float(float)>& curveFunction, T origin, T end, T* valueToChange, float duration, const std::function<void()>& callback = []()->void{}) {

        for (auto i = tweens.begin(); i != tweens.end();) {
            auto tween = (Tween<T>*)(*i);
            if (tween->valueToChange == valueToChange) {
                tween->Finish();
                delete (*i);
                i = tweens.erase(i);
            } else {
                i++;
            }
        }

        auto tween = new Tween<T>();
        tween->curveFunction = curveFunction;
        tween->origin = origin;
        tween->end = end;
        tween->valueToChange = valueToChange;
        tween->duration = duration;
        tween->elapsed = 0;
        tween->callback = callback;
        tween->shouldDie = false;
        tweens.push_back((BaseTween*)tween);
    }

    void Update(float dt);

    static float Linear(float t) {
        return t;
    }

    static float EaseOutSine(float t) {
        return sinf((t * (float)PI_VALUE) * 0.5f);
    }

    static float EaseOutElastic(float t) {
        float c4 = (2.0f * PI_VALUE) / 3.0f;
        return t == 0 ? 0 : t == 1 ? 1 : powf(2, -10 * t) * sinf((t * 7.0f - 0.75f) * c4) + 1;
    }

private:
    struct BaseTween {
        bool shouldDie;
        virtual ~BaseTween() = default;
        virtual void Update(float dt) = 0;
        virtual void Finish() = 0;
    };

    template <typename T>
    struct Tween : BaseTween {
        std::function<float(float)> curveFunction;
        T origin;
        T end;
        float duration;
        float elapsed;
        T* valueToChange;
        std::function<void()> callback;

        void Update(float dt) override {
            elapsed += dt;

            elapsed = std::min(elapsed, duration);

            float tValue = elapsed / duration;
            float adjustedTValue = curveFunction(tValue);
            *valueToChange = origin + (end - origin) * adjustedTValue;

            if (elapsed >= duration) {
                // Just putting this in case there are floating point shenanigans
                // that lead to imprecise endings.
                origin = end;
                shouldDie = true;
            }
        }

        void Finish() override {
            callback();
        }
    };

    std::vector<BaseTween*> tweens;
};


#endif //CSC8503_TWEENMANAGER_H
