/******************************************************************/
/* igluImage.cpp                                                  */
/* -----------------------                                        */
/*                                                                */
/* The file defines an image class that reads images of various   */
/*     kinds into a uniform internal structure.                   */
/*                                                                */
/* Chris Wyman (10/03/2011)                                       */
/******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "iglu/igluImage.h"
#include "Images/igluPPM.h"
#include "Images/igluBMP.h"
#include "Images/igluRGB.h"
#include "Images/igluJPEG.h"
#include <GL/glew.h>

using namespace iglu;

IGLUImage::IGLUImage( char *filename ) :
	m_imgData(0), m_width(-1), m_height(-1),
	m_glDatatype( GL_UNSIGNED_BYTE ), m_glFormat( GL_RGB ),
	m_freeMemory(true)
{
	// Identify the type of file
	char *ptr = strrchr( filename, '.' );
	char buf[16];
	strncpy( buf, ptr, 16 );
	for (int i=0;i<16;i++) buf[i] = tolower( buf[i] );

	// Load the image, if possible.
	bool success = false;

	// Is it a PPM file?
	if (!strcmp(buf, ".ppm"))	
		success = LoadPPM( filename );
	
	// Is it a RGB/SGI file?
	else if (!strcmp(buf, ".rgb") || !strcmp(buf, ".rgba"))
		success = LoadRGB( filename );
	
	// Is it a BMP file?
	else if (!strcmp(buf, ".bmp"))
		success = LoadBMP( filename );

	// Is it a JPEG file?
	else if (!strcmp(buf, ".jpg") || !strcmp(buf, ".jpeg"))
		success = LoadJPEG( filename );

	// We don't know how to load this file...
	else
		printf("IGLUImage:  Unable to load image '%s'...  Unknown file format.\n", filename);

	// Did we fail to load the image for one reason or another?
	if (!success)
		m_width = m_height = -1;
}

IGLUImage::IGLUImage( unsigned char* image, int width, int height, bool useAlpha, bool freeMemory ) :
    m_width( width ), m_height( height ), m_glFormat( useAlpha ? GL_RGBA : GL_RGB ),
	m_glDatatype( GL_UNSIGNED_BYTE ), m_freeMemory(freeMemory)
{
	m_imgData = image;
}

IGLUImage::~IGLUImage()
{
	if (m_imgData && m_freeMemory) free(m_imgData);
}


bool IGLUImage::LoadRGB( char *filename )
{
	int components;
	m_imgData = ReadRGB(filename, &m_width, &m_height, &components, true);
	m_glFormat = GL_RGBA;
	m_glDatatype = GL_UNSIGNED_BYTE;

	if (!m_imgData) 
		return false;
	return true;
}


bool IGLUImage::LoadPPM( char *filename )
{
	int mode;
	m_imgData = ReadPPM( filename, &mode, &m_width, &m_height, false );
	m_glFormat = GL_RGB;
	m_glDatatype = GL_UNSIGNED_BYTE;

	if (!m_imgData) 
		return false;
	return true;
}


bool IGLUImage::LoadBMP( char *filename )
{
	m_imgData = ReadBMP( filename, &m_width, &m_height, true );
	m_glFormat = GL_RGB;
	m_glDatatype = GL_UNSIGNED_BYTE;

	if (!m_imgData) 
		return false;
	return true;
}

bool IGLUImage::LoadJPEG( char *filename )
{
	m_imgData = ReadJPEG( filename, &m_width, &m_height );
	m_glFormat = GL_RGB;
	m_glDatatype = GL_UNSIGNED_BYTE;

	if (!m_imgData) 
		return false;
	return true;
}


void IGLUImage::SaveAsPPM( char *ppmFile )
{
	if (!IsValid()) return;

	WritePPM( ppmFile, PPM_RAW, m_width, m_height, m_imgData );
}


