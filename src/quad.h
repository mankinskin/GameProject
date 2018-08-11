#pragma once
#include <cstring>
#include <glm.hpp>
#include <vector>
#include <tuple>
#include "utils/id.h"
#include "storage.h"
#include "color.h"
#include "signal.h"

namespace gui
{
    struct Quad : public glm::vec4
    {
        using ID = utils::ID<Quad>;
        constexpr static typename ID::Container& all = ID::container;
        Quad(glm::vec4 pData)
            : glm::vec4(pData)
        {}
        void setPos(const glm::vec2 p);
        void move(const glm::vec2 v);
        void resize(const glm::vec2 v);
    };

    typedef utils::ID<Quad> QuadID;

    struct QuadElement : public utils::ID<Quad>, signals::ButtonSignals<utils::ID<Quad>>
    {
        using Preset = Quad;
        struct Data
        {
            Data(const Quad q)
                : quad(utils::makeID(q))
            {}
            Data(const utils::ID<Quad> id)
                : quad(id)
            {}
            const utils::ID<Quad> quad;
        };
        QuadElement(const Data data)
            : utils::ID<Quad>(data.quad)
            , signals::ButtonSignals<utils::ID<Quad>>(data.quad)
        {}
        QuadElement(const std::tuple<utils::ID<Quad>> id)
            : utils::ID<Quad>(std::get<0>(id))
            , signals::ButtonSignals<utils::ID<Quad>>(std::get<0>(id))
        {}
    };

    const unsigned int MAX_QUAD_COUNT = 10000;
    void setQuadPos(const utils::ID<Quad> q, const glm::vec2 p);
    void moveQuad(const utils::ID<Quad> q, const glm::vec2 v);
    utils::ID<Quad> topQuadAtPosition(const float x, const float y);

    void initQuadBuffer();
    void updateQuadBuffer();
    extern gl::StreamStorage<glm::vec4> quadBuffer;
}
