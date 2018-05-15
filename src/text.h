#pragma once
#include "font.h"
#include "texture.h"

namespace text
{
	struct LoadedFont
	{
		
		LoadedFont()
		{
		}

		LoadedFont( const Font& font );
		gl::Storage<glm::vec4> uvBuffer;
		gl::Storage<glm::vec2> sizeBuffer;
		gl::StreamStorage<glm::vec2> posBuffer;
		gl::StreamStorage<unsigned int> charBuffer;
		texture::Texture2D atlasTexture;
		void render();

		static void setTargetResolution( const unsigned int rx, const unsigned int ry );
		static void setTargetResolution( glm::uvec2 pRes );
		void uploadChars();
		void uploadPositions();

		void print( const std::string& str, glm::vec2 pos = glm::vec2() );

		private:
		struct LoadedMetric
		{
			LoadedMetric()
			{
			}
			LoadedMetric( float adv, float bx, float by )
				:advance ( adv ), bearingx( bx ), bearingy( by )
			{
			}
			float advance;
			float bearingx;
			float bearingy;
		};
		std::vector<LoadedMetric> metrics;
		std::vector<glm::vec2> positions;
		std::vector<unsigned int> chars;
		static std::vector<LoadedFont&> allFonts;
		static glm::uvec2 resolution;
	};

	LoadedFont createFont( const Font& font );

	extern LoadedFont mainFont;
	void loadFonts();

	void initFontVAO();
	
	void initFontShader();
	void setupFontShader();

	void renderFont();
}
