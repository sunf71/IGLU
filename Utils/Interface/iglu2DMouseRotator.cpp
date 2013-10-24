/************************************************************************/
/* iglu2DMouseRotator.cpp                                               */
/* ---------------------                                                */
/*                                                                      */
/* This is a simple class that gives a rotation matrix around the       */
/*    z-axis, with the amount depending on the amount of rotation the   */
/*    mouse moves around a specified point on screen.                   */
/*                                                                      */
/* Inputs are:                                                          */
/*    * Integers width & height.  Often these define the window width & */
/*      height, they can also be thought of as the number of pixels you */
/*      need move the mouse to translate by xVec and yVec.              */
/*    * Integers x & y, specifying the point on screen to rotate around */
/*      these should be specified in WINDOW coordinates.                */
/*    * float speed, is a multipler controlling how fast mouse rotation */
/*      maps to world-space rotation.                                   */
/*                                                                      */
/* Chris Wyman (1/24/2012)                                              */
/************************************************************************/


#include "iglu.h"

using namespace iglu;


IGLU2DMouseRotator::IGLU2DMouseRotator( int width, int height, 
		                                int xRotCenter, int yRotCenter, float speed ) :
	IGLUMouseInteractor(width,height), m_rotCtr( xRotCenter, yRotCenter, 0.0 ), m_speed(speed)
{
	Reset();
}


void IGLU2DMouseRotator::ResetRotationCenter( int xPos, int yPos )
{
	m_rotCtr = vec3( xPos, yPos, 0.0 );
}

void IGLU2DMouseRotator::SetOnClick( int x, int y )
{
	m_lastPos[0] = x;
	m_lastPos[1] = y;
	m_currentlyTracking = 1;
}

void IGLU2DMouseRotator::UpdateOnMotion( int x, int y )
{
	float dx, dy;
	if (!m_currentlyTracking) return;

	dx = x - m_lastPos[0];
	dy = y - m_lastPos[1];
	
	if ( abs(dx) > 0 || abs(dy) > 0 )
	{
		// Find normalized screen-space vectors to clicks from rotation point
		vec3 toLast = (vec3( m_lastPos[0], m_lastPos[1], 0) - m_rotCtr).vNormalize();
		vec3 toCur  = (vec3( x, y, 0 ) - m_rotCtr).vNormalize();

		// Compute angle moved.
		float dotPrd = iglu::clamp( toLast.Dot( toCur ), -0.9999f, 0.9999f );
		float angle  = m_speed * acos( dotPrd ) * 180.0 / IGLU_PI;
		angle       *= (toLast.Cross(toCur).Z() > 0) ? -1.0 : 1.0;
		m_curAngle  += angle;

		// Store wherw we are now
		m_lastPos[0] = x;
		m_lastPos[1] = y;

		// Compute our current rotation matrix.
		m_matrix = IGLUMatrix4x4::Rotate( m_curAngle, vec3(0,0,1) );
		m_inverseMatrix = m_matrix.Invert();
		m_hasChanged = 1;
	}

}

void IGLU2DMouseRotator::Release( void )                    
{
	m_currentlyTracking = 0; 
}


void IGLU2DMouseRotator::Reset( void )
{
	m_currentlyTracking = 0;
	m_lastPos[0] = m_lastPos[1] = 0; 
	m_matrix = IGLUMatrix4x4::Identity();
	m_inverseMatrix = IGLUMatrix4x4::Identity();
	m_curAngle = 0.0f;
	m_hasChanged = 1;
}



