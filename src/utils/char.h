#pragma once

namespace utils
{
  bool isSpace(const unsigned char c)
  {
	return c == ' ';
  }
  bool isTab(const unsigned char c)
  {
	return c == '\t';
  }
  bool isNewline(const unsigned char c)
  {
	return c == '\n';
  }

  bool isWhitespace(const unsigned char c)
  {
	return isSpace(c) || isTab(c) || isNewline(c);
  }

  bool isBackspace(const unsigned char c)
  {
	return c == '\r';
  }
}
