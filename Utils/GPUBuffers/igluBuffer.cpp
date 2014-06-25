/******************************************************************/
/* igluBuffer.cpp                                                 */
/* -----------------------                                        */
/*                                                                */
/* A class that creates and stores and OpenGL buffer object       */
/*                                                                */
/* Chris Wyman (06/13/2011)                                       */
/******************************************************************/

#include "iglu.h"

using namespace iglu;

IGLUBuffer::IGLUBuffer( BufferType type ) : 
	m_mapped(false), m_bufPtr(0), m_isBound(false)
{ 
	m_type = ConvertToGLType( type );
	glGenBuffers( 1, &m_bufID ); 
}

IGLUBuffer::~IGLUBuffer()
{
	// Make sure we're not deleting a bound buffer!
	if (m_isBound) Unbind();

	glDeleteBuffers( 1, &m_bufID );
}

void IGLUBuffer::Bind( void )
{
	if (!m_isBound)
		glBindBuffer( m_type, m_bufID );
	m_isBound = true;
}

void IGLUBuffer::Unbind( void )
{
	if (m_isBound)
		glBindBuffer( m_type, 0 );
	m_isBound = false;
}

void *IGLUBuffer::Map( AccessMode mode )
{
	// Are we already mapped?  If so, return the existing ptr
	if (m_bufPtr && m_mapped) 
		return m_bufPtr;

	// If not, figure out which GL access mode to request
	GLenum access = ConvertToGLAccessMode( mode );
	
	// Map the buffer
	Bind();
	m_bufPtr = glMapBuffer( m_type, access );
	Unbind();

	// Make sure that everything worked out correctly
	m_mapped = (m_bufPtr ? true : false);

	// Return our mapped pointer
	return m_bufPtr;
}

void IGLUBuffer::Unmap( void )
{
	// We don't need to unmap unless we're already mapped
	if (!m_mapped) return;

	//Unmapping will fail if the buffer is not currently bound
	Bind();
	
	//Unmap the buffer
	m_mapped = ( glUnmapBuffer( m_type ) == GL_TRUE ) ? false : true;	

	//Unbind the buffer
	Unbind();
}

void IGLUBuffer::SetBufferData( GLsizeiptr bufSize, void *bufData, int use )
{
	// Figure out the requested usage mode for this buffer
	int usage = ConvertToUsageMode( use );

	// Alright, bind the data to the buffer
	Bind();
	glBufferData( m_type, bufSize, bufData, usage );
	Unbind();
}

void IGLUBuffer::SetBufferSubData( GLintptr bufOffset, GLsizeiptr subSize, void *subData )
{
	// Alright, send the data to the buffer
	Bind();
	glBufferSubData( m_type, bufOffset, subSize, subData );
	Unbind();
}

GLenum IGLUBuffer::ConvertToGLAccessMode( AccessMode mode )
{
	switch( mode )
	{
	case IGLU_READ:
		return GL_READ_ONLY;
	case IGLU_WRITE:
		return GL_WRITE_ONLY;
	case IGLU_READ_WRITE:
		return GL_READ_WRITE;
	}

	// Should not happen.  Return a reasonable default
	return GL_READ_ONLY;
}

GLenum IGLUBuffer::ConvertToGLType      ( BufferType type )
{
	switch( type )
	{
	case IGLU_ARRAY:
		return GL_ARRAY_BUFFER;
	case IGLU_ELEMENT_ARRAY:
		return GL_ELEMENT_ARRAY_BUFFER;
	case IGLU_PIXEL_PACK:
		return GL_PIXEL_PACK_BUFFER;
	case IGLU_PIXEL_UNPACK:
		return GL_PIXEL_UNPACK_BUFFER;
	case IGLU_TEXTURE:
		return GL_TEXTURE_BUFFER;
	case IGLU_UNIFORM:
		return GL_UNIFORM_BUFFER;
	case IGLU_DRAW_INDIRECT:
		return GL_DRAW_INDIRECT_BUFFER;
	}

	// Should not happen.  Return a reasonable default.
	return GL_ARRAY_BUFFER;
}


GLenum IGLUBuffer::ConvertToUsageMode   ( int use )
{
	if ( use & (IGLU_STATIC|IGLU_DRAW) )
		return GL_STATIC_DRAW;
	if ( use & (IGLU_STATIC|IGLU_COPY) )
		return GL_STATIC_COPY;
	if ( use & (IGLU_STATIC|IGLU_READ) )
		return GL_STATIC_READ;
	if ( use & (IGLU_DYNAMIC|IGLU_DRAW) )
		return GL_DYNAMIC_DRAW;
	if ( use & (IGLU_DYNAMIC|IGLU_COPY) )
		return GL_DYNAMIC_COPY;
	if ( use & (IGLU_DYNAMIC|IGLU_READ) )
		return GL_DYNAMIC_READ;
	if ( use & (IGLU_STREAM|IGLU_DRAW) )
		return GL_STREAM_DRAW;
	if ( use & (IGLU_STREAM|IGLU_COPY) )
		return GL_STREAM_COPY;
	if ( use & (IGLU_STREAM|IGLU_READ) )
		return GL_STREAM_READ;

	// Should not happen.  Return a reasonable default.
	return GL_DYNAMIC_DRAW;
}

