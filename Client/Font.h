//
// Created by c3042750 on 30/01/2024.
//

#ifndef CSC8503_FONT_H
#define CSC8503_FONT_H

#include <map>
#include "glad/glad.h"

class Font {
public:
    struct Vector2i {
        int x;
        int y;
    };

    struct Character {
        unsigned int TextureID;  // ID handle of the glyph texture
        Vector2i   Size;       // Size of glyph
        Vector2i  Bearing;    // Offset from baseline to left/top of glyph
        unsigned int Advance;    // Offset to advance to next glyph
    };

    std::map<char, Character> characters;
    GLuint textVAO;
    GLuint textVBO;
};


#endif //CSC8503_FONT_H
