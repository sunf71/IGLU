/******************************************************************/
/* igluUniformBuffer.cpp                                          */
/* -----------------------                                        */
/*                                                                */
/* A class that creates an OpenGL uniform buffer and provides     */
/*    some methods for accessing data inside the uniform buffer.  */
/*                                                                */
/* Chris Wyman (06/13/2011)                                       */
/******************************************************************/


#include "iglu.h"


using namespace iglu;

IGLUUniformBuffer::IGLUUniformBuffer( const char *bufName, GLuint glslProgramID ) : 
	IGLUBuffer( IGLU_UNIFORM ), m_bufProgID(0), m_blkIdx(-1)
{ 
	// Check:  Did the user pass in the GLSL program to look at to create this buffer?
	if (!glslProgramID)
	{
		// Nope.  Assume the 

	}
}

IGLUUniformBuffer::~IGLUUniformBuffer()
{
}

void IGLUUniformBuffer::Bind( void )
{
	glBindBuffer( m_type, m_bufID );
}



void IGLUUniformBuffer::SetBufferData( GLsizeiptr bufSize, void *bufData, int use )
{

}

void IGLUUniformBuffer::SetBufferSubData( GLintptr bufOffset, GLsizeiptr subSize, void *subData )
{

}

