/******************************************************************/
/* igluTexture2D.h                                                */
/* -----------------------                                        */
/*                                                                */
/* The file defines an image class that stores a 2D image texture */
/*     in a format for easy display for OpenGL.                   */
/*                                                                */
/* Chris Wyman (10/26/2006)                                       */
/******************************************************************/

#ifndef IGLU_TEXTURE2D_H
#define IGLU_TEXTURE2D_H

#pragma warning( disable: 4996 )

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "igluTexture.h"
#include "igluImage.h"

namespace iglu {

enum {
	IGLU_BUILTIN_TEX_PLUSBUTTON = 0,
	IGLU_BUILTIN_TEX_MAX,
};

class IGLUTexture2D : public IGLUTexture
{
public:
    IGLUTexture2D( char *filename, unsigned int flags = IGLU_TEXTURE_DEFAULT, bool initializeImmediately=true );
	IGLUTexture2D( unsigned char *image, int width, int height, bool freeMemory=false,
		           unsigned int flags = IGLU_TEXTURE_DEFAULT, bool initializeImmediately=true );
	IGLUTexture2D( int builtInTexID, unsigned int flags = IGLU_TEXTURE_DEFAULT, bool initializeImmediately=true );
    virtual ~IGLUTexture2D();

	// Initialize() must be called after an OpenGL context has been created.
	virtual void Initialize( void );

	// Returns the OpenGL type of this particular texture (needs to be defined!)
	virtual GLenum GetTextureType( void ) const         { return GL_TEXTURE_2D; }

	// Query detailed information about this texture type
	virtual bool Is1DTexture( void ) const              { return false; }
	virtual bool Is2DTexture( void ) const              { return true; }
	virtual bool Is3DTexture( void ) const              { return false; }
	virtual bool IsBufferTexture( void ) const          { return false; }
	virtual bool IsRectTexture( void ) const            { return false; }
	virtual bool IsCubeTexture( void ) const            { return false; }
	virtual bool IsMultisampleTexture( void ) const     { return false; }
	virtual bool IsArrayTexture( void ) const           { return false; }

	// Set/update the texture parameters for this texture
	virtual void SetTextureParameters( unsigned int flags );

	// This reads from a texture image.  Types are pretty basic. 
	//   --> If you need more complex types, you might want to overload IGLUTexture yourself!
	virtual GLenum GetTextureFormat( void ) const       { return ( m_pixelFormat == GL_RGBA ? GL_RGBA8 : GL_RGB8 ); }

	// A pointer to a IGLUTexture2D could have type IGLUTexture2D::Ptr
	typedef IGLUTexture2D *Ptr;

protected:
	// Our image-loader class
	IGLUImage *m_texImg;

	// OpenGL texture settings
	GLint m_minFilter, m_magFilter;
	GLint m_sWrap, m_tWrap;

	// The pixel format.  For these images either GL_RGB or GL_RGBA
	GLenum m_pixelFormat;

	bool m_mipmapsNeeded;
};


// End namespace iglu
}


#endif
