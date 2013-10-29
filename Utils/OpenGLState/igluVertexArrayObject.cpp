/******************************************************************/
/* igluVertexArrayObject.h                                        */
/* -----------------------                                        */
/*                                                                */
/* A class for creating and using OpenGL Vertex Array Objects.    */
/*                                                                */
/* Chris Wyman (06/13/2011)                                       */
/******************************************************************/

#include "iglu.h"

using namespace iglu;

IGLUVertexArray::IGLUVertexArray() :
	m_vertArray(0), m_elemArray(0), m_enableAttribCalled(false), m_elementArrayBound(false),
	m_vertArrayInit(false), m_elemType(GL_UNSIGNED_INT), m_isBound(false), m_withPrimitiveRestart(false)
{
	glGenVertexArrays( 1, &m_arrayID );
	m_vertArray = new IGLUBuffer( IGLU_ARRAY );
}

IGLUVertexArray::~IGLUVertexArray()
{
	// Make sure we're not deleting a bound array!
	if (m_isBound) Unbind();

	glDeleteVertexArrays( 1, &m_arrayID );
	if (m_vertArray) delete m_vertArray;
	if (m_elemArray) delete m_elemArray;
}


void IGLUVertexArray::Bind( void )
{
	if (!m_isBound)
	{
		glBindVertexArray( m_arrayID );
		m_isBound=true;
		if (m_withPrimitiveRestart) 
		{
			glEnable( GL_PRIMITIVE_RESTART );
			glPrimitiveRestartIndex( m_restartIndex );
		}
	}
}

void IGLUVertexArray::Unbind( void )
{
	if (m_isBound)
	{
		if (m_withPrimitiveRestart) 
		{
			glDisable( GL_PRIMITIVE_RESTART );
			glPrimitiveRestartIndex( 0 );
		}
		glBindVertexArray( 0 );
		m_isBound=false;
	}
}

void IGLUVertexArray::SetVertexArray       ( GLsizeiptr bufSize, void *bufData, int use )
{
	m_vertArrayInit = true;
	m_vertArray->SetBufferData( bufSize, bufData, use );
}

void IGLUVertexArray::SetVertexArraySubset ( GLintptr   bufOffset, GLsizeiptr subSize, void *subData )
{
	// You should cannot set a subset prior to setting up the whole array...  Call SetVertexArray() first.
	assert( m_vertArrayInit );
	m_vertArray->SetBufferSubData( bufOffset, subSize, subData );
}

void IGLUVertexArray::SetElementArray      ( GLenum elemType, GLsizeiptr bufSize, void *bufData, int use )
{
	m_elemType = elemType;
	if (!m_elemArray)
		m_elemArray = new IGLUBuffer( IGLU_ELEMENT_ARRAY );
	m_elemArray->SetBufferData( bufSize, bufData, use );
}

void IGLUVertexArray::SetElementArraySubset( GLintptr   bufOffset, GLsizeiptr subSize, void *subData )
{
	// You should cannot set a subset prior to setting up the whole array...  Call SetElementArray() first.
	assert( m_elemArray );
	m_elemArray->SetBufferSubData( bufOffset, subSize, subData );
}

void *IGLUVertexArray::MapVertexArray  ( AccessMode mode )
{
	return m_vertArray->Map( mode );
}

void *IGLUVertexArray::MapElementArray ( AccessMode mode )
{
	if (m_elemArray)
		return m_elemArray->Map( mode );
	return NULL;
}

void IGLUVertexArray::UnmapVertexArray ( void )
{
	m_vertArray->Unmap();
}

void IGLUVertexArray::UnmapElementArray( void )
{
	if (m_elemArray) m_elemArray->Unmap();
}

void IGLUVertexArray::EnableAttribute( const IGLUShaderVariable &attrib, 
									   GLint size, GLenum type, 
									   GLsizei stride, const GLvoid *pointer,
									   int instancesDivisor )
{
	m_enableAttribCalled = true;

	Bind();
	m_vertArray->Bind();
	glEnableVertexAttribArray( attrib.GetVariableIndex() );
	glVertexAttribPointer( attrib.GetVariableIndex(), size, type, false, stride, pointer );
	if (instancesDivisor > 0) glVertexAttribDivisor( attrib.GetVariableIndex(), instancesDivisor );
	Unbind();
}

void IGLUVertexArray::EnableAttribute( int glAttribIdx,                          
		                               GLint size, GLenum type,                  
						               GLsizei stride, const GLvoid *pointer,    
						               int instancesDivisor )
{
	m_enableAttribCalled = true;

	Bind();
	m_vertArray->Bind();
	glEnableVertexAttribArray( glAttribIdx );
	glVertexAttribPointer( glAttribIdx, size, type, false, stride, pointer );
	if (instancesDivisor > 0) glVertexAttribDivisor( glAttribIdx, instancesDivisor );
	Unbind();
}


void IGLUVertexArray::EnablePrimitiveRestart( unsigned int restartIndex )
{
	m_withPrimitiveRestart = true;
	m_restartIndex = restartIndex;
}

bool IGLUVertexArray::Internal_InitPrimRestart( void )
{	
	bool primRestartEnabled = (glIsEnabled( GL_PRIMITIVE_RESTART ) == GL_TRUE);
	if ( m_withPrimitiveRestart )
	{
		glPrimitiveRestartIndex( m_restartIndex );
		glEnable( GL_PRIMITIVE_RESTART );
	}
	return primRestartEnabled;
}

void IGLUVertexArray::Internal_StopPrimRestart( bool wasEnabled )
{
	if (wasEnabled)
		glEnable( GL_PRIMITIVE_RESTART );
	else
		glDisable( GL_PRIMITIVE_RESTART );
}


void IGLUVertexArray::DrawArrays( GLenum mode, GLint first, GLsizei count )
{
	bool primRestartEnabled = Internal_InitPrimRestart();

	Bind();
	glDrawArrays( mode, first, count );
	Unbind();

	Internal_StopPrimRestart( primRestartEnabled );
}

void IGLUVertexArray::MultiDrawArrays( GLenum mode, GLint *first, GLsizei *count, GLsizei primCount )
{
	bool primRestartEnabled = Internal_InitPrimRestart();

	Bind();
	glMultiDrawArrays( mode, first, count, primCount );
	Unbind();

	Internal_StopPrimRestart( primRestartEnabled );
}

void IGLUVertexArray::DrawArraysInstanced( GLenum mode, GLint first, GLsizei count, 
										   GLsizei primCount, GLuint baseInstance )
{
	bool primRestartEnabled = Internal_InitPrimRestart();

	Bind();
	glDrawArraysInstancedBaseInstance( mode, first, count, primCount, baseInstance );
	Unbind();

	Internal_StopPrimRestart( primRestartEnabled );
}

void IGLUVertexArray::DrawTransformFeedback( GLenum mode, const IGLUTransformFeedback::Ptr &feedback, 
											 GLuint feedbackStream, GLsizei instances )
{
	bool primRestartEnabled = Internal_InitPrimRestart();

	Bind();
	glDrawTransformFeedbackStreamInstanced( mode, feedback->GetTransformID(), feedbackStream, instances );
	Unbind();

	Internal_StopPrimRestart( primRestartEnabled );
}
void IGLUVertexArray::DrawElementsInstanced( GLenum mode, GLsizei count, GLsizei instanceNum, GLuint bufOffsetBytes)
{
		assert( m_elemArray );
	bool primRestartEnabled = Internal_InitPrimRestart();

	Bind();
	if (!m_elementArrayBound)
	{
		m_elemArray->Bind();
		m_elementArrayBound = true;
	}
	glDrawElementsInstanced( mode, count, m_elemType, BUFFER_OFFSET(bufOffsetBytes) ,instanceNum);
	Unbind();

	Internal_StopPrimRestart( primRestartEnabled );
}
void IGLUVertexArray::DrawElements( GLenum mode, GLsizei count, GLuint bufOffsetBytes )
{
	assert( m_elemArray );
	bool primRestartEnabled = Internal_InitPrimRestart();

	Bind();
	if (!m_elementArrayBound)
	{
		m_elemArray->Bind();
		m_elementArrayBound = true;
	}
	glDrawElements( mode, count, m_elemType, BUFFER_OFFSET(bufOffsetBytes) );
	Unbind();

	Internal_StopPrimRestart( primRestartEnabled );
}