#include "keys.h"
#include "signal.h"
#include "event.h"
#include "gates.h"

using namespace signals;
using namespace events;

#define SCANCODE_SPACE 56
int deriveScancode( int pKey ) 
{
    if ( pKey == GLFW_KEY_SPACE ) {
        return SCANCODE_SPACE;
    }
    return 0;
}
input::KeyEvent::KeyEvent( int pKey, KeyCondition pChange )
    :key( pKey ), change( pChange ) 
{}
input::KeyEvent::KeyEvent( int pKey, int pAction )
    :key( pKey ), change( KeyCondition( pAction ) ) 
{}
input::KeyEvent input::KeyEventFromScancode( int pScancode, int pAction )
{
    return KeyEvent( GLFW_KEY_UNKNOWN, pAction );
}
input::KeySignal::KeySignal( int pKey )
{
    int scancode = 0;
    scancode = deriveScancode( pKey );
    Event press_evt = createEvent( KeyEvent( pKey, KeyCondition( 1 ) ) );
    Event release_evt = createEvent( KeyEvent( pKey, KeyCondition( 0 ) ) );
    press = createSignal( press_evt );
    release = createSignal( release_evt );
    gates::switch_gate<Event, Event> togglers( press_evt, release_evt );
    hold = createSignal( togglers );
}

void input::reserveKeySignals( unsigned int pCount ) 
{
    allSignals.reserve( allSignals.size() + pCount );
    EventSlot<KeyEvent>::reserve_slots( pCount*3 );
}
void input::key_Callback( GLFWwindow * window, int pKey, int pScancode, int pAction, int pMods )
{
    pushEvent( KeyEvent( pKey, pAction ) );
}

void input::char_Callback( GLFWwindow * window, unsigned int pCodepoint )
{
    //printf( "char callBack! Char: %c\n", pCodepoint );
}
