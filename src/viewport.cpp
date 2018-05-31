#include "viewport.h"

gl::Viewport gl::screenViewport = gl::Viewport();
gl::Viewport* gl::Viewport::current = nullptr;

gl::Viewport::Viewport()
{ }

gl::Viewport::Viewport( app::Window window, float res ) 
    :width( window.width ), height( window.height ), resolution( res ) 
{ }

void gl::Viewport::bind()
{
    current = this;
    glViewport( 0, 0, width, height );
}
unsigned int gl::getWidth()
{
    return Viewport::current->width;
}
unsigned int gl::getHeight()
{   
    return Viewport::current->height;
}
float gl::getAspectRatio()
{
    return getWidth()/getHeight();
}
