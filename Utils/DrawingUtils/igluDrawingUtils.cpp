/***************************************************************************/
/* igluDrawingUtils.cpp                                                    */
/* -------------------                                                     */
/*                                                                         */
/* This "class" is simply a collection of drawing utilities, each is a     */
/*     static method.  In some sense, these really belong as part of other */
/*     classes (e.g., the full-screen texture draw could belong to         */
/*     IGLUTexture).  But these are not really integral to the behavior    */
/*     of the associated classes and carry a lot of overhead (e.g.,        */
/*     shaders) that would clutter up other classes.                       */
/*                                                                         */
/* Chris Wyman (10/13/2011)                                                */
/***************************************************************************/


#include "iglu.h"

using namespace iglu;

namespace {  // anonymous namespace to hide globals inside this file

static char iglu_FullScreenVS[] = {
	"#version 400\n"
	"layout(location = 0) in vec3 vertex;\n"
	"layout(location = 2) in vec2 texcoord;\n"
	"out vec2 fragTexCoord;\n"
	"void main( void ) {\n"
	"    gl_Position = vec4( vertex.xyz, 1.0f );\n"
	"    fragTexCoord = texcoord.xy;\n"
	"}\n"
};

static char iglu_FullScreenFS[] = {
	"#version 400\n"
	"uniform sampler2D copyTex;\n"
	"uniform float flipX, flipY;\n"
	"in vec2 fragTexCoord;\n"
	"out vec4 result;\n"
	"void main( void ) {\n"
	"    bvec2 flip = bvec2( (flipX > 0.5), (flipY > 0.5) );"
	"    vec4 texColor = texture( copyTex, vec2( flip.x ? 1.0-fragTexCoord.x : fragTexCoord.x, flip.y ? 1.0-fragTexCoord.y : fragTexCoord.y ) );\n"
	"    result = texColor;\n"
	"}\n"
};

static float iglu_FullScreenQuadData[] = {
	0, 0,   -1, -1, 0,   0, 0, -1,  // Lower-left  vertex, (u,v) = (0,0)
	1, 0,   1, -1, 0,    0, 0, -1,  // Lower-right vertex, (u,v) = (1,0)
	0, 1,   -1, 1, 0,    0, 0, -1,  // Upper-left  vertex, (u,v) = (0,1)
	1, 1,   1, 1, 0,     0, 0, -1,  // Upper-right vertex, (u,v) = (1,1)
};

bool iglu_FullScreenInitialized   = false;
IGLUShaderProgram::Ptr fullScreen = 0; 
IGLUVertexArray::Ptr   quadGeom   = 0;

} // End anonymous namespace for globals inside this file...

static bool InitializeFullScreen( void )
{
	// Make sure GLEW has been initialized
	if (!glGetAttribLocation)
		return false;

	// Create our full-screen shader program.
	fullScreen = new IGLUShaderProgram();
	fullScreen->CreateFromString( iglu_FullScreenVS, iglu_FullScreenFS );
	fullScreen->SetProgramDisables( IGLU_GLSL_BLEND | IGLU_GLSL_DEPTH_TEST );
	fullScreen["flipX"] = 0.0;
	fullScreen["flipY"] = 0.0;

	// Create a vertex array storing our full-screen quad
	quadGeom = new IGLUVertexArray();
	quadGeom->SetVertexArray( sizeof(iglu_FullScreenQuadData), iglu_FullScreenQuadData, iglu::IGLU_STATIC | iglu::IGLU_DRAW );
	quadGeom->EnableAttribute( IGLU_ATTRIB_VERTEX,   3, GL_FLOAT, 8 * sizeof(float), BUFFER_OFFSET( 2 * sizeof(float) ) );
	quadGeom->EnableAttribute( IGLU_ATTRIB_TEXCOORD, 2, GL_FLOAT, 8 * sizeof(float), BUFFER_OFFSET( 0 ) );
	quadGeom->EnableAttribute( IGLU_ATTRIB_NORMAL,   3, GL_FLOAT, 8 * sizeof(float), BUFFER_OFFSET( 5 * sizeof(float) ) );

	iglu_FullScreenInitialized = true;
	return true;
}

int iglu::IGLUDraw::Fullscreen( const IGLUTexture &texture, uint flags )
{
	// Check if the shader for drawing fullscreen stuff has been compiled
	if (!iglu_FullScreenInitialized)
		if (!InitializeFullScreen())
			return IGLU_ERROR_INITIALIZING_DRAW_ROUTINES;

	// Draw a full-screen quad with the copy-texture shader
	fullScreen->Enable();
	fullScreen["copyTex"] = texture;
	fullScreen["flipX"]   = ( flags & IGLU_DRAW_FLIP_X ? 1.0 : 0.0 );
	fullScreen["flipY"]   = ( flags & IGLU_DRAW_FLIP_Y ? 1.0 : 0.0 );
	quadGeom->DrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	fullScreen->Disable();

	return IGLU_NO_ERROR;
}

int iglu::IGLUDraw::Fullscreen( IGLUTexture *texture, uint flags )
{
	// Check if the shader for drawing fullscreen stuff has been compiled
	if (!iglu_FullScreenInitialized)
		if (!InitializeFullScreen())
			return IGLU_ERROR_INITIALIZING_DRAW_ROUTINES;

	// Draw a full-screen quad with the copy-texture shader
	fullScreen->Enable();
	fullScreen["copyTex"] = texture;
	fullScreen["flipX"]   = ( flags & IGLU_DRAW_FLIP_X ? 1.0 : 0.0 );
	fullScreen["flipY"]   = ( flags & IGLU_DRAW_FLIP_Y ? 1.0 : 0.0 );
	quadGeom->DrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	fullScreen->Disable();

	return IGLU_NO_ERROR;
}


int iglu::IGLUDraw::Fullscreen( IGLUShaderProgram::Ptr &shader, IGLUTexture *texture, const char *texVar )
{
	// Check if the shader for drawing fullscreen stuff has been compiled
	if (!iglu_FullScreenInitialized)
		if (!InitializeFullScreen())
			return IGLU_ERROR_INITIALIZING_DRAW_ROUTINES;

	// Draw a full-screen quad with the copy-texture shader
	shader->Enable();
	shader[texVar] = texture;
		quadGeom->DrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	shader->Disable();

	return IGLU_NO_ERROR;
}

int IGLUDraw::Fullscreen( IGLUShaderProgram::Ptr &shader, const IGLUTexture &texture, const char *texVar )
{
	// Check if the shader for drawing fullscreen stuff has been compiled
	if (!iglu_FullScreenInitialized)
		if (!InitializeFullScreen())
			return IGLU_ERROR_INITIALIZING_DRAW_ROUTINES;

	// Draw a full-screen quad with the copy-texture shader
	shader->Enable();
	shader[texVar] = texture;
		quadGeom->DrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	shader->Disable();

	return IGLU_NO_ERROR;
}
