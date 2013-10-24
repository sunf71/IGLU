/******************************************************************/
/* igluOrthoNormalBasis.cpp                                       */
/* -----------------------                                        */
/*                                                                */
/* The file defines an ortho normal basis class.                  */
/*                                                                */
/* This class takes in a vector and creates two mutually-         */
/*     perpendicular vectors that are both, also, perpendicular   */
/*     to the input vector.  In other words, it creates an ortho- */
/*     normal basis from the the input, and defines (in the new   */
/*     space) the input vector as the z-vector for the purposes   */
/*     of this class API.                                         */
/*                                                                */
/* Please note that this ortho normal basis is NOT unique, and if */  
/*     you pass in slightly varying inputs to the constructor,    */
/*     you may get wildly varying x and y axes.  However, often   */
/*     just having any basis is all that you need.                */
/*                                                                */
/* Chris Wyman (09/30/2011)                                       */
/******************************************************************/

#include <math.h>
#include "iglu.h"

using namespace iglu;

IGLUOrthoNormalBasis::IGLUOrthoNormalBasis( const vec3 &zAxis ) :
	m_zAxis( zAxis )
{
	// Users are silly...  They might not realize the input should be normalized.
	m_zAxis.Normalize();

	// Compute ortho-normal vectors using the approach of Stark in J. Graph. Tools 
	//    14(1) "Efficient Construction of Perpendicular Vectors without Branching."
	//    (Which, of course, is only non-branching on the CPU if all my functions are
	//    inlined appropriately.)
	bool xGreaterZ = fabs(m_zAxis.X()) > fabs(m_zAxis.Z());
	m_yAxis = vec3( xGreaterZ ? -m_zAxis.Y() : 0,
		            xGreaterZ ?  m_zAxis.X() : -m_zAxis.Z(),
					xGreaterZ ?  0           :  m_zAxis.Y() ).vNormalize();
	m_xAxis = m_yAxis.Cross( m_zAxis );
}

vec3 IGLUOrthoNormalBasis::ToBasis( const vec3 &p )
{
	return p.X()*m_xAxis + p.Y()*m_yAxis + p.Z()*m_zAxis;
}