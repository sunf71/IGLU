/*****************************************************************************
** igluErrorExit.h                                                          **
** ------------                                                             **
**                                                                          **
** A header that defines common functions for crashing or printing fatal    **
**    error messages inside of IGLU.                                        **
**                                                                          **
** Chris Wyman (2/23/2012)                                                  **
*****************************************************************************/

#ifndef IGLU__ERROREXIT_H
#define IGLU__ERROREXIT_H

namespace iglu {

void ErrorExit( const char *error,     // Your error message
			    const char *fileName,  // Usually the preprocessor macro __FILE__
				const char *funcName,  // Usually the compiler built-in __FUNCTION__
				int lineNum );         // Usually the preprocessor macro __LINE__

void Warning  ( const char *error,     // Your error message
			    const char *fileName,  // Usually the preprocessor macro __FILE__
				const char *funcName,  // Usually the compiler built-in __FUNCTION__
				int lineNum );         // Usually the preprocessor macro __LINE__


} // end namespace iglu.


#endif