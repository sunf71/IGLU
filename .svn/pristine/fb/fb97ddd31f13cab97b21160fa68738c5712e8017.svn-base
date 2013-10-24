/******************************************************************/
/* igluFloat.cpp                                                  */
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
#include <FL/Fl_Valuator.H>

using namespace iglu;

IGLUFloat::IGLUFloat() :
	IGLUVariable(0), m_value(0), m_max(-1.0f), m_min(1.0f), m_delta(0.1)
{
}

IGLUFloat::IGLUFloat( float value, float _min, float _max, char *name ) : 
	IGLUVariable(name), m_value(value), m_max(_max), m_min(_min), m_delta(0.1)
{
}

IGLUFloat::IGLUFloat( float value, const IGLURange<float> &range, float delta, char *name ) : 
	IGLUVariable(name), m_value(value), m_max(range.GetMax()), m_min(range.GetMin()),
	m_delta(delta)
{
}

void IGLUFloat::Initialize( float value, const IGLURange<float> &range, float delta, char *name )
{
	m_min = range.GetMin();
	m_max = range.GetMax();
	m_delta = delta;
	m_value = iglu::clamp( value, m_min, m_max );
	if (name) SetName( name );
}

void IGLUFloat::SetValue( float newValue, bool force )
{
	float oldVal = m_value;
	m_value = (!force) ? iglu::clamp( newValue, m_min, m_max ) : newValue;

	// If we've modified the variable, update any FLTK widget and call
	//    any callback we may have set up.
	if (oldVal != m_value)
	{
		for (uint i=0; i<m_fltkValuators.Size(); i++)
			((Fl_Valuator *)m_fltkValuators[i])->value( double(m_value) );
		//if (m_fltkValuator)
		//	((Fl_Valuator *)m_fltkValuator)->value( double(m_value) );
		ExecuteCallbacks();
	}
}

bool IGLUFloat::UpdateFromInput( unsigned int inputKey )
{
	bool handled = false;
	float newVal = m_value;

	// Check all keys we respond to, check if this matches.
	for (unsigned int i=0; i< m_keys.Size(); i++)
	{
		if (inputKey == m_keys[i])
		{
			newVal += m_responses[i];
			handled = true;
			break;
		}
	}

	// Update the value, if need be
	if (newVal != m_value)
		SetValue( newVal );

	return handled;
}


void IGLUFloat::FLTKCallback( Fl_Widget *fltkWidget, void *igluFloat )
{
	// Cast the inputs so we can appropriately set the value.
	IGLUFloat *ptr = (IGLUFloat *)igluFloat;
	Fl_Valuator *val = (Fl_Valuator *)fltkWidget;

	// We'll only update stuff if this actually modifies things!
	if ( ptr->m_value != float(val->value()) ) 
		ptr->SetValue( val->value() );
}

