/******************************************************************/
/* igluRenderTexture.h                                            */
/* -----------------------                                        */
/*                                                                */
/* The file defines an image class that stores a 2D image texture */
/*     in a format for easy display for OpenGL.                   */
/*                                                                */
/* Chris Wyman (10/12/2011)                                       */
/******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "iglu.h"

#include "igluRenderTexture.h"

using namespace iglu;

IGLURenderTexture::IGLURenderTexture( GLenum format ) : 
	m_format( format ), m_mipmapsNeeded( false ) 
{
	if (!IsFormatRenderable( format ))
		printf("*** Error:  Non-renderable format passed to IGLURenderTexture (%d)\n", format);

	m_dataType = GetFormatDataType( format );
}
	
