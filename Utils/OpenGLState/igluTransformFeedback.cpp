/******************************************************************/
/* igluTransformFeedback.cpp                                      */
/* -----------------------                                        */
/*                                                                */
/* A class for creating and using OpenGL's transform feedback     */
/*     mechanism.  To output values to a GPU memory buffer after  */
/*     the vertex/geometry GLSL stages (and prior to rasterizing) */
/*                                                                */
/* Chris Wyman (06/13/2011)                                       */
/******************************************************************/

#include "iglu.h"

using namespace iglu;

IGLUTransformFeedback::IGLUTransformFeedback() :
	m_transformID(0), m_isBound(false), m_boundOnBegin(false), m_isActive(false), m_isPaused(false)
{
	glGenTransformFeedbacks( 1, &m_transformID );
}

IGLUTransformFeedback::~IGLUTransformFeedback()
{
	glDeleteTransformFeedbacks( 1, &m_transformID );
}


void IGLUTransformFeedback::Bind( void )
{
	// Make sure we're not already bound.
	if (m_isBound) return;

	// Go ahead and bind the transform feedback object.
	glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, m_transformID );
	m_isBound = true;        // We are now bound
	m_boundOnBegin = false;  // We did *not* bind inside of Begin()
}

void IGLUTransformFeedback::Unbind( void )
{
	// Don't unbind unless we're actually bound
	if (m_isBound) return;

	// Go ahead and unbind out transform feedback object.
	glBindTransformFeedback( GL_TRANSFORM_FEEDBACK, 0 );
	m_isBound = false;
}

int IGLUTransformFeedback::Begin( GLenum primitiveType )
{
	// Make sure we haven't already begun
	if (m_isActive) { assert(0); return IGLU_ERROR_INVALID_GLSTATE; }

	// If we haven't bound our transform feedback object yet, do it
	if (!m_isBound)
	{
		Bind();
		m_boundOnBegin = true;
	}

	// Start transform feedback
	glBeginTransformFeedback( primitiveType );  
	m_isActive = true;
	m_isPaused = false;

	return IGLU_NO_ERROR;
}

int IGLUTransformFeedback::End( void )
{
	// Can't End() if we didn't first Begin()!
	if (!m_isActive) { assert(0); return IGLU_ERROR_INVALID_GLSTATE; }

	// End transform feedback
	glEndTransformFeedback(); 
	m_isActive = false;
	m_isPaused = true;

	// If Begin() had to Bind() our object, unbind it here.
	if (m_boundOnBegin)
		Unbind();

	return IGLU_NO_ERROR;
}

int IGLUTransformFeedback::Pause( void )
{
	// Can't Pause() if we didn't yet Begin() or if we already Pause()'d
	if (!m_isActive || m_isPaused) { assert(0); return IGLU_ERROR_INVALID_GLSTATE; }

	// Actually pause
	glPauseTransformFeedback();
	m_isPaused = true;

	return IGLU_NO_ERROR;
}

int IGLUTransformFeedback::Resume( void )
{
	// Can't Resume() if we didn't Begin() and Pause() previously!
	if (!m_isActive || !m_isPaused) { assert(0); return IGLU_ERROR_INVALID_GLSTATE; }

	// Actually resume
	glResumeTransformFeedback();
	m_isPaused = false;

	return IGLU_NO_ERROR;
}


int IGLUTransformFeedback::AttachBuffer( const IGLUBuffer::Ptr &buf, GLuint attachIdx )
{
	// Can't do this if we already started feedback!
	if (m_isActive) { assert(0); return IGLU_ERROR_INVALID_GLSTATE; }

	// To attach a buffer, we need to be bound.
	bool wasBound = m_isBound;
	if (!m_isBound) Bind();

	glBindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, attachIdx, buf->GetBufferID() ); 

	// If we were bound before calling AttachBuffer(), leave us bound.  Otherwise Unbind();
	if (!wasBound) Unbind();

	return IGLU_NO_ERROR;
}


int IGLUTransformFeedback::AttachSubBuffer( const IGLUBuffer::Ptr &buf, 
											 GLintptr startOffsetBytes, GLsizeiptr sizeBytes, 
											 GLuint attachIdx )
{
	// Can't do this if we already started feedback!
	if (m_isActive) { assert(0); return IGLU_ERROR_INVALID_GLSTATE; }

	// To attach a buffer, we need to be bound.
	bool wasBound = m_isBound;
	if (!m_isBound) Bind();

	glBindBufferRange( GL_TRANSFORM_FEEDBACK_BUFFER, 
		               attachIdx, buf->GetBufferID(),
		               startOffsetBytes, sizeBytes ); 

	// If we were bound before calling AttachBuffer(), leave us bound.  Otherwise Unbind();
	if (!wasBound) Unbind();

	return IGLU_NO_ERROR;
}

