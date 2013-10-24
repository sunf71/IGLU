/*****************************************************************************
** igluErrorExit.cpp                                                        **
** ------------                                                             **
**                                                                          **
** A file that defines common functions for crashing or printing fatal      **
**    error messages inside of IGLU.                                        **
**                                                                          **
** Chris Wyman (2/23/2012)                                                  **
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "iglu.h"

using namespace iglu;

void iglu::ErrorExit( const char *error, 
			          const char *fileName, const char *funcName, int lineNum )
{
	printf("*** Encountered Fatal IGLU Error!!\n");
	printf("    -> Line: %4d,  File: [%s]\n", lineNum, fileName );
	printf("    ->          Function: [%s]\n", funcName); 
	printf("\n*** Detailed Message Follows:\n\n%s\n", error);
	exit(-1);
}

void iglu::Warning( const char *error, 
			          const char *fileName, const char *funcName, int lineNum )
{
	printf("*** WARNING.  Undesirable IGLU behavior!  Consider code modifications!\n");
	printf("    -> Line: %4d,  File: [%s]\n", lineNum, fileName );
	printf("    ->          Function: [%s]\n", funcName); 
	printf("*** Detailed Message Follows:\n%s\n", error);
}



void iglu::StatusMessage( const char *error )
{
	printf("IGLU Status: %s\n", error);
}

