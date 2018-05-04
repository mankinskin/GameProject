#pragma once
#include <string>
#include "font.h"


namespace text
{
	class TTF
	{
		public:
			TTF( std::string filename );
			~TTF();
			void load( Font& font );
	
		private:
	};
}

