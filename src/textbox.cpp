#include "textbox.h"
#include "font.h"

utils::Container<text::Textbox> text::Textbox::all = utils::Container<text::Textbox>();

void text::setupTextboxes()
{
  text::Textbox::ID term_box1 = text::Textbox::all.makeID(text::Textbox(glm::vec2(-1.0f, 0.0f), glm::vec2(0.5f, 1.0f)));
  term_box1->getText() = "J - change camera\nC - Toggle cursor\nG - Toggle Coordinate System\nH - Toggle Axis";
  text::Textbox::ID term_box2 = text::Textbox::all.makeID(text::Textbox(glm::vec2(0.0f, 0.0f), glm::vec2(0.2f, 0.5f)));
  term_box2->getText() = "A textbox\n\nYo sup";
  //term_box2->setFont(1);
}
