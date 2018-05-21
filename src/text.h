#pragma once
#include "font.h"
#include <glm.hpp>
#include <vector>
#include "utils.h"

namespace text
{
	struct Text
	{
		public:
			glm::vec2 pos;
			glm::vec2 size;
			Text( glm::vec2 pPos = glm::vec2( 0.0f, 0.0f ), glm::vec2 pSize = glm::vec2( 1.0f, 1.0f ) )
				:font( FontID( 0 ) ), pos( pPos ), size( pSize )
			{
			}
			void print( const std::string& str );
			void writeChars();
			void setFont( FontID );
		private:
			void lineBreak();
			glm::vec2 cursor; // relative to pos
			std::string str;
			FontID font;
	};
	extern std::vector<Text> texts;
    using TextID = utils::Itr<Text, std::vector<Text>, texts>; 

	void updateTexts();

}
