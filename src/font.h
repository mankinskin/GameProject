#pragma once
#include <vector>
#include <string>
#include "utils.h"
#include "fontfile.h"

namespace text 
{
	struct Font
	{
		Font()
		{
		}

		Font( const FontFile& font );
		gl::Storage<glm::vec4> uvBuffer;
		gl::Storage<glm::vec2> sizeBuffer;
		gl::StreamStorage<glm::vec2> posBuffer;
		gl::StreamStorage<unsigned int> charBuffer;
		texture::Texture2D atlasTexture;
		void render() const;

		static void setTargetResolution( const unsigned int rx, const unsigned int ry );
		static void setTargetResolution( glm::uvec2 pRes );
		void uploadChars() const;
		void uploadPositions() const;

		std::string name;
		std::vector<glm::vec2> positions;
		std::vector<unsigned int> chars;
		static glm::vec2 pixel_quantize( glm::vec2 v )
		{
			return glm::round( v / pixel_size ) * pixel_size;
		}
		struct Metric
		{
			Metric()
			{
			}
			Metric( float adv, float bx, float by )
				:advance ( adv ), bearing( glm::vec2( bx, by ) )
			{
			}
			
			float advance;
			glm::vec2 bearing;
		};
		std::vector<Metric> metrics;
		float linegap;
		static glm::uvec2 resolution;
		static glm::vec2 pixel_size;
	};

	extern std::vector<Font> fonts;
    using FontID = utils::Itr<Font, std::vector<Font>, fonts>;


	
	extern FontID mainFont;
	void loadFonts();

	void initFontVAO();
	
	void initFontShader();
	void setupFontShader();
	void updateFonts();
	void renderFonts();

	void renderFont();
}

