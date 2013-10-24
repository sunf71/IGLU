/************************************************************************/
/* igluCPUTimer.h                                                       */
/* --------------                                                       */
/*                                                                      */
/* A basic timer for determining GPU usage based on GPU cycle-counting  */
/*     hardware and the OpenGL timer-query extension/API.               */
/*                                                                      */
/* Chris Wyman (9/26/2011)                                              */
/************************************************************************/


#include "iglu.h"

/************************************************************************/
/*  First, identify which OS we're using.                               */
/*      (May need to be tweaked, esp on Windows if not using MS VC++)   */
/************************************************************************/

// Check are we using Windows? 
#if defined(WIN32) && defined(_MSC_VER) && !defined(USING_MSVC)
	#define USING_MSVC 
#endif

// Check are we using MacOS?
#if defined(__APPLE__) && !defined(USING_MACOSX) && !defined(USING_LINUX) 
	#define USING_MACOSX
#endif

// Check are we using Linux?
#if defined(__GNUC__) && !defined(USING_MACOSX) && !defined(USING_LINUX) 
	#define USING_LINUX
#endif

/************************************************************************/
/*  Now define OS-specific data structures & funcs needed by our class  */
/************************************************************************/

#if   defined(USING_MSVC)
          #include <windows.h>
          #pragma comment(lib, "kernel32.lib")
          typedef LARGE_INTEGER TimerStruct;
          void igluGetHighResolutionTime( TimerStruct *t ) { QueryPerformanceCounter( t ); }
          double igluConvertTimeDifferenceToSec( TimerStruct *end, TimerStruct *begin ) 
			{ TimerStruct freq; QueryPerformanceFrequency( &freq );  return (end->QuadPart - begin->QuadPart)/(double)freq.QuadPart; }
		  double igluConvertTimeDifferenceToMSec( TimerStruct *end, TimerStruct *begin ) 
			{ TimerStruct freq; QueryPerformanceFrequency( &freq );  return (end->QuadPart - begin->QuadPart)/(1e-3*(double)freq.QuadPart); }
#elif defined(USING_LINUX)
          #include <time.h>
          typedef struct timespec TimerStruct;
          void igluGetHighResolutionTime( TimerStruct *t ) 
		{ clock_gettime( CLOCK_REALTIME, t ); }
	  double igluConvertTimeDifferenceToSec( TimerStruct *end, TimerStruct *begin ) 
		{ return (end->tv_sec - begin->tv_sec) + (1e-9)*(end->tv_nsec - begin->tv_nsec); }
	  double igluConvertTimeDifferenceToMSec( TimerStruct *end, TimerStruct *begin ) 
		{ return (end->tv_sec - begin->tv_sec) + (1e-6)*(end->tv_nsec - begin->tv_nsec); }
#elif defined(USING_MACOSX)
          #include <CoreServices/CoreServices.h>
          #include <mach/mach.h>
          #include <mach/mach_time.h>
          typedef uint64_t TimerStruct;
	  void igluGetHighResolutionTime( TimerStruct *t ) 
		{ *t = mach_absolute_time(); }
	  double igluConvertTimeDifferenceToSec( TimerStruct *end, TimerStruct *begin ) 
		{ uint64_t elapsed = *end - *begin; Nanoseconds elapsedNano = AbsoluteToNanoseconds( *(AbsoluteTime*)&elapsed );
			return double(*(uint64_t*)&elapsedNano) * (1e-9); }
	  double igluConvertTimeDifferenceToMSec( TimerStruct *end, TimerStruct *begin ) 
		{ uint64_t elapsed = *end - *begin; Nanoseconds elapsedNano = AbsoluteToNanoseconds( *(AbsoluteTime*)&elapsed );
			return double(*(uint64_t*)&elapsedNano) * (1e-6); }
#endif



using namespace iglu;

iglu::IGLUCPUTimer::IGLUCPUTimer()
{
	igluGetHighResolutionTime( &lastTime );
}

void iglu::IGLUCPUTimer::Start( void )
{
	igluGetHighResolutionTime( &lastTime ); 
}

double iglu::IGLUCPUTimer::GetTime( void )
{
	igluGetHighResolutionTime( &curTime );  
	return igluConvertTimeDifferenceToMSec( &curTime, &lastTime ); 
}


double iglu::IGLUCPUTimer::Tick( void )
{
	curTime = lastTime; 
	igluGetHighResolutionTime( &lastTime ); 
	return igluConvertTimeDifferenceToMSec( &lastTime, &curTime ); 
}


