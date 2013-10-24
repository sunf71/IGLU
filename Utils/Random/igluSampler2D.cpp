/******************************************************************/
/* igluSampler2D.cpp                                              */
/* -----------------------                                        */
/*                                                                */
/* The file defines a base class from which all 2D samplers are   */
/*     derived.  Basically, base classes have to define how to    */
/*     get a sample in the range x,y in [0..1), and this base     */
/*     class defines methods that can use these samples to get    */
/*     more interesting behaviors (e.g., samples on hemispheres)  */
/*                                                                */
/* Chris Wyman (09/30/2011)                                       */
/******************************************************************/

#include <math.h>
#include "iglu.h"

using namespace iglu;


vec3 IGLUSampler2D::SamplePositiveZHemisphere( unsigned int sampIdx )
{
	vec2 samp = Sample( sampIdx );
	float r   = sqrt( max(0.0f, 1.0f - samp.X()) );
	float phi = 2.0f * float(IGLU_PI) * samp.Y();

	return vec3( r*cos( phi ), r*sin( phi ), samp.X() );
}

vec3 IGLUSampler2D::SampleCosineWeightedPositiveZHemisphere( unsigned int sampIdx )
{
	vec2 samp = Sample( sampIdx );
	float r   = sqrt( samp.X() );
	float phi = 2.0f * float(IGLU_PI) * samp.Y();

	return vec3( r*cos( phi ), r*sin( phi ), sqrt( max( 0.0f, 1.0f - samp.X() ) ) );
}

vec3 IGLUSampler2D::SampleSphere( unsigned int sampIdx )
{
	vec2 samp = Sample( sampIdx );
	float f1 = 2.0f * samp.X() - 1.0f;
	float f2 = 2.0f * float(IGLU_PI) * samp.Y();
	float r = sqrt( max( 0.0f, 1.0f - f1*f1 ) );

	return vec3( r*cos( f2 ), r*sin( f2 ), f1 );
}

vec3 IGLUSampler2D::SampleHemisphere( const vec3 &inDir, unsigned int sampIdx )
{
	IGLUOrthoNormalBasis onb( inDir );
	return onb.ToBasis( SamplePositiveZHemisphere( sampIdx ) );
}

vec3 IGLUSampler2D::SampleCosineWeightedHemisphere( const vec3 &inDir, unsigned int sampIdx )
{
	IGLUOrthoNormalBasis onb( inDir );
	return onb.ToBasis( SampleCosineWeightedPositiveZHemisphere( sampIdx ) );
}

vec2 IGLUSampler2D::SampleDisk( unsigned int sampIdx )
{
	vec2 samp = Sample( sampIdx );
	float r   = sqrt( samp.X() );
	float phi = 2.0f * float(IGLU_PI) * samp.Y();

	return vec2( r*cos( phi ), r*sin( phi ) );
}

vec3 IGLUSampler2D::SampleTriangle( const vec3 &v0, const vec3 &v1, const vec3 &v2, unsigned int sampIdx )
{
	vec2 samp = Sample( sampIdx );
	float xSqrt = sqrt( samp.X() );
	float bary0 = 1.0f - xSqrt;
	float bary1 = samp.Y() * xSqrt;
	float bary2 = 1.0f - (bary0 + bary1);

	return bary0*v0 + bary1*v1 + bary2*v2;
}