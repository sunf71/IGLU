/******************************************************************/
/* igluTextureLightprobeCubemap.cpp                               */
/* -----------------------                                        */
/*                                                                */
/* The file defines an image class that loads a 2D image that     */
/*     contains a lightprobe stored as a cubemap in an unfolded   */
/*     box (i.e., cross) image format.  This loads the file and   */
/*     constructs an OpenGL cubemap texture that can be used.     */
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


IGLUTextureLightprobeCubemap::IGLUTextureLightprobeCubemap( char *filename, unsigned int flags, bool initializeImmediately ) : 
	IGLUTexture()
{
	m_filename      = strdup( filename );
	m_texImg        = new IGLUImage( filename );
	SetTextureParameters( flags );
	m_compressTex   = (flags & IGLU_COMPRESS_TEXTURE) ? true : false;

	m_width         = m_texImg->GetWidth() / 3;
	m_height        = m_texImg->GetHeight() / 4;
	m_pixelFormat   = m_texImg->GetGLFormat();

	if ( (3*m_width != m_texImg->GetWidth()) || 
		 (4*m_height != m_texImg->GetHeight()) )
	{
		printf("Warning!  Lightprobe from IGLUTextureLightprobeCubemap() does not appear to\n");
		printf("          be sized appropriately?  Is '%s' actually a lightprobe?\n", filename);
	}

	if (initializeImmediately) Initialize();
}


IGLUTextureLightprobeCubemap::~IGLUTextureLightprobeCubemap()
{
	if (m_filename) free( m_filename );
}

void IGLUTextureLightprobeCubemap::SetTextureParameters( unsigned int flags )
{
	m_minFilter     = GetMinification( flags );
	m_magFilter     = GetMagnification( flags );
	m_mipmapsNeeded = UsingMipmaps( flags );

	if (m_initialized)
	{
		glBindTexture( GL_TEXTURE_CUBE_MAP, m_texID );
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, m_minFilter );
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, m_magFilter );
		glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
	}
}

void IGLUTextureLightprobeCubemap::Initialize( void )
{
	// Make sure to avoid doubly-calling initialize
	if (m_initialized) return;

	// Double check that we *can* initialize.
	if( !glGenerateMipmap )     // non-NULL only after glewInit()
	{
		printf("*** ERROR! IGLUTextureLightprobeCubemap() constructor called with\n");
		printf("           initializeImmediately flag enabled, but OpenGL context has not yet\n");
		printf("           been created!  Textures created in this manner are invalid.  Set\n");
		printf("           this flag to false, and call IGLUTextureLightprobeCubemap::Initialize()\n");
		printf("           manually after an OpenGL context is available!\n");
		exit(-1);  
	}

	// OK pull the faces out of the image
	const unsigned char *arr = m_texImg->ImageData();
	int imgW = m_texImg->GetWidth();
	int imgH = m_texImg->GetHeight();

	// data for the different faces
	unsigned char *posX, *negX, *posY, *negY, *posZ, *negZ;
	posX = (unsigned char *)malloc( 3 * m_width * m_height * sizeof( unsigned char ) );
	negX = (unsigned char *)malloc( 3 * m_width * m_height * sizeof( unsigned char ) );
	posY = (unsigned char *)malloc( 3 * m_width * m_height * sizeof( unsigned char ) );
	negY = (unsigned char *)malloc( 3 * m_width * m_height * sizeof( unsigned char ) );
	posZ = (unsigned char *)malloc( 3 * m_width * m_height * sizeof( unsigned char ) );
	negZ = (unsigned char *)malloc( 3 * m_width * m_height * sizeof( unsigned char ) );

	/* transfer cube faces to separate arrays */
	int count = 0; 
	for (int i=m_width;i<2*m_width;i++)
		for (int j=0;j<m_height;j++)
		{
			posY[count++] = arr[3* (j*imgW + i) + 0];
			posY[count++] = arr[3* (j*imgW + i) + 1];
			posY[count++] = arr[3* (j*imgW + i) + 2];
		}

	count = 0;
	for (int j=m_height;j<2*m_height;j++)
		for (int i=m_width-1;i>=0;i--)
		{
			negZ[count++] = arr[3* (j*imgW + i) + 0];
			negZ[count++] = arr[3* (j*imgW + i) + 1];
			negZ[count++] = arr[3* (j*imgW + i) + 2];
		}

	count = 0;
	for (int j=m_height;j<2*m_height;j++)
		for (int i=2*m_width-1;i>=m_width;i--)
		{
			posX[count++] = arr[3* (j*imgW + i) + 0];
			posX[count++] = arr[3* (j*imgW + i) + 1];
			posX[count++] = arr[3* (j*imgW + i) + 2];		
		}

	count = 0;
	for (int j=m_height;j<2*m_height;j++)
		for (int i=3*m_width-1;i>=2*m_width;i--)
		{
			posZ[count++] = arr[3* (j*imgW + i) + 0];
			posZ[count++] = arr[3* (j*imgW + i) + 1];
			posZ[count++] = arr[3* (j*imgW + i) + 2];
		}

	count = 0;
	for (int i=2*m_width-1;i>=m_width;i--)
		for (int j=3*m_height-1;j>=2*m_height;j--)
		{
			negY[count++] = arr[3* (j*imgW + i) + 0];
			negY[count++] = arr[3* (j*imgW + i) + 1];
			negY[count++] = arr[3* (j*imgW + i) + 2];
		}

	count = 0;
	for (int j=4*m_height-1;j>=3*m_height;j--)
		for (int i=m_width;i<2*m_width;i++)
		{
			negX[count++] = arr[3* (j*imgW + i) + 0];
			negX[count++] = arr[3* (j*imgW + i) + 1];
			negX[count++] = arr[3* (j*imgW + i) + 2];
		}

	// OK, now we can set up our texture
	glBindTexture( GL_TEXTURE_CUBE_MAP, m_texID );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, m_minFilter );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, m_magFilter );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

	GLenum iFmt = GetCompressedFormat( m_texImg->GetGLFormat() );
	GLenum dFmt = m_texImg->GetGLFormat();
	GLenum dType = m_texImg->GetGLDatatype(); 
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, iFmt, m_width, m_height, 0, dFmt, dType, posX );
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, iFmt, m_width, m_height, 0, dFmt, dType, posY );
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, iFmt, m_width, m_height, 0, dFmt, dType, posZ );
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, iFmt, m_width, m_height, 0, dFmt, dType, negX );
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, iFmt, m_width, m_height, 0, dFmt, dType, negY );
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, iFmt, m_width, m_height, 0, dFmt, dType, negZ );

	if (m_mipmapsNeeded)
		glGenerateMipmap( GL_TEXTURE_CUBE_MAP );

	glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );

	// Now that we've copied the data into texture memory, free the CPU side copy.
	free( posX );
	free( negX );
	free( posY ); 
	free( negY );
	free( posZ );
	free( negZ );
	delete m_texImg;

	// Done initializing
	m_initialized = true;
}

