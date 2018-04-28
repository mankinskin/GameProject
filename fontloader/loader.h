#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm.hpp>
#include <vector>
#include <string>

namespace loader
{
	struct FontMemory
	{
		struct Glyphs
		{
			struct Metric
			{
				unsigned int advance;
				unsigned int bearx;
				unsigned int beary;

				Metric()
				{
				}
				Metric( unsigned int adv, unsigned int bx, unsigned int by )
					:advance ( adv ), bearx( bx ), beary( by )
				{
				}
			};
			std::vector<glm::uvec4> quads;
			std::vector<Metric> metrics;
			unsigned int count;
			void resize( size_t size )
			{
				count = size;
				quads.resize( count );
				metrics.resize( count );
			}
			void storeGlyph( size_t index, const FT_GlyphSlotRec* pGlyph );
		};
		struct Atlas
		{
			std::vector<unsigned char> pixels;
			unsigned int width;
			unsigned int height;
			Atlas()
				:width( 0 ), height( 0 )
			{
			}
		};
		Atlas atlas;
		Glyphs glyphs;

		void print();
		int loadFile( std::string filename );
		void loadGlyphs( FT_Face& face );
		void loadAtlas( FT_Face& face );
	};

	int initFreeType();
}
