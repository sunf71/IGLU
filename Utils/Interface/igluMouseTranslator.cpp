/************************************************************************/
/* igluMouseTranslator.cpp                                              */
/* ---------------------                                                */
/*                                                                      */
/* This is a simple class that implements an translator based on mouse  */
/*    interactions (similar to the IGLUTrackball class).                */
/*                                                                      */
/* The key parameters are:
/*                                                                      */
/* Chris Wyman (1/24/2012)                                              */
/************************************************************************/


#include "iglu.h"

using namespace iglu;


IGLUMouseTranslator::IGLUMouseTranslator( int width, int height, float speed, 
		                                  const vec3& xVec, 
										  const vec3& yVec ) :
	IGLUMouseInteractor(width,height), m_xVec(xVec), m_yVec(yVec), m_speed(speed)
{
	Reset();
}


void IGLUMouseTranslator::SetOnClick( int x, int y )
{
	m_lastPos[0] = x;
	m_lastPos[1] = y;
	m_currentlyTracking = 1;
}

void IGLUMouseTranslator::UpdateOnMotion( int x, int y )
{
	float dx, dy;
	if (!m_currentlyTracking) return;

	dx = x - m_lastPos[0];
	dy = y - m_lastPos[1];
	
	if ( abs(dx) > 0 || abs(dy) > 0 )
	{
		m_lastPos[0] = x;
		m_lastPos[1] = y;

		vec3 xForm = ((m_speed*dx)/m_width)  * m_xVec +
			         ((-m_speed*dy)/m_height) * m_yVec;

		m_matrix = IGLUMatrix4x4::Translate( xForm ) * m_matrix;
		m_inverseMatrix = m_matrix.Invert();
		m_hasChanged = 1;
	}

}

void IGLUMouseTranslator::Release( void )                    
{
	m_currentlyTracking = 0; 
}


void IGLUMouseTranslator::Reset( void )
{
	m_currentlyTracking = 0;
	m_lastPos[0] = m_lastPos[1] = 0; 
	m_matrix = IGLUMatrix4x4::Identity();
	m_inverseMatrix = IGLUMatrix4x4::Identity();
	m_hasChanged = 1;
}



