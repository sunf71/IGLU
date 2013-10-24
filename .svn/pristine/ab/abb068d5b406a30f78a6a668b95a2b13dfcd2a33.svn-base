/************************************************************************/
/* igluFrameRate.cpp                                                    */
/* ------------------                                                   */
/*                                                                      */
/* This is a pretty simple class that allows you to start timing at the */
/*    beginning of a frame's rendering and display the current          */
/*    framerate at the end of the frame.  Additionally, because 1-frame */
/*    counters are notoriously noisy (due to context switches, etc),    */
/*    this class allows the averaging over multiple frames, specified   */
/*    to the constructor.  Please note this class works only if the     */
/*    screen continuously updates (e.g., the idle function calls        */
/*    glutPostRedisplay()) -- after all a "framerate" makes no sense    */
/*    otherwise.                                                        */
/*                                                                      */
/* Chris Wyman (12/7/2007)                                              */
/************************************************************************/

#include "iglu.h"

using namespace iglu;

// Grab some functions from igluCPUTimer.cpp
void igluGetHighResolutionTime( TimerStruct *t );
double igluConvertTimeDifferenceToSec( TimerStruct *end, TimerStruct *begin );


IGLUFrameRate::IGLUFrameRate( int avgOverFrames ) : avgOverFrames(avgOverFrames),
	currFrameCount(0), frameIdx(0), lastFrameRate(0)
{
	timeArray = (TimerStruct *)malloc( avgOverFrames * sizeof( TimerStruct ) );
}


void IGLUFrameRate::StartFrame( void )
{
	// Check to see if we're in the first few frames.
	if (currFrameCount < avgOverFrames)
		currFrameCount++;

	// Get the current time, increment the index.
	igluGetHighResolutionTime( &timeArray[frameIdx++] );
	frameIdx %= avgOverFrames;
}


float IGLUFrameRate::EndFrame( void )
{
	// If this is called before we've done any timing at all, fail gracefully.
	if (currFrameCount <= 0) return 0.0f; 

	// Get the current time
	TimerStruct curTime;
	igluGetHighResolutionTime( &curTime );	

	// Get the time difference.  
	int idx = (currFrameCount == avgOverFrames) ? frameIdx : 0;
	float sec = igluConvertTimeDifferenceToSec( &curTime, &timeArray[idx] );

	// Divide # frames by time to get frames-per-second
	return (lastFrameRate = (currFrameCount / sec));
}


