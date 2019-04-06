#pragma once
#include "gui.h"
#include <glm.hpp>
#include <vector>
#include "utils/id.h"
#include "quad.h"
#include "viewport.h"
#include "font.h"

namespace text
{
  struct Textbox : public gui::Quad
  {
    public:
      static utils::Container<Textbox> all;
      struct ID : public utils::ID<Textbox>
      {
        ID()
          : utils::ID<Textbox>(&all)
        {}
        ID(const size_t i)
          : utils::ID<Textbox>(i, &all)
        {}
        ID(const utils::ID<Textbox>& id)
          : utils::ID<Textbox>(std::forward<const utils::ID<Textbox>&>(id))
        {}
        ID(utils::ID<Textbox>&& id)
          : utils::ID<Textbox>(std::move(id))
        {}
      };

      Textbox(glm::vec2 pPos = glm::vec2(0.0f, 0.0f),
              glm::vec2 pSize = glm::vec2(1.0f, 1.0f))
        : Quad(gl::pixel_round(pPos), gl::pixel_round(pSize))
        , font(Font::ID(0))
        , textIndex(font->makeText(glm::vec4(pPos, pSize)))
      {}

      void update() const;

      Font::ID font;
      size_t textIndex;

      Text& getText() const
      {
        return font->texts[textIndex];
      }
  };
}
