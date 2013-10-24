/***************************************************************************/
/* igluGeomUtils.cpp                                                       */
/* -------------------                                                     */
/*                                                                         */
/* This "class" is simply a collection of drawing utilities, each is a     */
/*     static method.  In some sense, these really belong as part of other */
/*     classes (e.g., the full-screen texture draw could belong to         */
/*     IGLUTexture).  But these are not really integral to the behavior    */
/*     of the associated classes and carry a lot of overhead (e.g.,        */
/*     shaders) that would clutter up other classes.                       */
/*                                                                         */
/* Chris Wyman (10/13/2011)                                                */
/***************************************************************************/


#include "iglu.h"

using namespace iglu;


namespace {  // anonymous namespace to hide globals inside this file

// All planes should be the same size!!
float xyplane[36] = {0, 0,1, 0,0,-1, -1,-1,0,    0, 1,1, 0,0,-1, 1,-1,0,    0, 0,0, 0,0,-1, -1,1,0,    0, 1,0, 0,0,-1, 1,1,0};
float yzplane[36] = {0, 0,1, 1,0,0, 0,-1,-1,     0, 1,1, 1,0,0, 0,-1,1,     0, 0,0, 1,0,0, 0,1,-1,     0, 1,0, 1,0,0, 0,1,1};
float xzplane[36] = {0, 0,1, 0,1,0, -1,0,-1,     0, 1,1, 0,1,0, 1,0,-1,     0, 0,0, 0,1,0, -1,0,1,     0, 1,0, 0,1,0, 1,0,1};

}

IGLUVertexArray *iglu::IGLUGeometry::CreateQuad( uint type, uint matlid )
{
	float *vptr = xyplane;
	if ( type & IGLU_GEOM_XZ_PLANE ) vptr = xzplane;
	if ( type & IGLU_GEOM_YZ_PLANE ) vptr = yzplane;

	IGLUVertexArray::Ptr tmp = new IGLUVertexArray();
	tmp->SetVertexArray( sizeof( xyplane ), vptr, IGLU_STATIC|IGLU_DRAW );
	tmp->EnableAttribute( IGLU_ATTRIB_VERTEX, 3, GL_FLOAT, 9 * sizeof(float), BUFFER_OFFSET(6*sizeof(float)) );
	if (type & IGLU_GEOM_USE_NORMAL) 
		tmp->EnableAttribute( IGLU_ATTRIB_NORMAL, 3, GL_FLOAT, 9 * sizeof(float), BUFFER_OFFSET(3*sizeof(float)) );
	if (type & IGLU_GEOM_USE_TEXCOORD) 
		tmp->EnableAttribute( IGLU_ATTRIB_TEXCOORD, 2, GL_FLOAT, 9 * sizeof(float), BUFFER_OFFSET(1*sizeof(float)) );

	return tmp;
}