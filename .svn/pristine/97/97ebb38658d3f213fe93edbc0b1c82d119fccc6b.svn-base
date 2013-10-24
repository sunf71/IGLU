/***************************************************************************/
/* igluDrawTexture.cpp                                                     */
/* -------------------                                                     */
/*                                                                         */
/* Draw a texture on screen in an easy way.                                */
/*                                                                         */
/* Chris Wyman (10/13/2011)                                                */
/***************************************************************************/


#include "iglu.h"

using namespace iglu;

namespace {  // anonymous namespace to hide globals inside this file

static char iglu_DrawTextureVS[] = {
	"#version 400\n"
	"layout(location = 0) in vec3 vertex;\n"
	"layout(location = 2) in vec2 texcoord;\n"
	"uniform vec2 ll, ur;\n"
	"out vec2 fragTexCoord;\n"
	"void main( void ) {\n"
	"    vec3 pos = vec3( vertex.x < 0 ? ll.x : ur.x,\n"
	"                     vertex.y < 0 ? ll.y : ur.y,\n"
	"                     vertex.z );\n"
	"    gl_Position = vec4( pos, 1.0f );\n"
	"    fragTexCoord = texcoord.xy;\n"
	"}\n"
};

static char iglu_DrawTextureFS[] = {
	"#version 400\n"
	"uniform sampler2D copyTex;\n"
	"uniform float flipX, flipY;\n"
	"uniform float blackAsTransp;\n"
	"uniform float simpleBrighten;\n"
	"in vec2 fragTexCoord;\n"
	"out vec4 result;\n"
	"void main( void ) {\n"
	"    bvec2 flip = bvec2( (flipX > 0.5), (flipY > 0.5) );"
	"    vec4 texColor = texture( copyTex, vec2( flip.x ? 1.0-fragTexCoord.x : fragTexCoord.x,\n"
	"                                            flip.y ? 1.0-fragTexCoord.y : fragTexCoord.y ) );\n"
	"    bool makeTransp = (dot(texColor.xyz,texColor.xyz)<0.001) && (blackAsTransp>0.5f);\n"
	"    vec3 bright     = 0.25*(vec3(1,1,1)-texColor.xyz)+texColor.xyz;\n"
	"    result = vec4( (simpleBrighten>0.5f?bright:texColor.xyz),\n"
	"                   (makeTransp?0.0f:texColor.w) );\n"
	"}\n"
};

static float iglu_TexQuadData[] = {
	0, 0, -1, -1, 0,   // Lower-left  vertex, (u,v) = (0,0)
	1, 0, 1, -1, 0,    // Lower-right vertex, (u,v) = (1,0)
	0, 1, -1, 1, 0,    // Upper-left  vertex, (u,v) = (0,1)
	1, 1, 1, 1, 0,     // Upper-right vertex, (u,v) = (1,1)
};

bool iglu_TexInitialized          = false;
IGLUShaderProgram::Ptr tex        = 0; 
IGLUVertexArray::Ptr   quadGeom   = 0;

//} // End anonymous namespace for globals inside this file...

static bool InitializeTexture( void )
{
	// Make sure GLEW has been initialized
	if (!glGetAttribLocation) return false;

	// Create our full-screen shader program.
	tex = new IGLUShaderProgram();
	tex->CreateFromString( iglu_DrawTextureVS, iglu_DrawTextureFS );
	tex->SetProgramEnables( IGLU_GLSL_BLEND );
	tex->SetProgramDisables( IGLU_GLSL_DEPTH_TEST );

	// Create a vertex array storing our full-screen quad
	quadGeom = new IGLUVertexArray();
	quadGeom->SetVertexArray( sizeof(iglu_TexQuadData), iglu_TexQuadData, iglu::IGLU_STATIC | iglu::IGLU_DRAW );
	quadGeom->EnableAttribute( IGLU_ATTRIB_VERTEX, 3, GL_FLOAT, 5 * sizeof(float), BUFFER_OFFSET( 2 * sizeof(float) ) );
	quadGeom->EnableAttribute( IGLU_ATTRIB_TEXCOORD, 2, GL_FLOAT, 5 * sizeof(float), BUFFER_OFFSET( 0 ) );

	return (iglu_TexInitialized = true);
}

} // End anonymous namespace for globals inside this file...

int iglu::IGLUDraw::Texture( const IGLUTexture &texture, float left, float bottom, float right, float top, uint flags )
{
	// Check if the shader for drawing fullscreen stuff has been compiled
	if (!iglu_TexInitialized)
		if (!InitializeTexture())
			return IGLU_ERROR_INITIALIZING_DRAW_ROUTINES;

	// Draw a full-screen quad with the copy-texture shader
	tex->Enable();
	tex["copyTex"]        = texture;
	tex["flipX"]          = ( flags & IGLU_DRAW_FLIP_X ? 1.0 : 0.0 );
	tex["flipY"]          = ( flags & IGLU_DRAW_FLIP_Y ? 1.0 : 0.0 );
	tex["blackAsTransp"]  = ( flags & IGLU_DRAW_TEX_BLACK_AS_TRANSPARENT ? 1.0f : 0.0f );
	tex["simpleBrighten"] = ( flags & IGLU_DRAW_TEX_SIMPLE_BRIGHTEN ? 1.0f : 0.0f );
	tex["ll"]             = vec2( left, bottom );
	tex["ur"]             = vec2( right, top );
	quadGeom->DrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	tex->Disable();

	return IGLU_NO_ERROR;
}

int iglu::IGLUDraw::Texture( IGLUTexture *texture, float left, float bottom, float right, float top, uint flags )
{
	// Check if the shader for drawing fullscreen stuff has been compiled
	if (!iglu_TexInitialized)
		if (!InitializeTexture())
			return IGLU_ERROR_INITIALIZING_DRAW_ROUTINES;

	// Draw a full-screen quad with the copy-texture shader
	tex->Enable();
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	tex["copyTex"] = texture;
	tex["flipX"]   = ( flags & IGLU_DRAW_FLIP_X ? 1.0 : 0.0 );
	tex["flipY"]   = ( flags & IGLU_DRAW_FLIP_Y ? 1.0 : 0.0 );
	tex["blackAsTransp"] = ( flags & IGLU_DRAW_TEX_BLACK_AS_TRANSPARENT ? 1.0 : 0.0 );
	tex["simpleBrighten"] = ( flags & IGLU_DRAW_TEX_SIMPLE_BRIGHTEN ? 1.0f : 0.0f );
	tex["ll"]      = vec2( left, bottom );
	tex["ur"]      = vec2( right, top );
	quadGeom->DrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	tex->Disable();

	return IGLU_NO_ERROR;
}

