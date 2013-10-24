/***************************************************************************/
/* igluGeomUtils.h                                                         */
/* -------------------                                                     */
/*                                                                         */
/* This "class" is simply a collection of utilities for creating simple or */
/*     common geometry that may be needed by many programs.                */
/*                                                                         */
/* Chris Wyman (10/13/2011)                                                */
/***************************************************************************/

#ifndef IGLU_GEOMUTILS_H
#define IGLU_GEOMUTILS_H


#pragma warning( disable: 4996 )

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>

namespace iglu {

class IGLUVertexArray;

enum {
	IGLU_GEOM_DEFAULT      = 0x0000,
	IGLU_GEOM_XY_PLANE     = 0x0001,
	IGLU_GEOM_XZ_PLANE     = 0x0002,
	IGLU_GEOM_YZ_PLANE     = 0x0004,
	IGLU_GEOM_USE_MATLID   = 0x0008,
	IGLU_GEOM_USE_NORMAL   = 0x0010,
	IGLU_GEOM_USE_TEXCOORD = 0x0020,
};



class IGLUGeometry
{
private:
	IGLUGeometry() {};
	~IGLUGeometry() {};

public:
	// Create a quad in [-1..1] in the specified plane.  (TODO: Currently only works with
	//     xy plane and does not yet work using a material ID...)
	static IGLUVertexArray *CreateQuad( uint type=IGLU_GEOM_XY_PLANE, uint matlid=0 );		
};




// End iglu namespace
}




#endif

