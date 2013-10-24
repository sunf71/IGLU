/******************************************************************/
/* igluRenderTexture2DArray.cpp                                   */
/* -----------------------                                        */
/*                                                                */
/* The file defines an image class that stores a 2D array texture.*/
/*                                                                */
/* Chris Wyman (01/30/2012)                                       */
/******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "iglu.h"

using namespace iglu;


IGLURenderTexture2DArray::IGLURenderTexture2DArray( int width, int height, int layers,
												    GLenum format, unsigned int flags, bool initializeImmediately ) : 
	IGLURenderTexture( format )
{
	SetTextureParameters( flags );
	m_width         = width;
	m_height        = height;
	m_depth         = layers;

	if (m_depth <= 0)
	{
		printf("Error!  IGLURenderTexture2DArray() constructed with invalid # layers!\n");
		assert( m_depth > 0 );
		exit(-1);
	}

	// See discussion in IGLURenderTexture2D for the purpose of the checks below
	if (initializeImmediately)
	{
		if( !glGenerateMipmap )    
		{
			printf("*** ERROR!  IGLURenderTexture2DArray() constructor called with \n");
			printf("            initializeImmediately flag enabled, but OpenGL context has not yet\n");
			printf("            been created! Textures created in this manner are invalid.  Set\n");
			printf("            this flag to false, and call IGLURenderTexture2D::Initialize()\n");
			printf("            manually after an OpenGL context is available!\n");
			exit(-1);   
		}
		// Ok, GL is setup.  We can initialize our texture!
		Initialize();
	}
}


IGLURenderTexture2DArray::~IGLURenderTexture2DArray()
{
}

void IGLURenderTexture2DArray::SetTextureParameters( unsigned int flags )
{
	m_minFilter     = GetMinification( flags );
	m_magFilter     = GetMagnification( flags );
	m_sWrap         = GetSWrap( flags );
	m_tWrap         = GetTWrap( flags );
	m_mipmapsNeeded = UsingMipmaps( flags );

	if (m_initialized)
	{
		glBindTexture( GL_TEXTURE_2D_ARRAY, m_texID );
		glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, m_minFilter );
		glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, m_magFilter );
		glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, m_sWrap );
		glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, m_tWrap );
		glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );
	}
}

void IGLURenderTexture2DArray::Initialize( void )
{
	// Make sure to avoid doubly-calling initialize
	if (m_initialized) return;

	// OK, now we can set up our texture
	glBindTexture( GL_TEXTURE_2D_ARRAY, m_texID );
	glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, m_minFilter );
	glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, m_magFilter );
	glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, m_sWrap );
	glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, m_tWrap );

	glTexImage3D( GL_TEXTURE_2D_ARRAY, 0, m_format,  
						   m_width, m_height, m_depth, 0,
						   GetBaseInternalFormat( m_format ), 
						   m_dataType,  
						   0 );    // No data yet!  We'll be rendering to it.                   
	if (m_mipmapsNeeded)                                
		glGenerateMipmap( GL_TEXTURE_2D_ARRAY );

	glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );

	// Done initializing
	m_initialized = true;
}

void IGLURenderTexture2DArray::GenerateMipmaps( bool leaveBound )
{
	if (m_mipmapsNeeded)
	{
		glBindTexture( GL_TEXTURE_2D_ARRAY, m_texID );
		glGenerateMipmap( GL_TEXTURE_2D_ARRAY );
		if (!leaveBound) glBindTexture( GL_TEXTURE_2D_ARRAY, 0 );
	}
}
