/******************************************************************/
/* igluRenderTexture2D.cpp                                        */
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
#include <GL/glut.h>
#include "igluRenderTexture2D.h"

using namespace iglu;


IGLURenderTexture2D::IGLURenderTexture2D( int width, int height, GLenum format, 
										  unsigned int flags, bool initializeImmediately ) : 
	IGLURenderTexture( format )
{
	SetTextureParameters( flags );
	m_width         = width;
	m_height        = height;

	// See discussion in IGLURenderTexture2D for the purpose of the checks below
	if (initializeImmediately)
	{
		if( !glGenerateMipmap )    
		{
			printf("*** ERROR!  IGLURenderTexture2D() constructor called with initializeImmediately\n");
			printf("            flag enabled, but OpenGL context has not yet been created! Textures\n");
			printf("            created in this manner are invalid.  Set this flag to false, and\n");
			printf("            call IGLURenderTexture2D::Initialize() manually after an OpenGL\n");
			printf("            context is available!\n");
			exit(-1);   
		}
		// Ok, GL is setup.  We can initialize our texture!
		Initialize();
	}
}


IGLURenderTexture2D::~IGLURenderTexture2D()
{
}

void IGLURenderTexture2D::SetTextureParameters( unsigned int flags )
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

void IGLURenderTexture2D::Initialize( void )
{
	// Make sure to avoid doubly-calling initialize
	if (m_initialized) return;

	// OK, now we can set up our texture
	glBindTexture( GL_TEXTURE_2D, m_texID );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_minFilter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_magFilter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_sWrap );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_tWrap );

	glTexImage2D( GL_TEXTURE_2D, 0, m_format,  
						   m_width, m_height, 0,
						   GetBaseInternalFormat( m_format ), 
						   m_dataType,  
						   0 );    // No data yet!  We'll be rendering to it.                   
	if (m_mipmapsNeeded)                                
		glGenerateMipmap( GL_TEXTURE_2D );

	glBindTexture( GL_TEXTURE_2D, 0 );

	// Done initializing
	m_initialized = true;
}

void IGLURenderTexture2D::GenerateMipmaps( bool leaveBound )
{
	if (m_mipmapsNeeded)
	{
		glBindTexture( GL_TEXTURE_2D, m_texID );
		glGenerateMipmap( GL_TEXTURE_2D );
		if (!leaveBound) glBindTexture( GL_TEXTURE_2D, 0 );
	}
}

void IGLURenderTexture2D::Resize(int width, int height)
{
	//Set new values of height and width 
	m_width         = width;
	m_height        = height;
	
	//If already initialized, then initialize with new size.
	//	Otherwise wait for the user to call Initialize()
	if(m_initialized){
		m_initialized = false;
		Initialize();
	}
	
}