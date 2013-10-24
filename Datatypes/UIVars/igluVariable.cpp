/******************************************************************/
/* igluVariable.cpp                                               */
/* -----------------------                                        */
/*                                                                */
/* There's nearly always values in your program that need to      */
/*     change according to some user input.  Often these values   */
/*     require all sorts of logic to update, set, get, or         */
/*     change in response to user interface.                      */
/* This class tries to avoid spreading this logic out over the    */
/*     entire code, and instead encapsulate it all in a class.    */
/*     The additional advantage here, is that derived classes     */
/*     from IGLUVariable can all be treated identically, avoiding */
/*     special-casing depending on the variable type.             */
/*                                                                */
/* Realize that this class adds overhead, and shouldn't be used   */
/*     for all variables.  Just those that might vary with input  */
/*     or, perhaps, those defined in an input or script file.     */
/*                                                                */
/* Chris Wyman (02/09/2012)                                       */
/******************************************************************/
#include "iglu.h"

using namespace iglu;



bool IGLUVariable::RespondsToInput( unsigned int inputKey )
{
	for (unsigned int i=0; i< m_keys.Size(); i++)
		if (inputKey == m_keys[i]) return true;
	return false;
}


void IGLUVariable::RemoveKeyResponse( unsigned int key )
{
	// We should really shrink the array, not just overwrite
	//    the keystroke with a NULL.  But this should be an infrequently
	//    used function, and IGLUArray1D does not currently support 
	//    removing array entries.
	for (unsigned int i=0; i< m_keys.Size(); i++)
		if (key == m_keys[i]) m_keys[i] = KEY_UNKNOWN;
}

/*
void IGLUVariable::FLTKSetValuator( void *fltkValuator )
{
	m_fltkValuator = fltkValuator;
}
*/

void IGLUVariable::SetCallback( CallbackPtr_VoidP callback )
{
	Warning( "SetCallback() is depricated!  Please consider using AddCallback() with a new\n"
		     "   IGLUCallback-derived class.  This allows a more general set of callbacks\n"
			 "   and should keep both IGLU and developed code cleaner!  SetCallback() will\n"
			 "   go away shortly!\n", __FILE__, __FUNCTION__, __LINE__ );

	m_valueCallback = callback;
}	

void IGLUVariable::SetCallback( CallbackPtr_VoidP_VoidP callback, void *firstInput )
{
	Warning( "SetCallback() is depricated!  Please consider using AddCallback() with a new\n"
		     "   IGLUCallback-derived class.  This allows a more general set of callbacks\n"
			 "   and should keep both IGLU and developed code cleaner!  SetCallback() will\n"
			 "   go away shortly!\n", __FILE__, __FUNCTION__, __LINE__ );

	m_valueCallback2 = callback;
	m_callback2Data  = firstInput;
}

void IGLUVariable::ExecuteCallbacks( void )
{
	for (uint i=0; i<m_callbacks.Size(); i++)
		m_callbacks[i]->Execute( this );

	/* Callbacks below here are going away soon */
	if (m_valueCallback)
		(*m_valueCallback)((void *)this);
	if (m_valueCallback2)
		(*m_valueCallback2)(m_callback2Data, (void *)this);
}
