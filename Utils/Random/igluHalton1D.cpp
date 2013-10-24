/******************************************************************/
/* igluHalton1D.cpp                                               */
/* -----------------------                                        */
/*                                                                */
/* The file defines a generator for a Halton sequence of quasi    */
/*    random numbers, given a particular prime number as base.    */
/*    For simple uses, the base will be small prime numbers (e.g. */
/*    2 for one dimension, 3 for another dimension, 5 for the 3rd */
/*    dimension) that vary from one sampling dimension to another */
/*    To ensure good sampling in high dimensional spaces, make    */
/*    there are some de-corellation techniques needed once you    */
/*    get beyond a base of 7 or 11.  This class does not handle   */
/*    these.                                                      */
/*                                                                */
/* Chris Wyman (09/30/2011)                                       */
/******************************************************************/

#include <math.h>
#include "iglu.h"

using namespace iglu;

IGLUHalton1D::IGLUHalton1D( unsigned int primeBase ) : 
	m_base( primeBase ), 
	m_1_basef( 1.0f / primeBase ), 
	m_1_based ( 1.0 / primeBase )
{
}

double         IGLUHalton1D::dSample( unsigned int sampIdx )
{
	double H=0;

	// This parses through sampIdx, in it's representation in the specified
	//     base (e.g., base 2) one digit at a time, reflecting each individual
	//     digit around the decimal point.
	for( double mult = m_1_based ;   // Use our precomputed 1/base
		 sampIdx ; 
		 sampIdx /= m_base ,         // Do *integer* division by our base
		 mult *= m_1_based )         // Do floating-point division by base
		                             //   (using mult by precomputed 1/base)
	{
		// Accumulate reflection of current digit around decimal point
		//     The mod operation takes the least significant digit.
		//     mult reflects this digit around the decimal (changes per digit)
		H += (sampIdx % m_base) * mult;
	}
	return H;
}

float          IGLUHalton1D::fSample( unsigned int sampIdx )
{
	float H=0;

	// This parses through sampIdx, in it's representation in the specified
	//     base (e.g., base 2) one digit at a time, reflecting each individual
	//     digit around the decimal point.
	for( float mult = m_1_basef ;    // Use our precomputed 1/base
		 sampIdx ; 
		 sampIdx /= m_base ,         // Do *integer* division by our base
		 mult *= m_1_basef )         // Do floating-point division by base
		                             //   (using mult by precomputed 1/base)
	{
		// Accumulate reflection of current digit around decimal point
		//     The mod operation takes the least significant digit.
		//     mult reflects this digit around the decimal (changes per digit)
		H += (sampIdx % m_base) * mult;
	}
	return H;
}

unsigned short IGLUHalton1D::sSample( unsigned int sampIdx )
{
	float fSamp = fSample( sampIdx );

    // fSamp < 0 (strictly), so this will never give 2^16.
	return (unsigned short)floor( fSamp * 65536 );  
}

bool           IGLUHalton1D::bSample( unsigned int sampIdx )
{
	float fSamp = fSample( sampIdx );
	return ( fSamp >= 0.5 );
}

unsigned char  IGLUHalton1D::cSample( unsigned int sampIdx )
{
	float fSamp = fSample( sampIdx );

    // fSamp < 0 (strictly), so this will never give 2^8.
	return (unsigned char)floor( fSamp * 256 );
}

