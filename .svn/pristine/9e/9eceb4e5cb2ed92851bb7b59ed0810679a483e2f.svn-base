/**********************************
** igluJPEG.cpp                  **
** -------                       **
**                               **
** Read JPEG images using the    **
**   libjpeg code compiled into  **
**   the IGLU library.           **
**                               **
** Chris Wyman (2/28/2012)       **
**********************************/

#include <stdio.h>
#include <stdlib.h>
#include "jpeg/jpeglib.h"
#include "jpeg/jerror.h"
#include "igluJPEG.h"
#include "iglu/errors/igluErrorExit.h"
#include "iglu/errors/igluWarning.h"

using namespace iglu;

void FatalError( char *msg );


unsigned char *iglu::ReadJPEG( char *f, int *width, int *height )
{ 
	char buf[256];
	FILE *pFile = fopen( f, "rb" );
	if (!pFile) {
		sprintf( buf, "ReadJPEG() unable to open file '%s'!", f );
		ErrorExit( buf, __FILE__, __FUNCTION__, __LINE__ );
	}

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, pFile);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

    *width = cinfo.output_width;
    *height = cinfo.output_height;

	unsigned char *img = (unsigned char *)malloc( cinfo.output_width * 
		                                          cinfo.output_height *
												  3 * sizeof(unsigned char) );
	unsigned char *ptr = img;
	if (!img) {
		sprintf( buf, "ReadJPEG() unable to allocate temporary memory!", f );
		FatalError( buf );
	}

	while (cinfo.output_scanline < cinfo.output_height) {
		int numlines = jpeg_read_scanlines(&cinfo, &ptr, 1);
		ptr += numlines * 3 * cinfo.output_width;
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	fclose(pFile);
	return img;
}


