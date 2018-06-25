#include "keys.h"
#include "signal.h"
#include "event.h"
#include "gates.h"

using namespace signals;
using namespace events;

input::KeySignal input::key_esc;
input::KeySignal input::key_c;
input::KeySignal input::key_g;
input::KeySignal input::key_h;
input::KeySignal input::key_i;
input::KeySignal input::key_w;
input::KeySignal input::key_s;
input::KeySignal input::key_a;
input::KeySignal input::key_d;
input::KeySignal input::key_space;
input::KeySignal input::key_z;
input::KeySignal input::key_f;
input::KeySignal input::key_n;
input::KeySignal input::key_j;
input::KeySignal input::key_up;
input::KeySignal input::key_down;
input::KeySignal input::key_left;
input::KeySignal input::key_right;
input::KeySignal input::key_o;
input::KeySignal input::key_l;
input::KeySignal input::key_lshift;
input::KeySignal input::key_x;


input::KeyEvent::KeyEvent( int pKey, KeyCondition pChange )
    :key( pKey ), change( pChange ) 
{}
input::KeyEvent::KeyEvent( int pKey, int pAction )
    :key( pKey ), change( KeyCondition( pAction ) ) 
{}

input::KeySignal::KeySignal( int pKey )
{
    press = Event<KeyEvent>::ID( KeyEvent( pKey, KeyCondition( 1 ) ) )->signal;
    release = Event<KeyEvent>::ID( KeyEvent( pKey, KeyCondition( 0 ) ) )->signal;
}

void input::key_Callback( GLFWwindow * window, int pKey, int pScancode, int pAction, int pMods )
{
    pushEvent( KeyEvent( pKey, pAction ) );
}

void input::char_Callback( GLFWwindow * window, unsigned int pCodepoint )
{
    //printf( "char callBack! Char: %c\n", pCodepoint );
}
