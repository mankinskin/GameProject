#pragma once
#include <string>
#include <algorithm>

namespace path
{
    const unsigned char WINDOWS_PATH_SEP = '\\';
    const unsigned char LINUX_PATH_SEP = '/';

#ifdef _WIN32
    static const unsigned char os_sep = WINDOWS_PATH_SEP;
#else
    static const unsigned char os_sep = LINUX_PATH_SEP;
#endif

    inline void natify( std::string& path )
    {
#ifdef _WIN32
        std::replace( path.begin(), path.end(), LINUX_PATH_SEP, WINDOWS_PATH_SEP );
#else
        std::replace( path.begin(), path.end(), WINDOWS_PATH_SEP, LINUX_PATH_SEP );
#endif
    }

    inline bool isAbsolute( const std::string& path ) 
    {
#ifdef _WIN32
        return ( path.size() >= 3 ) && ( path[1] == ':' );
#else
        return !path.empty() && path[0] == os_sep;
#endif
    }

    inline bool isRelative( const std::string& path ) 
    {
        return !isAbsolute( path );
    }

    inline std::string getExtension( const std::string& path )
    {
        unsigned int pos = path.find_last_of( '.' );
        return pos == std::string::npos
            ? ""
            : path.substr( pos + 1 );
    }

    inline std::string getFilename( const std::string& filepath )
    {
        unsigned int sep = filepath.find_last_of( "/\\" );
        if ( sep == std::string::npos ) {
            sep = 0;
        } else {
            sep += 1;
        }
        unsigned int dot = filepath.find_last_of( '.', sep );
        return filepath.substr( sep, dot );

    }
    inline std::string getPath( const std::string& filepath )
    {
        unsigned int sep = filepath.find_last_of( "/\\" );
        if ( sep == std::string::npos ) {
            return "";
        } else {
            sep += 1;
        }
        return filepath.substr( 0, sep );
    }
}
