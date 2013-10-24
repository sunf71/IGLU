/******************************************************************/
/* igluInt.h                                                      */
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

IGLUInt::IGLUInt() :
	IGLUVariable( 0 ), m_value(0), m_max(-1), m_min(1), m_delta(1)
{
}
	

IGLUInt::IGLUInt( int value, int _min, int _max, char *name ) : 
	IGLUVariable(name), m_value(value), m_max(_max), m_min(_min), m_delta(1)
{
}

IGLUInt::IGLUInt( int value, const IGLURange<int> &range, int delta, char *name ) : 
	IGLUVariable(name), m_value(value), m_max(range.GetMax()), m_min(range.GetMin()), m_delta(delta)
{
}

void IGLUInt::Initialize( int value, const IGLURange<int> &range, int delta, char *name )
{
	m_min = range.GetMin();
	m_max = range.GetMax();
	m_delta = delta;
	m_value = iglu::clamp( value, m_min, m_max );
	if (name) SetName( name );
}

void IGLUInt::AddKeyResponse( unsigned int key, int response )	
{ 
	m_keys.Add( key ); 
	m_responses.Add( response ); 
}

void IGLUInt::SetValue( int newValue, bool force )
{
	int oldVal = m_value;
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

bool IGLUInt::UpdateFromInput( unsigned int inputKey )
{
	bool handled = false;
	int newVal = m_value;

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

// This gets called whenever the FLTK widget's value is modified
void IGLUInt::FLTKCallback( Fl_Widget *fltkWidget, void *igluInt )
{
	// Cast the inputs so we can appropriately set the value.
	IGLUInt *ptr = (IGLUInt *)igluInt;
	Fl_Valuator *val = (Fl_Valuator *)(fltkWidget);

	// We'll only update stuff if this actually modifies things!
	if ( ptr->m_value != int(val->value()) ) 
		ptr->SetValue( val->value() );
}
