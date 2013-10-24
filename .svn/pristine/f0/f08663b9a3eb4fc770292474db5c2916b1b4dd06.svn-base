/******************************************************************/
/* igluRandomTexture2D.cpp                                        */
/* -----------------------                                        */
/*                                                                */
/* The file defines an image class for passing a 2D array of      */
/*    random unsigned intergers down to a GLSL program.  This     */
/*    is based of the internal IGLU random number generator to    */
/*    create random seeds.  The texture is of type GL_R32UI.      */
/*                                                                */
/* Note: When using the random seeds on the GPU, ensure you use   */
/*    a random number generator that uses *different* adder and   */
/*    multipliers than the built-in IGLU generator, otherwise you */
/*    ray get strange correlation artifacts between adjacent      */
/*    pixels!                                                     */
/*                                                                */
/* Chris Wyman (12/07/2011)                                       */
/******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "iglu.h"

using namespace iglu;


IGLURandomTexture2D::IGLURandomTexture2D( unsigned int width, unsigned int height, GLenum textureType,
										  unsigned int randomSeed, unsigned int flags, bool initializeImmediately ):
	IGLUTexture(), m_rndSeed(randomSeed), m_randType(textureType)
{
	m_minFilter     = GL_NEAREST; 
	m_magFilter     = GL_NEAREST; 
	m_sWrap         = GetSWrap( flags );
	m_tWrap         = GetTWrap( flags );
	m_mipmapsNeeded = false; 
	m_compressTex   = false;

	m_width         = width;
	m_height        = height;

	m_rng           = new IGLURandom( m_rndSeed );

	if ( (m_randType != GL_FLOAT) && (m_randType != GL_INT) )
	{
		printf("*** ERROR!  IGLURandomTexture2D() does not specify GL_INT or GL_FLOAT!\n");
		exit(-1);
	}

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
			printf("*** ERROR!  IGLURandomTexture2D() constructor called with initializeImmediately\n");
			printf("            flag enabled, but no OpenGL context yet exists!  Textures created\n");
			printf("            in this manner are invalid.  Set this flag to false, and call\n");
			printf("            IGLURandomTexture2D::Initialize() manually after an OpenGL context\n");
			printf("            is available!\n");
			exit(-1);  // This may also occur if the supported OpenGL version is < 3.0, when 
			           //     glGenerateMipmap was added to the core OpenGL spec.
		}

		// Ok, GL is setup.  We can initialize our texture!
		Initialize();
	}
}

IGLURandomTexture2D::~IGLURandomTexture2D()
{
	if (m_rng) delete m_rng;
}

//#define TEST_USE_FLOAT 1

void IGLURandomTexture2D::Initialize( void )
{
	// Make sure to avoid doubly-calling initialize
	if (m_initialized) return;

	// OK, now we can set up our texture
	glBindTexture( GL_TEXTURE_2D, m_texID );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_minFilter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_magFilter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_sWrap );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_tWrap );

	if ( m_randType == GL_FLOAT )
	{
		float *rands = new float[ m_width * m_height ];
		for (int i=0; i<m_width*m_height; i++)
			rands[i] = (float)m_rng->fRandom();
		glTexImage2D( GL_TEXTURE_2D, 0, GL_R32F,  
					  m_width, m_height, 0, GL_RED, GL_FLOAT, 
					  (void *)rands );
		delete rands;
	}
	else // if ( m_randType == GL_INT )
	{
		uint *rands = new uint[ 4 * m_width * m_height ];
		for (int i=0; i<4*m_width*m_height; i++)
			rands[i] = (uint)(65535 * m_rng->fRandom());
		glTexImage2D( GL_TEXTURE_2D, 0, GL_R32I,  
					  m_width, m_height, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, 
					  (void *)rands );
		delete rands;
	}

	glBindTexture( GL_TEXTURE_2D, 0 );

	// Done initializing
	m_initialized = true;
}


void IGLURandomTexture2D::Resize(int width, int height){
	
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
