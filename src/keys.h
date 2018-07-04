#pragma once
#include "contextwindow.h"
#include "signal.h"
#include "event.h"

namespace input
{
    class KeyCondition
    {
        public:
            KeyCondition()
                : action( 0 ) // 0 = release, 1 = press
            {}
            KeyCondition( int pAction )
                : action( pAction )
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
            : key( -1 )
            , change( KeyCondition() )
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
    struct KeySignal
    {
        KeySignal()
        {}
        KeySignal( int pKey );
        signals::ListenerID press;
        signals::ListenerID release;
    };

    extern KeySignal key_esc;
    extern KeySignal key_c;
    extern KeySignal key_g;
    extern KeySignal key_h;
    extern KeySignal key_i;
    extern KeySignal key_w;
    extern KeySignal key_s;
    extern KeySignal key_a;
    extern KeySignal key_d;
    extern KeySignal key_space;
    extern KeySignal key_z;
    extern KeySignal key_f;
    extern KeySignal key_n;
    extern KeySignal key_j;
    extern KeySignal key_up;
    extern KeySignal key_down;
    extern KeySignal key_left;
    extern KeySignal key_right;
    extern KeySignal key_o;
    extern KeySignal key_l;
    extern KeySignal key_lshift;
    extern KeySignal key_x;

    void key_Callback( GLFWwindow* window, int pKey, int pScancode, int pAction, int pMods );
    void char_Callback( GLFWwindow* window, unsigned int pCodepoint );
}
