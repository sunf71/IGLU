/******************************************************************/
/* igluTextureBuffer.cpp                                          */
/* -----------------------                                        */
/*                                                                */
/* The file defines an OpenGL texture buffer class that allows    */
/*    the creation of a texture buffer object that can be used    */
/*    with shader programs and supports reading of arbitrary      */
/*    IGLUBuffers inside GLSL programs using a samplerBuffer.     */
/*                                                                */
/* Chris Wyman (01/06/2012)                                       */
/******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "iglu.h"

using namespace iglu;


IGLUTextureBuffer::IGLUTextureBuffer( bool initializeImmediately ) : 
	IGLUTexture(), m_internalFormat(0), m_texBuffer(0)
{
	// Ok, the user wants us to initialize the OpenGL texture immediately (not wait until
	//    they later call Initialize() separately).
	if (initializeImmediately)
	{
		// First check that we have an OpenGL context, or texture initialization will fail
		//     silently -- leading to very hard-to debug errors.  Unfortunately, detecting
		//     a valid context relies on wgl/glx/agl, etc, and so is not OS-independent.
		//     Instead, we'll check a function that needs to be initialized by some extension
		//     loader (to grab the function pointer from the OpenGL .dll/.so/etc).  If this
		//     has not been done, then extensions haven't been processed yet, which likely
		//     means we have no context.  (Or at the least advanced GL functionality will
		//     core dump).  Here we check glTexBuffer(), which is used in the class.
		if( !glTexBuffer )     // In my code, this is non-NULL after glewInit()
		{
			printf("*** ERROR!  IGLUTextureBuffer() constructor called with initializeImmediately\n");
			printf("            flag enabled, but OpenGL context has not yet been created! Textures\n");
			printf("            created in this manner are invalid.  Set this flag to false, and\n");
			printf("            call IGLUTextureBuffer::Initialize() manually after an OpenGL\n");
			printf("            context is available!\n");
			exit(-1);  // This may also occur if the supported OpenGL version is < 3.3, when 
			           //     glTexBuffer was added to the core OpenGL spec.
		}

		// Ok, GL is setup.  We can initialize our texture!
		Initialize();
	}
}


IGLUTextureBuffer::~IGLUTextureBuffer()
{
}

void IGLUTextureBuffer::Initialize( void )
{
	// Make sure to avoid doubly-calling initialize
	if (m_initialized) return;

	// OK, now we can "set up" our texture buffer.  This doesn't really do anything, but at least we
	//     will have bound the texture ID and done something to it (even if it's an unbinding)
	glBindTexture( GL_TEXTURE_BUFFER, m_texID );
	glTexBuffer  ( GL_TEXTURE_BUFFER, GL_RGBA32F, 0 );  // Doesn't really do anything.
	glBindTexture( GL_TEXTURE_BUFFER, 0 );

	// Done initializing
	m_initialized = true;
}

void IGLUTextureBuffer::BindBuffer( GLenum bufTexelType, const IGLUBuffer *buffer, bool warnOnTypeChange )
{
	// If this texture buffer has already been assigned to a GLSL sampler and the type changes,
	//     the buffer may no longer be bound correctly (i.e., it may need an 'isamplerBuffer'
	//     instead of a 'samplerBuffer').  These GLSL type mismatches cause silent failure of
	//     shaders to execute correctly.  This warning alerts developers to the fact something
	//     bad may be occurring so they can appropriately debug.
	if (warnOnTypeChange && (m_internalFormat > 0) && (m_internalFormat != bufTexelType))
	{
		printf("***Warning: IGLUTextureBuffer::BindBuffer() called with a change to internal\n"); 
		printf("            buffer format, which may evade IGLU run-time type checking and\n");
		printf("            result in undetected type-mismatches and undefined GLSL results!\n");
	}

	m_internalFormat = bufTexelType;
	m_texBuffer      = buffer;

	glBindTexture( GL_TEXTURE_BUFFER, m_texID );
	glTexBuffer  ( GL_TEXTURE_BUFFER, m_internalFormat, buffer ? buffer->GetBufferID() : 0 );
	glBindTexture( GL_TEXTURE_BUFFER, 0 );
}

void IGLUTextureBuffer::UnbindBuffer( void )
{
	// Unbind any existing buffer, but remember its type to help detect errors (see BindBuffer())
	if (m_texBuffer)
	{
		glBindTexture( GL_TEXTURE_BUFFER, m_texID );
		glTexBuffer  ( GL_TEXTURE_BUFFER, m_internalFormat, 0 );
		glBindTexture( GL_TEXTURE_BUFFER, 0 );
		m_texBuffer = 0;
	}
}

