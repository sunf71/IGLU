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



void IGLUMouseInteractor::ResizeInteractorWindow( int width, int height )
{
	m_width = width;
	m_height = height;
}

IGLUMouseInteractor::IGLUMouseInteractor( int width, int height) :
	m_width(width), m_height(height), m_currentlyTracking(0), m_hasChanged(1)
{
}

void IGLUMouseInteractor::Reset( void )
{
	m_currentlyTracking = 0;
	m_matrix = IGLUMatrix4x4::Identity();
	m_inverseMatrix = IGLUMatrix4x4::Identity();
	m_hasChanged = 1;
}

void IGLUMouseInteractor::ApplyMatrix( float inVec[4], float result[4] )
{
	result[0] = m_matrix[0]*inVec[0] + m_matrix[4]*inVec[1] + m_matrix[8]*inVec[2] + m_matrix[12]*inVec[3];
	result[1] = m_matrix[1]*inVec[0] + m_matrix[5]*inVec[1] + m_matrix[9]*inVec[2] + m_matrix[13]*inVec[3];
	result[2] = m_matrix[2]*inVec[0] + m_matrix[6]*inVec[1] + m_matrix[10]*inVec[2] + m_matrix[14]*inVec[3];
	result[3] = m_matrix[3]*inVec[0] + m_matrix[7]*inVec[1] + m_matrix[11]*inVec[2] + m_matrix[15]*inVec[3];
}

vec4 IGLUMouseInteractor::ApplyMatrix( const vec4 &vec )
{
	return m_matrix * vec;
}

void IGLUMouseInteractor::SetMatrix( GLfloat *newMat )
{
	m_matrix = IGLUMatrix4x4( newMat );
	m_inverseMatrix = m_matrix.Invert();
	m_hasChanged = 1;
}

void IGLUMouseInteractor::SetMatrix( const IGLUMatrix4x4 &newMat )
{
	m_matrix = newMat;
	m_inverseMatrix = m_matrix.Invert();
	m_hasChanged = 1;
}


void IGLUMouseInteractor::GetMatrix( GLfloat *matrix )
{
	for (int i=0;i<16;i++) 
		matrix[i] = m_matrix[i];
}

void IGLUMouseInteractor::GetInverseMatrix( GLfloat *matrix )
{
	for (int i=0;i<16;i++) 
		matrix[i] = m_inverseMatrix[i];
}


bool IGLUMouseInteractor::HasChanged( void )
{
	bool curVal = (m_hasChanged != 0);
	m_hasChanged = 0;
	return curVal;
}



