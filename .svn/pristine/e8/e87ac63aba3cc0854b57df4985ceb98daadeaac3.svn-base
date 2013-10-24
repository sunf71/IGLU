/************************************************************************/
/* igluTrackball.cpp                                                    */
/* ------------------                                                   */
/*                                                                      */
/* This is a class that implements a simple virtual trackball for       */
/*    mouse-based 3D rotations.  The trackball is not a very good       */
/*    interface, but it is quick and easy to use and allows for simple  */
/*    addition of UI-based rotations.                                   */
/*                                                                      */
/* I have been using this trackball for over 10 years now, and this     */
/*    class is nearly identical to the Trackball.cpp class I used for   */
/*    most of those years.  This one is different in that it derives    */
/*    from a base mouse-interactor class to allow sharing of interfaces */
/*    between other simple mouse interactions.  The only difference     */
/*    in the class is that method names including 'Trackball' have been */
/*    renamed suitable for a more generic interface.                    */
/*                                                                      */
/* Inputs are:                                                          */
/*    * Integers width & height.  To rotate in a plausible, 'correct'   */
/*      manner, the trackball needs the window width & height.          */
/*                                                                      */
/* Chris Wyman (01/24/2012)                                             */
/************************************************************************/


#include "iglu.h"

using namespace iglu;


IGLUTrackball::IGLUTrackball( int width, int height ) : 
	IGLUMouseInteractor(width,height)
{
	Reset();
}


/* the internal code which computes the rotation */
void IGLUTrackball::trackball_ptov(int x, int y, int width, int height, float v[3])
{
    float d, a;

    /* project x,y onto a hemi-sphere centered within width, height */
    v[0] = (2.0F*x - width) / width;
    v[1] = (height - 2.0F*y) / height;
    d = (float) sqrt(v[0]*v[0] + v[1]*v[1]);
    v[2] = (float) cos((IGLU_PI/2.0F) * ((d < 1.0F) ? d : 1.0F));
    a = 1.0F / (float) sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    v[0] *= a;
    v[1] *= a;
    v[2] *= a;
}

void IGLUTrackball::SetOnClick( int x, int y )
{
	trackball_ptov( x, y, m_width, m_height, m_lastPos );
	m_currentlyTracking = 1;
}

void IGLUTrackball::UpdateOnMotion( int x, int y )
{
	float curPos[3], dx, dy, dz, angle, axis[3];
	if (!m_currentlyTracking) return;

	trackball_ptov( x, y, m_width, m_height, curPos );
    dx = curPos[0] - m_lastPos[0];
	dy = curPos[1] - m_lastPos[1];
	dz = curPos[2] - m_lastPos[2];
	if ( fabs(dx) > 0 || fabs(dy) > 0 || fabs(dz) > 0 )
	{
		angle           = 90.0 * sqrt( dx*dx + dy*dy + dz*dz );
		axis[0]         = m_lastPos[1]*curPos[2] - m_lastPos[2]*curPos[1];
		axis[1]         = m_lastPos[2]*curPos[0] - m_lastPos[0]*curPos[2];
		axis[2]         = m_lastPos[0]*curPos[1] - m_lastPos[1]*curPos[0];
		m_lastPos[0]    = curPos[0];
		m_lastPos[1]    = curPos[1];
		m_lastPos[2]    = curPos[2];
		m_matrix        = IGLUMatrix4x4::Rotate( angle, vec3( axis ) ) * m_matrix;
		m_inverseMatrix = m_matrix.Invert();
		m_hasChanged    = 1;
	}
}

void IGLUTrackball::Release( void )                    
{
	m_currentlyTracking = 0; 
}

void IGLUTrackball::Reset( void )
{
	m_currentlyTracking = 0;
	m_lastPos[0] = m_lastPos[1] = m_lastPos[2] = 0; 
	m_matrix = IGLUMatrix4x4::Identity();
	m_inverseMatrix = IGLUMatrix4x4::Identity();
	m_hasChanged = 1;
}


