#include "text.h"
#include "font.h"
#include "printer.h"

utils::Container<text::Textbox> text::Textbox::all = utils::Container<text::Textbox>();

void text::Textbox::setString(const std::string& pStr)
{
  buf = pStr;
}
std::string text::Textbox::getString() const
{
  return buf;
}

