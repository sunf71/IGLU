/******************************************************************/
/* igluTexture2D.cpp                                              */
/* -----------------------                                        */
/*                                                                */
/* The file defines an image class that stores a 2D texture.      */
/*     This is very similar to the image class, but also defines  */
/*     access patterns for the texture with interpolation.        */
/*                                                                */
/* Chris Wyman (10/26/2006)                                       */
/******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <GL/glew.h>
#include "iglu.h"
#include "BuiltIns/igluBuiltInTextures.h"

using namespace iglu;

extern IGLUBuiltInTexture g_igluBuiltInTex[ IGLU_BUILTIN_TEX_MAX ];

IGLUTexture2D::IGLUTexture2D( char *filename, unsigned int flags, bool initializeImmediately ) : 
	IGLUTexture()
{
	m_filename      = strdup( filename );
	m_texImg        = new IGLUImage( filename );
	SetTextureParameters( flags );
	m_compressTex   = (flags & IGLU_COMPRESS_TEXTURE) ? true : false;

	m_width         = m_texImg->GetWidth();
	m_height        = m_texImg->GetHeight();
	m_pixelFormat   = m_texImg->GetGLFormat();

	if (initializeImmediately) Initialize();
}

IGLUTexture2D::IGLUTexture2D( unsigned char *image, int width, int height, bool freeMemory,
							 unsigned int flags, bool initializeImmediately ) :
	IGLUTexture()
{
	m_filename      = strdup( "(Image from memory!)" );
	m_texImg        = new IGLUImage( image, width, height, false, freeMemory );
	SetTextureParameters( flags );
	m_compressTex   = true;

	m_width         = m_texImg->GetWidth();
	m_height        = m_texImg->GetHeight();
	m_pixelFormat   = m_texImg->GetGLFormat();

	if (initializeImmediately) Initialize();
}

IGLUTexture2D::IGLUTexture2D( int builtInTexID, unsigned int flags, bool initializeImmediately )
{
	if (builtInTexID < 0 || builtInTexID >= IGLU_BUILTIN_TEX_MAX)
		ErrorExit("Non-existant built-in IGLU Texture!\n", __FILE__, __FUNCTION__, __LINE__);

	m_filename      = strdup( "(Built-in IGLU texture!)" );
	m_texImg        = new IGLUImage( g_igluBuiltInTex[builtInTexID].m_pixelData, 
		                             g_igluBuiltInTex[builtInTexID].m_width, 
									 g_igluBuiltInTex[builtInTexID].m_height,
									 (g_igluBuiltInTex[builtInTexID].m_bytesPerPixel > 3) );  // has alpha?
	SetTextureParameters( flags );
	m_compressTex   = (flags & IGLU_COMPRESS_TEXTURE) ? true : false;

	m_width         = m_texImg->GetWidth();
	m_height        = m_texImg->GetHeight();
	m_pixelFormat   = m_texImg->GetGLFormat();

	if (initializeImmediately) Initialize();
}


IGLUTexture2D::~IGLUTexture2D()
{
	if (m_filename) free( m_filename );
}

void IGLUTexture2D::SetTextureParameters( unsigned int flags )
{
	m_minFilter     = GetMinification( flags );
	m_magFilter     = GetMagnification( flags );
	m_sWrap         = GetSWrap( flags );
	m_tWrap         = GetTWrap( flags );
	m_mipmapsNeeded = UsingMipmaps( flags );

	if (m_initialized)
	{
		glBindTexture( GL_TEXTURE_2D, m_texID );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_minFilter );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_magFilter );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_sWrap );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_tWrap );
		glBindTexture( GL_TEXTURE_2D, 0 );
	}
}

void IGLUTexture2D::Initialize( void )
{
	// Make sure to avoid doubly-calling initialize
	if (m_initialized) return;

	// Double check that we *can* initialize.
	if( !glGenerateMipmap )     // non-NULL only after glewInit()
	{
		printf("*** ERROR!  IGLUTexture2D() constructor called with initializeImmediately flag\n");
		printf("            enabled, but OpenGL context has not yet been created!  Textures\n");
		printf("            created in this manner are invalid.  Set this flag to false, and\n");
		printf("            call IGLUTexture2D::Initialize() manually after an OpenGL context\n");
		printf("            is available!\n");
		exit(-1);  
	}

	// OK, now we can set up our texture
	glBindTexture( GL_TEXTURE_2D, m_texID );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_minFilter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_magFilter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_sWrap );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_tWrap );

	glTexImage2D( GL_TEXTURE_2D, 0, GetCompressedFormat( m_texImg->GetGLFormat() ),  
						   m_texImg->GetWidth(), m_texImg->GetHeight(), 0,
						   m_texImg->GetGLFormat(), m_texImg->GetGLDatatype(), 
						   (void *)m_texImg->ImageData() );
	GLenum error = glGetError(	);

	if (m_mipmapsNeeded)
		glGenerateMipmap( GL_TEXTURE_2D );

	glBindTexture( GL_TEXTURE_2D, 0 );

	// Now that we've copied the data into texture memory, free the CPU side copy.
	delete m_texImg;

	// Done initializing
	m_initialized = true;
}

