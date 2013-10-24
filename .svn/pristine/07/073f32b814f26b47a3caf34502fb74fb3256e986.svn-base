/******************************************************************/
/* igluHammersley2D.cpp                                           */
/* -----------------------                                        */
/*                                                                */
/* The file defines a generator for a Hammersley sequence of      */
/*    quasi random numbers on 2D surfaces                         */
/*                                                                */
/* Chris Wyman (09/30/2011)                                       */
/******************************************************************/

#include <math.h>
#include "iglu.h"

using namespace iglu;

IGLUHammersley2D::IGLUHammersley2D( uint numSamples ) :
	m_dim2( 2 ), m_numSamples( numSamples )
{
}

vec2 IGLUHammersley2D::Sample( unsigned int sampIdx )
{
	return vec2( sampIdx / float(m_numSamples), m_dim2.fSample( sampIdx ) );
}