/******************************************************************/
/* igluBool.cpp                                                   */
/* -----------------------                                        */
/*                                                                */
/* There's nearly always values in your program that need to      */
/*     change according to some user input.  Often these values   */
/*     require all sorts of logic to update, set, get, or         */
/*     change in response to user interface.                      */
/* This class, and others like it, inherit from UIVariable and    */
/*     try to avoid spreading UI interface logic out over the     */
/*     entire code, and instead encapsulate it in these classes.  */
/*     The additional advantage here, is that derived classes     */
/*     from UIVariable can all be treated identically, avoiding   */
/*     special-casing depending on the variable type.             */
/*                                                                */
/* Realize that this class adds overhead, and shouldn't be used   */
/*     for all variables.  Just those that might vary with input  */
/*     or, perhaps, those defined in an input or script file.     */
/*                                                                */
/* Chris Wyman (02/09/2012)                                       */
/******************************************************************/


#include "iglu.h"
#include <FL/Fl_Button.H>

using namespace iglu;

void IGLUBool::Initialize( bool value, char *name )
{
	m_value = value;
	if (name) SetName( name );
}

bool IGLUBool::UpdateFromInput( unsigned int inputKey )
{
	bool modified = false;
	bool newVal   = m_value;
	
	// Check all keys we respond to, check if this matches.
	for (unsigned int i=0; i< m_keys.Size(); i++)
	{
		if (inputKey == m_keys[i])
		{
			newVal = !newVal;
			modified = true;
			break;
		}
	}

	// Update the value, if need be
	if (newVal != m_value)
		SetValue( newVal );

	return modified;
}

void IGLUBool::SetValue( bool newValue )
{
	if (newValue == m_value) 
		return;

	m_value = newValue;

	// Update any FLTK widget and call any callback we may have set up.
	for (uint i=0; i<m_fltkValuators.Size(); i++)
		((Fl_Button *)m_fltkValuators[i])->value( m_value ? 1 : 0 );

	ExecuteCallbacks();
}

// This method is a callback that allows FLTK widget to modify IGLUInt values
void IGLUBool::FLTKCallback( Fl_Widget *fltkWidget, void *igluBool )
{
	// Cast the inputs so we can appropriately set the value.
	IGLUBool *ptr = (IGLUBool *)igluBool;
	Fl_Button *val = (Fl_Button *)fltkWidget;

	// If the value has been modified (should be...)
	if ( ptr->m_value != (val->value() != 0) )
		ptr->SetValue( val->value() != 0 );
}
