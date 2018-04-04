#pragma once
#include <array>
#include <tuple>
#include <utility>
#include <vector>
#include <glm.hpp>

#include "color.h"
#include "quad.h"
#include "quadcolors.h"
#include "utils.h"
#include "hierarchy.h"
#include "hierarchy_utils.h"


//-----Widgets
// This class is rather complex. It also does a little too many things in one place.
// However it provides a very efficient and stable, yet extremely flexible foundation for a class hierarchy
//
// To create a Widget composed of 2D quads you have to provide some data:
// - the initial data to be loaded into each element(quad) of the widget
// - the policies about how the element should pass an input command to its subelements

namespace gui {

    struct Button 
    {
        using Quads = utils::gen_Element_t<Quad, 2>;
        using QuadIniter = utils::gen_Element_t<glm::vec4, 2>;

        using Colors = utils::gen_Element_t<gl::ColorIt, 2>;

        using MoveRule = utils::gen_Element_t<glm::vec2, 2>;
        using ResizeRule = utils::gen_Element_t<glm::vec4, 2>;
    };

    struct Window
    {
        struct Frame
        {
            using Quads = utils::gen_Element_t<Quad, 9>; 
            using QuadIniter = utils::gen_Element_t<glm::vec4, 9>;

            using Colors = utils::gen_Element_t<gl::ColorIt, 9>;
            using ColorIniter = utils::gen_Element_t<glm::vec4, 9>;

            using MoveRule = utils::gen_Element_t<glm::vec2, 9>;
            using ResizeRule = utils::gen_Element_t<glm::vec4, 9>;
        };

        struct Header 
        {
            using Quads = utils::Element<Quad, Quad>;
            using QuadIniter = utils::Element<glm::vec4, glm::vec4>;

            using Colors = utils::Element<gl::ColorIt, gl::ColorIt>;
            using ColorIniter = utils::Element<glm::vec4, glm::vec4>;

            using MoveRule = utils::gen_Element_t<glm::vec2, 2>;
            using ResizeRule = utils::gen_Element_t<glm::vec4, 2>;
        };

        using Quads = utils::Element<Frame::Quads, Header::Quads>;
        using QuadIniter = utils::Element<Frame::QuadIniter, Header::QuadIniter>;

        using Colors = utils::Element<Frame::Colors, Header::Colors>;
        using ColorIniter = utils::Element<Frame::ColorIniter, Header::ColorIniter>;

        using MoveRule = utils::Element<Frame::MoveRule, Header::MoveRule>;//utils::gen_Element_t<glm::vec2, 2>;
        using ResizeRule = utils::gen_Element_t<glm::vec4, 2>;
    };

    template<typename... Qs, typename Vec>
        void moveQuads(const utils::Element<Qs...> elem, Vec v)
        {
            utils::foreach( gui::moveQuad, elem, v );
        }

    template<typename... Qs, typename Vec, typename Scal>
        void moveQuadsScaled(const utils::Element<Qs...> elem, Vec v, Scal scale)
        {
            utils::foreach( gui::moveQuadScaled, elem, v, scale);
        }
}

