//
// Created by c3042750 on 10/02/2024.
//

#include "Element.h"

void Element::Update(float dt) {

    update.publish(*this, dt);

    bool isClicked = Window::GetMouse()->ButtonDown(MouseButtons::LEFT);
    Vector2 position = Window::GetMouse()->GetAbsolutePosition();

    int windowWidth = Window::GetWindow()->GetScreenSize().x;
    int windowHeight = Window::GetWindow()->GetScreenSize().y;

    // Y Coordinates are flipped, just matrix things, can make this not the case if it's important.
    position.y = windowHeight - position.y;

    float lowerBound = GetAbsolutePosition().y + GetRelativePosition().y * windowHeight - extendLowerY;
    float upperBound = lowerBound + (GetAbsoluteSize().y + GetRelativeSize().y * windowHeight) + extendUpperY;
    float leftBound = GetAbsolutePosition().x + GetRelativePosition().x * windowWidth - extendLowerX;
    float rightBound = leftBound + (GetAbsoluteSize().x + GetRelativeSize().x * windowWidth) + extendUpperX;

    if (position.x > leftBound && position.x < rightBound && position.y < upperBound && position.y > lowerBound) {
        if (isClicked) {
            if (mouseDownTimer > 0) {
                mouseHold.publish(*this, mouseDownTimer);
                mouseDownTimer += dt;
            } else {
                mouseDown.publish(*this);
                mouseDownTimer += dt;
            }
        } else {

            if (mouseDownTimer > 0) {
                mouseUp.publish(*this);
                mouseDownTimer = 0;
            }
        }

        if (hoverTimer > 0.0f) {
            mouseHover.publish(*this, hoverTimer);
            hoverTimer += dt;
        } else {
            mouseEnter.publish(*this);
            hoverTimer += dt;
        }
        return;
    }

    if (hoverTimer > 0) {
        mouseExit.publish(*this);
        hoverTimer = 0;
    }
}
