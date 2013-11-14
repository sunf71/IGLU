/******************************************************************/
/* igluFileParser.cpp                                             */
/* -----------------------                                        */
/*                                                                */
/* This class provides a cleanish parsing interface for various   */
/*   files, allowing easy tokenizing and reading of at least some */
/*   basic IGLU datatype classes directly.                        */
/*                                                                */
/* Chris Wyman (06/09/2010)                                       */
/******************************************************************/

#include "iglu.h"

using namespace iglu;

#pragma warning( disable : 4996 )

IGLUFileParser::IGLUFileParser( char *filename, bool verbose) : 
	f(NULL), lineNum(0), fileName(0), m_closed(false)
{
	f = fopen( filename, "r" );
	if (!f)
	{
		printf("*** Error: IGLUFileParser unable to open '%s'...\n", filename);
		exit(-1);
	}
	fileName = strdup( filename );
	char *fptr = strrchr( fileName, '/' );
	char *rptr = strrchr( fileName, '\\' );
	if (!fptr && !rptr)
		unqualifiedFileName = fileName;
	else if (!fptr && rptr)
		unqualifiedFileName = rptr+1;
	else if (fptr && !rptr)
		unqualifiedFileName = fptr+1;
	else
		unqualifiedFileName = (fptr>rptr ? fptr+1 : rptr+1);

	fileDirectory = (char *)malloc( (unqualifiedFileName-fileName+1)*sizeof(char)); 
	strncpy( fileDirectory, filename, unqualifiedFileName-fileName );
	fileDirectory[unqualifiedFileName-fileName]=0;
}

IGLUFileParser::~IGLUFileParser()
{
	if (!m_closed) fclose( f );
	if (fileName) free( fileName );
}

void IGLUFileParser::CloseFile( void )
{
	fclose( f );
	m_closed = true;
}


char *IGLUFileParser::ReadNextLine( bool discardBlanks )
{
	char *haveLine;

	// Get the next line from the file, and increment our line counter
	while ( (haveLine = __ReadLine()) && discardBlanks )
	{
		char *ptr = StripLeadingWhiteSpace( internalBuf );

		// If we start with a comment or have no non-blank characters read a new line.
		if (ptr[0] == '#' || ptr[0] == 0) 
			continue;
		else
			break;
	}
	if (!haveLine) return NULL;

	// Replace '\n' at the end of the line with 0.
	char *tmp = strrchr( internalBuf, '\n' );
	if (tmp) tmp[0] = 0;

	// When we start looking through the line, we'll want the first non-whitespace
	internalBufPtr = StripLeadingWhiteSpace( internalBuf );
	return internalBufPtr;
}

char *IGLUFileParser::__ReadLine( void )
{
	if (m_closed) 
		return 0;
	char *ptr = fgets(internalBuf, 2048, f);
	if (ptr) lineNum++;
	return ptr;
}

char *IGLUFileParser::GetToken( char *token )
{
	internalBufPtr = StripLeadingTokenToBuffer( internalBufPtr, token );
	return internalBufPtr;
}

char *IGLUFileParser::GetLowerCaseToken( char *token )
{
	internalBufPtr = StripLeadingTokenToBuffer( internalBufPtr, token );
	MakeLower( token );
	return internalBufPtr;
}

char *IGLUFileParser::GetUpperCaseToken( char *token )
{
	internalBufPtr = StripLeadingTokenToBuffer( internalBufPtr, token );
	MakeUpper( token );
	return internalBufPtr;
}

int		 IGLUFileParser::GetInteger( void )
{
	char token[ 128 ];
	internalBufPtr = StripLeadingNumericalToken( internalBufPtr, token );
	return (int)atoi( token );
}

unsigned IGLUFileParser::GetUnsigned( void )
{
	char token[ 128 ];
	internalBufPtr = StripLeadingNumericalToken( internalBufPtr, token );
	return (unsigned)atoi( token );
}

float	 IGLUFileParser::GetFloat( void )
{
	char token[ 128 ];
	internalBufPtr = StripLeadingNumericalToken( internalBufPtr, token );
	return (float)atof( token );
}

double	 IGLUFileParser::GetDouble( void )
{
	char token[ 128 ];
	internalBufPtr = StripLeadingNumericalToken( internalBufPtr, token );
	return atof( token );
}

vec2	 IGLUFileParser:: GetVec2( void )
{
	float x = GetFloat();
	float y = GetFloat();
	return vec2( x, y );
}

vec3	 IGLUFileParser:: GetVec3( void )
{
	float x = GetFloat();
	float y = GetFloat();
	float z = GetFloat();
	return vec3( x, y, z );
}

vec4	 IGLUFileParser:: GetVec4( void )
{
	float x = GetFloat();
	float y = GetFloat();
	float z = GetFloat();
	float w = GetFloat();
	return vec4( x, y, z, w );
}

int2	 IGLUFileParser::GetIVec2( void )
{
	int x = GetInteger();
	int y = GetInteger();
	return int2( x, y );
}

int3	 IGLUFileParser::GetIVec3( void )
{
	int x = GetInteger();
	int y = GetInteger();
	int z = GetInteger();
	return int3( x, y, z );
}

int4	 IGLUFileParser::GetIVec4( void )
{
	int x = GetInteger();
	int y = GetInteger();
	int z = GetInteger();
	int w = GetInteger();
	return int4( x, y, z, w );
}

uint2	 IGLUFileParser::GetUVec2( void )
{
	uint x = GetUnsigned();
	uint y = GetUnsigned();
	return uint2( x, y );
}

uint3	 IGLUFileParser::GetUVec3( void )
{
	uint x = GetUnsigned();
	uint y = GetUnsigned();
	uint z = GetUnsigned();
	return uint3( x, y, z );
}

uint4	 IGLUFileParser::GetUVec4( void )
{
	uint x = GetUnsigned();
	uint y = GetUnsigned();
	uint z = GetUnsigned();
	uint w = GetUnsigned();
	return uint4( x, y, z, w );
}

dvec2	 IGLUFileParser::GetDVec2( void )
{
	double x = GetDouble();
	double y = GetDouble();
	return dvec2( x, y );
}

dvec3	 IGLUFileParser::GetDVec3( void )
{
	double x = GetDouble();
	double y = GetDouble();
	double z = GetDouble();
	return dvec3( x, y, z );
}

dvec4	 IGLUFileParser::GetDVec4( void )
{
	double x = GetDouble();
	double y = GetDouble();
	double z = GetDouble();
	double w = GetDouble();
	return dvec4( x, y, z, w );
}


char *	 IGLUFileParser::GetInteger( int *result )
{
	char token[ 128 ];
	internalBufPtr = StripLeadingTokenToBuffer( internalBufPtr, token );
	*result = (int)atoi( token );
	return internalBufPtr;
}

char *	 IGLUFileParser::GetUnsigned( unsigned *result )
{
	char token[ 128 ];
	internalBufPtr = StripLeadingTokenToBuffer( internalBufPtr, token );
	*result = (unsigned)atoi( token );
	return internalBufPtr;
}

char *	 IGLUFileParser::GetFloat( float *result )
{
	char token[ 128 ];
	internalBufPtr = StripLeadingTokenToBuffer( internalBufPtr, token );
	*result = (float)atof( token );
	return internalBufPtr;
}

char *	 IGLUFileParser::GetDouble( double *result )
{
	char token[ 128 ];
	internalBufPtr = StripLeadingTokenToBuffer( internalBufPtr, token );
	*result = atof( token );
	return internalBufPtr;
}

void IGLUFileParser::ResetProcessingForCurrentLine( void )
{
	internalBufPtr = StripLeadingWhiteSpace( internalBuf );
}

void IGLUFileParser::WarningMessage( const char *msg )
{
	printf( "Warning: %s (%s, line %d)\n", msg, GetFileName(), GetLineNumber() );
}

void IGLUFileParser::WarningMessage( const char *msg, const char *paramStr )
{
	char buf[ 256 ];
	sprintf( buf, msg, paramStr );
	printf( "Warning: %s (%s, line %d)\n", buf, GetFileName(), GetLineNumber() );
}

void IGLUFileParser::ErrorMessage( const char *msg )
{
	printf( "Error: %s (%s, line %d)\n", msg, GetFileName(), GetLineNumber() );
	exit(-1);
}

void IGLUFileParser::ErrorMessage( const char *msg, const char *paramStr )
{
	char buf[ 256 ];
	sprintf( buf, msg, paramStr );
	printf( "Error: %s (%s, line %d)\n", buf, GetFileName(), GetLineNumber() );
	exit(-1);
}

