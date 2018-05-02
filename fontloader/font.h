#pragma once
#include <vector>
#include <glm.hpp>
#include <string>

struct Font
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
		unsigned int count;
		std::vector<glm::uvec4> quads;
		std::vector<Metric> metrics;
		void resize( size_t size )
		{
			count = size;
			quads.resize( count );
			metrics.resize( count );
		}
		Glyphs()
			:count( 0 )
		{
		}
	};
	struct Atlas
	{
		unsigned int width;
		unsigned int height;
		std::vector<unsigned char> pixels;
		Atlas()
			:width( 0 ), height( 0 ) 
		{
		}
	};
	Atlas atlas;
	Glyphs glyphs;
	std::string name;

	Font()
	{
	}

	void save();

};
