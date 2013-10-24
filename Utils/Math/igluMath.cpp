/******************************************************************/
/* igluiMath.cpp                                                  */
/* -----------------------                                        */
/*                                                                */
/* Basic math functionality needed inside IGLUI.  Some of this    */
/*     might or might not be available on all systems, so we'll   */
/*     just define our own for consistancy and ease of porting.   */
/*                                                                */
/* Chris Wyman (12/06/2009)                                       */
/******************************************************************/

#include "iglu.h"
#include <math.h>

// A simple log-base2 function
float iglu::log2( float x )
{
	return (float)(log(x)/log(2.0));
}

