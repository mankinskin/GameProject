#pragma once
#include <cstring>
#include <glm.hpp>
#include <vector>
#include <tuple>
#include "utils/id.h"
#include "storage.h"
#include "vao.h"
#include "color.h"

namespace gui
{
    struct Quad
    {
        using ID = utils::ID<Quad>;
        constexpr static typename ID::Container& all = ID::container;
        Quad(glm::vec4 pData)
            :data(pData)
        {}
        glm::vec4 data;
        void setPos(const glm::vec2 p);
        void move(const glm::vec2 v);
        void resize(const glm::vec2 v);
    };

    struct QuadEvent    // signals thrown when entering or leaving quads with the cursor
    {
        QuadEvent(const utils::ID<Quad> pQuad, int pEnter)
            :quad(pQuad), enter(pEnter)
        {}
        const utils::ID<Quad> quad;
        int enter;
    };

    inline bool operator==(const QuadEvent& l, const QuadEvent& r)
    {
        return l.quad == r.quad;
    }

    typedef utils::ID<Quad> QuadID;


    const unsigned int MAX_QUAD_COUNT = 10000;
    void setQuadPos(const utils::ID<Quad> q, const glm::vec2 p);
    void moveQuad(const utils::ID<Quad> q, const glm::vec2 v);

    void initQuadBuffer();
    void updateQuadBuffer();
    extern gl::StreamStorage<glm::vec4> quadBuffer;
}
