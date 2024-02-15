//
// Created by c3042750 on 12/02/2024.
//

#ifndef CSC8503_TEXTDATA_H
#define CSC8503_TEXTDATA_H

#include "Font.h"
#include "Vector4.h"
#include <string>

using namespace NCL;
using namespace Maths;

struct TextData {
    TextData() {
        color = Vector4(1.0, 1.0, 1.0, 1.0);
        text = "";
        fontSize = 1.0f;
        font = nullptr;
    }

    void SetFont(Font* f) {
        font = f;
    }

    [[nodiscard]] Font* GetFont() const {
        return font;
    }

    Font* font;
    Vector4 color;
    std::string text;
    float fontSize;
};

#endif //CSC8503_TEXTDATA_H
