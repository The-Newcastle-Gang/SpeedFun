//
// Created by c3042750 on 30/01/2024.
//

#ifndef CSC8503_FONT_H
#define CSC8503_FONT_H

#include <map>
#include "Matrix4.h"
#include "OGLShader.h"

using namespace NCL;
using namespace Rendering;

class Font {
public:

    struct Vector2i {
        int x;
        int y;
    };

    struct Character {
        unsigned int textureId;
        Vector2i glyphSize;
        Vector2i baselineOffset;
        unsigned int advanceOffset;
    };

    std::map<char, Character> characters;
    unsigned int textVAO;
    unsigned int textVBO;

    std::shared_ptr<OGLShader> fontShader;
};


#endif //CSC8503_FONT_H
