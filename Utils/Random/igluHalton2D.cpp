/******************************************************************/
/* igluHalton2D.cpp                                               */
/* -----------------------                                        */
/*                                                                */
/* The file defines a generator for a Halton sequence of quasi    */
/*    random numbers on 2D surfaces                               */
/*                                                                */
/* Chris Wyman (09/30/2011)                                       */
/******************************************************************/

#include <math.h>
#include "iglu.h"

using namespace iglu;

IGLUHalton2D::IGLUHalton2D( uint primeBase1, uint primeBase2 ) :
	dim1( primeBase1 ), dim2( primeBase2 )
{
}

vec2 IGLUHalton2D::Sample( unsigned int sampIdx )
{
	return vec2( dim1.fSample( sampIdx ), dim2.fSample( sampIdx ) );
}