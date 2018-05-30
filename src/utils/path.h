#pragma once
#include <string>

#ifdef _WIN32
const unsigned char os_sep = '\\';
#else
const unsigned char os_sep = '/';
#endif

inline std::string getExtension( const std::string& pFilepath )
{
	unsigned int pos = pFilepath.find_last_of( '.' );
	return pos == std::string::npos
				? ""
				: pFilepath.substr( pos + 1 );
}
inline std::string getFilename( const std::string& pFilepath )
{
	unsigned int sep = pFilepath.find_last_of( "/\\" );
	if ( sep == std::string::npos ) {
		sep = 0;
	} else {
		sep += 1;
	}
	unsigned int dot = pFilepath.find_last_of( '.', sep );
	return pFilepath.substr( sep, dot );
}
