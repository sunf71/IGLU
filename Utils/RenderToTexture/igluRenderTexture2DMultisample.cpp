/******************************************************************/
/* igluRenderTexture2dMultisample.cpp                             */
/* ----------------------------------                             */
/*                                                                */
/* The file defines a class derived from IGLURenderTexture that   */
/*     is a renderable 2D multisample texture, which can also be  */
/*     used as an input for a GLSL multisample sampler.           */
/*                                                                */
/* Chris Wyman (01/06/2012)                                       */
/******************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "igluRenderTexture2DMultisample.h"

using namespace iglu;


IGLURenderTexture2DMultisample::IGLURenderTexture2DMultisample( int width, int height, int numSamples, GLenum format, 
										                        bool fixedSamples, unsigned int flags, 
																bool initializeImmediately ) : 
	IGLURenderTexture( format ), m_numSamples( numSamples )
{
	// Did the user want fixed multisample locations across all texels?
	m_fixedSampLocations = fixedSamples ? GL_TRUE : GL_FALSE;

	// Get some standard filtering & wrapping parameters
	m_minFilter     = GetMinification( flags );
	m_magFilter     = GetMagnification( flags );
	m_sWrap         = GetSWrap( flags );
	m_tWrap         = GetTWrap( flags );
	m_mipmapsNeeded = false;
	m_width         = width;
	m_height        = height;

	// Make sure the silly used didn't ask for mipmapping.
	if ( UsingMipmaps( flags ) )
	{
		printf("*** ERROR!  Constructor for IGLURenderTexture2DMultisample called\n");
		printf("            with flag asking for a mipmapped image.  Multisample\n");
		printf("            textures with mipmapping is not currently allowed!\n");
		exit(-1);
	}

	// See discussion in IGLURenderTexture2D for the purpose of the checks below
	if (initializeImmediately)
	{
		if( !glTexImage2DMultisample )    
		{
			printf("*** ERROR!  IGLURenderTexture2DMultisample() constructor called with \n");
			printf("            initializeImmediately flag enabled, but OpenGL context has not yet\n");
			printf("            been created! Textures created in this manner are invalid. Set this\n");
			printf("            flag false, and call IGLURenderTexture2DMultisample::Initialize()\n");
			printf("            manually after an OpenGL context is available!\n");
			exit(-1);   
		}
		// Ok, GL is setup.  We can initialize our texture!
		Initialize();
	}
}


IGLURenderTexture2DMultisample::~IGLURenderTexture2DMultisample()
{
}

void IGLURenderTexture2DMultisample::Initialize( void )
{
	// Make sure to avoid doubly-calling initialize
	if (m_initialized) return;

	// OK, now we can set up our texture
	glBindTexture( GL_TEXTURE_2D_MULTISAMPLE, m_texID );
	glTexParameteri( GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, m_minFilter );
	glTexParameteri( GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, m_magFilter );
	glTexParameteri( GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, m_sWrap );
	glTexParameteri( GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, m_tWrap );

	glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, 
		                     m_numSamples, m_format,
							 m_width, m_height, m_fixedSampLocations );             

	glBindTexture( GL_TEXTURE_2D_MULTISAMPLE, 0 );

	// Done initializing
	m_initialized = true;
}
