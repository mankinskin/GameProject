#pragma once
#include "contextwindow.h"

namespace input 
{
    class KeyCondition 
    {
        public:
            KeyCondition()
                :action( 0 ) // 0 = release, 1 = press
            {}
            KeyCondition( int pAction )
                :action( pAction )
            {}
            int action;
    };

    inline bool operator==( KeyCondition const & l, KeyCondition const& r )
    {
        return l.action == r.action;
    }
    inline bool operator!=( KeyCondition const & l, KeyCondition const& r ) 
    {
        return l.action != r.action;
    }

    struct KeyEvent 
    {
        KeyEvent()
            :key( -1 ), change( KeyCondition() ) 
        {}
        KeyEvent( int pKey, KeyCondition pChange );
        KeyEvent( int pKey, int pAction );

        int key;
        KeyCondition change;
    };

    inline bool operator==( KeyEvent const & l, KeyEvent const& r ) 
    {
        return l.key == r.key && l.change == r.change;
    }
	KeyEvent KeyEventFromScancode( int pScancode, int pAction );
    struct KeySignal 
    {
        KeySignal()
        {}
        KeySignal( int pKey );
        unsigned int hold;
        unsigned int press;
        unsigned int release;
    };

    void reserveKeySignals( unsigned int pCount );
    void key_Callback( GLFWwindow* window, int pKey, int pScancode, int pAction, int pMods );
    void char_Callback( GLFWwindow* window, unsigned int pCodepoint );
}
