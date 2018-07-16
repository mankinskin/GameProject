#pragma once
#include "contextwindow.h"

namespace gl {

    struct Viewport{
        Viewport();
        Viewport(app::Window pWindow, float res = 1.0f);
        void bind();

        unsigned int width;
        unsigned int height;
        float resolution;

        static Viewport* current;
    };

    extern Viewport screenViewport;


    unsigned int getWidth();
    unsigned int getHeight();
    float getAspectRatio();

}
