#include "Mouse.h"
#include "app.h"
#include "ContextWindow.h"
#include <algorithm>
#include "Event.h"
#include <array>
#include "quadindex.h"
#include "gl.h"
#include "viewport.h"

using namespace events;
glm::vec2 input::relativeCursorPosition;
glm::uvec2 input::absoluteCursorPosition;
glm::vec2 input::cursorFrameDelta;

std::array<input::KeyCondition, 3> mouseKeys;
int scroll = 0;
int disableCursor = 0;
unsigned int hovered_quad = 0;
unsigned int last_hovered_quad = 0;

void input::updateMouse()
{
	//update cursor pos
	double ax = 0.0;//absolute positions
	double ay = 0.0;
	
	//GLFW window coordinates are from top to bottom all others are bottom to top
	glfwGetCursorPos( app::mainWindow.window, &ax, &ay );
	float rx = ( float )ax;
	float ry = ( float )ay;
	ax = glm::clamp( ax*gl::Viewport::current->resolution, 0.0, ( double )( app::mainWindow.width*gl::Viewport::current->resolution ) - 1.0 );
	ay = glm::clamp( ay*gl::Viewport::current->resolution, 0.0, ( double )( app::mainWindow.height*gl::Viewport::current->resolution ) - 1.0 );
	if ( !disableCursor ) {
		rx = ( float )ax;//clamp relative positions too if cursor is not disabled
		ry = ( float )ay;
	}
	absoluteCursorPosition = glm::uvec2( ( unsigned int )ax, ( app::mainWindow.height*gl::Viewport::current->resolution ) - ( unsigned int )ay - 1 );

	rx = ( ( rx / ( float )app::mainWindow.width )*2.0f ) - 1.0f;
	ry = 1.0f - ( ry / ( float )app::mainWindow.height )*2.0f;
	glm::vec2 newRelativeCursorPosition = glm::vec2( rx, ry );
	cursorFrameDelta = glm::vec2( newRelativeCursorPosition.x - relativeCursorPosition.x,
		newRelativeCursorPosition.y - relativeCursorPosition.y );
	relativeCursorPosition = newRelativeCursorPosition;

	//printf( "RelPos: %.2f\t%.2f\n", relativeCursorPosition.x, relativeCursorPosition.y );
	//printf( "AbsPos: %u\t%u\n", absoluteCursorPosition.x, absoluteCursorPosition.y );
    //printf( "Delta: %.2f\t%.2f\n", cursorFrameDelta.x, cursorFrameDelta.y );
}

void input::resetMouse()
{
	scroll = 0;
}

void input::toggleCursor()
{
	disableCursor = !disableCursor;
	if ( disableCursor ) {
		glfwSetInputMode( app::mainWindow.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
	}
	else {
		glfwSetInputMode( app::mainWindow.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
	}
}

void input::getMouseKeyEvents()
{
	static std::array<KeyCondition, 3> lastMouseKeys;

	for ( unsigned int c = 0; c < 3; ++c ) {
		if ( mouseKeys[c] != lastMouseKeys[c] ) {//mouse key change event
			pushEvent( MouseKeyEvent( c, mouseKeys[c] ) );
		}
	}
	
	lastMouseKeys = mouseKeys;
}
void input::getCursorQuadEvents()
{
	last_hovered_quad = hovered_quad;
	hovered_quad = gui::readQuadIndexMap( absoluteCursorPosition.x, absoluteCursorPosition.y );
	float quad_depth = gui::readQuadDepthMap( absoluteCursorPosition.x, absoluteCursorPosition.y );
	if( last_hovered_quad != hovered_quad ){
		//quad change
		if ( last_hovered_quad ) {
			//button leave event
			pushEvent( QuadEvent( last_hovered_quad, 0 ) );
		}
		if ( hovered_quad ) {
			//button enter event
			pushEvent( QuadEvent( hovered_quad, 1 ) );
		}
	}
    printf( "%d\n", hovered_quad );
}

void input::mouseKey_Callback( GLFWwindow * window, int pKey, int pAction, int pMods )
{
	if ( pKey < 3 && pKey >= 0 ) {
		mouseKeys[pKey] = KeyCondition( pAction );
	}
}

void input::cursorPosition_Callback( GLFWwindow * window, double pX, double pY )
{

}
void input::scroll_Callback( GLFWwindow * window, double pX, double pY )
{

}
void input::cursorEnter_Callback( GLFWwindow* window, int pEntered )
{

}
