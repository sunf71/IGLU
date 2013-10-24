/******************************************************************/
/* igluModel.h                                                    */
/* -----------------------                                        */
/*                                                                */
/* Defines a base class that all renderable geometry inherits.    */
/*     This provides a base functionality developers can expect   */
/*     from draw-able geometry.                                   */
/*                                                                */
/* Chris Wyman (10/24/2011)                                       */
/******************************************************************/

#ifndef IGLU_MODEL_H
#define IGLU_MODEL_H

#include "iglu/igluShaderProgram.h"

namespace iglu {

class IGLUModel 
{
public:
	// Trivial constructor/destructor
	IGLUModel()                                     {}
	virtual ~IGLUModel()                            {}

	// A draw routine returning either IGLU_NO_ERROR or an error code
	virtual int Draw( IGLUShaderProgram::Ptr &shader ) = 0;

	// Specifies what data is available in the renderable primitive
        virtual bool HasVertices ( void ) const         { return true; }
	virtual bool HasTexCoords( void ) const         { return false; }
	virtual bool HasNormals  ( void ) const         { return false; }
	virtual bool HasMatlID   ( void ) const         { return false; }
        virtual bool HasObjectID ( void ) const		{ return false; }

	// A pointer to a IGLUModel could have type IGLUModel::Ptr
	typedef IGLUModel *Ptr;
};




// End namespace iglu
}



#endif
