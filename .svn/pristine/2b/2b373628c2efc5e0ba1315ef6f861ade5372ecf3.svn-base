/******************************************************************/
/* igluVideoTexture2D.cpp                                         */
/* -----------------------                                        */
/*                                                                */
/* The file defines a texture class that encapsulates a video     */
/*    texture loaded using the IGLUVideo class.  The IGLUVideo    */
/*    class (and hence the IGLUVideoTexture2D class) require      */
/*    linking with the FFmpeg libraries.  See igluVideo.h for     */
/*    more details.                                               */
/*                                                                */
/* Chris Wyman (10/06/2011)                                       */
/******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "igluVideoTexture2D.h"

using namespace iglu;


IGLUVideoTexture2D::IGLUVideoTexture2D( char *filename, unsigned int flags, bool initializeImmediately ) : 
	IGLUTexture()
{
	m_filename      = strdup( filename );
	m_videoTex      = new IGLUVideo( filename );
	m_minFilter     = GetMinification( flags );
	m_magFilter     = GetMagnification( flags );
	m_sWrap         = GetSWrap( flags );
	m_tWrap         = GetTWrap( flags );
	m_mipmapsNeeded = UsingMipmaps( flags );
	m_repeatVideo   = flags & IGLU_REPEAT_VIDEO ? true : false;
	m_compressTex   = false;

	m_width         = m_videoTex->GetWidth();
	m_height        = m_videoTex->GetHeight();
	m_pixelFormat   = m_videoTex->GetGLFormat();

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
		//     core dump).  Here we check glGenerateMipmap(), which might feasibly be used
		//     at some future point in this class.
		if( !glGenerateMipmap )     // In my code, this is non-NULL after glewInit()
		{
			printf("*** ERROR!  IGLUVideoTexture2D() constructor called with initializeImmediately\n");
			printf("            flag enabled, but OpenGL context has not yet been created! Textures\n");
			printf("            created in this manner are invalid.  Set this flag to false, and\n");
			printf("            call IGLUVideoTexture2D::Initialize() manually after an OpenGL\n");
			printf("            context is available!\n");
			exit(-1);  // This may also occur if the supported OpenGL version is < 3.0, when 
			           //     glGenerateMipmap was added to the core OpenGL spec.
		}

		// Ok, GL is setup.  We can initialize our texture!
		Initialize();
	}
}

IGLUVideoTexture2D::~IGLUVideoTexture2D()
{
	if (m_filename) free( m_filename );
	if (m_videoTex) delete m_videoTex;
}

void IGLUVideoTexture2D::Initialize( void )
{
	// Make sure to avoid doubly-calling initialize
	if (m_initialized) return;

	// Make sure we have a video frame to put in our texture!
	m_videoTex->GetNextFrame();

	// OK, now we can set up our texture
	glBindTexture( GL_TEXTURE_2D, m_texID );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_minFilter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_magFilter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_sWrap );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_tWrap );
	glTexImage2D( GL_TEXTURE_2D, 0, GetCompressedFormat( m_videoTex->GetGLFormat() ),  
						   m_videoTex->GetWidth(), m_videoTex->GetHeight(), 0,
						   m_videoTex->GetGLFormat(), m_videoTex->GetGLDatatype(), 
						   (void *)m_videoTex->GetFrameData() );
	if (m_mipmapsNeeded)
		glGenerateMipmap( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, 0 );

	// Done initializing
	m_initialized = true;
}

void IGLUVideoTexture2D::Update( void )
{
	if (!m_initialized) return;

	// Make sure we have a video frame to put in our texture!
	if ( !m_videoTex->GetNextFrame() && m_repeatVideo )  
		m_videoTex->SeekToFrame( 0 ); // If we finish the video, start over!

	// Update our texture based on the new video frame!
	glBindTexture( GL_TEXTURE_2D, m_texID );
	glTexImage2D( GL_TEXTURE_2D, 0, GetCompressedFormat( m_videoTex->GetGLFormat() ),  
						   m_videoTex->GetWidth(), m_videoTex->GetHeight(), 0,
						   m_videoTex->GetGLFormat(), m_videoTex->GetGLDatatype(), 
						   (void *)m_videoTex->GetFrameData() );
	if (m_mipmapsNeeded)
		glGenerateMipmap( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, 0 );
}

void IGLUVideoTexture2D::Update( float frameTime )
{
	assert(0); // Not implemented!
}
