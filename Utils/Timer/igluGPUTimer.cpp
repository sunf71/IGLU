/************************************************************************/
/* igluGPUTimer.h                                                       */
/* --------------                                                       */
/*                                                                      */
/* A basic timer for determining GPU usage based on GPU cycle-counting  */
/*     hardware and the OpenGL timer-query extension/API.               */
/*                                                                      */
/* Chris Wyman (9/26/2011)                                              */
/************************************************************************/


#include "iglu.h"

using namespace iglu;


IGLUGPUTimer::IGLUGPUTimer() : timeQuery(0), initialized(false)
{
	// This object might be constructed prior to GLEW initialization.
	//    If so, we'll construct the object lazily, when used.
	if (glGenQueries)
	{
		glGenQueries(1,&timeQuery);
		initialized = true;
	}
	nsElapsed = totalElapsed = 0;
}


void IGLUGPUTimer::Start( void )
{
	// Ok, now we really need the timer query object.  If we couldn't
	//    allocate it before, do it now!
	if (!initialized)
	{
		glGenQueries(1,&timeQuery);
		initialized = true;
	}
	nsElapsed = totalElapsed = 0;
	glBeginQuery( GL_TIME_ELAPSED, timeQuery );
}

double IGLUGPUTimer::GetTime( void )
{
	// Stop timing
	glEndQuery( GL_TIME_ELAPSED );

	// Wait for our result
	int queryReady=0;
	while (!queryReady) 
		glGetQueryObjectiv( timeQuery, GL_QUERY_RESULT_AVAILABLE, &queryReady );

	// Get the elapsed time
	glGetQueryObjectui64v( timeQuery, GL_QUERY_RESULT, &nsElapsed );

	// Since we're not restarting the time, just getting the current time, 
	//    update the current time, restart the query, then return our current time
	totalElapsed += nsElapsed;
	glBeginQuery( GL_TIME_ELAPSED, timeQuery );

	// Return time in milliseconds
	return totalElapsed*(1.0e-6);
}


double IGLUGPUTimer::Tick( void )
{
	// Stop timing
	glEndQuery( GL_TIME_ELAPSED );

	// Wait for our result
	int queryReady=0;
	while (!queryReady) 
		glGetQueryObjectiv(timeQuery, GL_QUERY_RESULT_AVAILABLE, &queryReady);

	// Get the elapsed time
	glGetQueryObjectui64v( timeQuery, GL_QUERY_RESULT, &nsElapsed );

	// Since we are restarting the time, compute the current time (store temporarily
	//    in nsElapsed), reset the elapsed time, then restart the query.
	nsElapsed += totalElapsed;
	totalElapsed = 0;
	glBeginQuery( GL_TIME_ELAPSED, timeQuery );

	// Return time in milliseconds
	return nsElapsed*(1.0e-6);
}


