#include "framebuffer.h"
#include "gl.h"
#include "debug.h"
#include "app.h"
#include "vao.h"
#include "viewport.h"

unsigned int texture::gBuffer;
texture::Texture2D texture::gPosTexture;
texture::Texture2D texture::gNormalTexture;
texture::Texture2D texture::gAmbientTexture;
texture::Texture2D texture::gDiffuseTexture;
texture::Texture2D texture::gSpecularTexture;
unsigned int texture::gDepthRenderbuffer;
unsigned int texture::guiFBO;
texture::Texture2D texture::fontColorTexture;
texture::Texture2D texture::quadIndexTexture;
unsigned int texture::guiDepthRenderbuffer;

void texture::initFramebuffers()
{	
	initGUIFBO();
	//initGBuffer();
}

void texture::initGUIFBO() {

	glCreateFramebuffers( 1, &guiFBO );
	glCreateRenderbuffers( 1, &guiDepthRenderbuffer );

	quadIndexTexture = Texture2D( 
                gl::getWidth(), gl::getHeight(), GL_R16UI, 
                GL_RED_INTEGER, GL_UNSIGNED_BYTE );

	fontColorTexture = Texture2D( 
                gl::getWidth(), gl::getHeight(), GL_RGBA8, 
                GL_RGBA, GL_UNSIGNED_BYTE );

	glBindFramebuffer( GL_FRAMEBUFFER, guiFBO );

	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
            GL_TEXTURE_2D, quadIndexTexture, 0 );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 
            GL_TEXTURE_2D, fontColorTexture, 0 );

	glBindRenderbuffer( GL_RENDERBUFFER, guiDepthRenderbuffer );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 
            gl::getWidth(), gl::getHeight() );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
            GL_RENDERBUFFER, guiDepthRenderbuffer );

	const unsigned int attch_sz = 1;
	unsigned int attachments[attch_sz] = { GL_COLOR_ATTACHMENT0 };
	glNamedFramebufferDrawBuffers( guiFBO, attch_sz, &attachments[0] );

	GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE ) {
		debug::pushError( "Framebuffer incomplete" );
	}

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void texture::initGBuffer()
{
	glCreateFramebuffers( 1, &gBuffer );
	glCreateRenderbuffers( 1, &gDepthRenderbuffer );

	gAmbientTexture = Texture2D( gl::getWidth(), gl::getHeight(), 
                GL_RGBA16F, GL_RGBA, GL_FLOAT );
	gDiffuseTexture = Texture2D( gl::getWidth(), gl::getHeight(), 
                GL_RGBA16F, GL_RGBA, GL_FLOAT );
	gSpecularTexture = Texture2D( gl::getWidth(), gl::getHeight(), 
                GL_RGBA16F, GL_RGBA, GL_FLOAT );
	gPosTexture = Texture2D( gl::getWidth(), gl::getHeight(), 
                GL_RGB16F, GL_RGB, GL_FLOAT );
	gNormalTexture = Texture2D( gl::getWidth(), gl::getHeight(), 
                GL_RGB16F, GL_RGB, GL_FLOAT );

	glBindFramebuffer( GL_FRAMEBUFFER, gBuffer );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
            GL_TEXTURE_2D, gAmbientTexture, 0 );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 
            GL_TEXTURE_2D, gDiffuseTexture, 0 );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, 
            GL_TEXTURE_2D, gSpecularTexture, 0 );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, 
            GL_TEXTURE_2D, gPosTexture, 0 );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, 
            GL_TEXTURE_2D, gNormalTexture, 0 );

	glBindRenderbuffer( GL_RENDERBUFFER, gDepthRenderbuffer );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 
            gl::getWidth(), gl::getHeight() );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
            GL_RENDERBUFFER, gDepthRenderbuffer );
	glBindRenderbuffer( GL_RENDERBUFFER, gDepthRenderbuffer );

	const unsigned int attch_sz = 5;
	unsigned int attachments[attch_sz] = 
    { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, 
        GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
	glNamedFramebufferDrawBuffers( gBuffer, attch_sz, &attachments[0] );
	
	GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE ) {
		debug::pushError( "Framebuffer incomplete" );
	}
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}
