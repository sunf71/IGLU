/**************************************************************************
** igluShaderProgram.cpp                                                 **
** -----------------                                                     **
**                                                                       **
** This header includes a class that can be used as an encapsulation for **
**   igluGLSLPrograms, that provides slightly nicer interfaces for       **
**   setting uniform variables, etc.  It builds on a GLSLProgram, and is **
**   essentially a bit of a glorified smart pointer.                     **
**                                                                       **
** Please beware that we haven't eliminated memory leakage here, if you  **
**   pass these around a lot.  We use these in a global structure and    **
**   access them directly.  If you start allocating copies of these on   **
**   the heap (e.g., using new()), you may start leaking a few bytes.    **
**   This should be fixable by turning this class into a true smart ptr  **
**   and keeping a reference count which can be decremented in the       **
**   destructor.                                                         **
**                                                                       **
** NOTE: This class is NOT thread safe.  Do not share among threads!     **
**                                                                       **
** Chris Wyman (06/14/2011)                                              **
**************************************************************************/
/*
add support for tesselation shader
Sun Feng(10/24/2013)
add support for transform feedback without fragment shader
Sun Feng(10/31/2013)
*/
#include "iglu.h"

using namespace iglu;

#define IGLU_ON_SHADER_START 0
#define IGLU_ON_SHADER_END   1


IGLUShaderProgram::IGLUShaderProgram() : 
	m_currentlyEnabled(false), m_verbose(true),
	m_invokeStateMask(0x0), m_invokeStateDisable(0x0), m_invokeStateEnable(0x0),
	m_isLinked(false), m_programID(0)
{ 
	// Initialize our semantic names (to NULL)
	for (int i=0; i<___IGLU_SEM_TYPE_COUNT; i++)
		m_semanticNames[i] = 0;
}
	

IGLUShaderProgram::IGLUShaderProgram( const char *vShaderFile, const char *fShaderFile ) : 
	m_currentlyEnabled(false), m_verbose(true),
	m_invokeStateMask(0x0), m_invokeStateDisable(0x0), m_invokeStateEnable(0x0),
	m_isLinked(false), m_programID(0)
{ 
	// Create a program
	m_programID = glCreateProgram();
	Load( vShaderFile, fShaderFile );
}


IGLUShaderProgram::IGLUShaderProgram( const char *vShaderFile, const char *gShaderFile, const char *fShaderFile ) : 
	m_currentlyEnabled(false), m_verbose(true),
	m_invokeStateMask(0x0), m_invokeStateDisable(0x0), m_invokeStateEnable(0x0),
	m_isLinked(false), m_programID(0)
{ 
	// Create a program
	m_programID = glCreateProgram();
	Load( vShaderFile, gShaderFile, fShaderFile );
}

//added by sunf to support tesselation shader
IGLUShaderProgram::IGLUShaderProgram( const char *vShaderFile, const char *tcShaderFile, const char * teShaderFile, const char *gShaderFile, const char *fShaderFile ) : 
	m_currentlyEnabled(false), m_verbose(true),
	m_invokeStateMask(0x0), m_invokeStateDisable(0x0), m_invokeStateEnable(0x0),
	m_isLinked(false), m_programID(0)
{ 
	// Create a program
	m_programID = glCreateProgram();
	Load( vShaderFile, tcShaderFile, teShaderFile, gShaderFile, fShaderFile );
}
IGLUShaderProgram::IGLUShaderProgram( const char *vShaderFile, const char *tcShaderFile, const char * teShaderFile,  const char *fShaderFile ) : 
	m_currentlyEnabled(false), m_verbose(true),
	m_invokeStateMask(0x0), m_invokeStateDisable(0x0), m_invokeStateEnable(0x0),
	m_isLinked(false), m_programID(0)
{ 
	// Create a program
	m_programID = glCreateProgram();
	Load( vShaderFile, tcShaderFile, teShaderFile,  fShaderFile );
}
IGLUShaderProgram::~IGLUShaderProgram()                                   
{
	for ( uint i=0; i < m_activeTex.Size(); i++ )
		delete m_activeTex[i];
}

void IGLUShaderProgram::Load( const char *vShaderFile, const char *fShaderFile )
{ 
	// The default constructor cannot create a program ID, so check here...
	if (!m_programID) m_programID = glCreateProgram();

	// If we already loaded shader stages, get rid of them...
	if (m_shaderStages.Size() > 0) m_shaderStages.SetSize( 0 );

	// Load our shader stages
	m_shaderStages.Add( new IGLUShaderStage( IGLU_SHADER_FROM_FILE | IGLU_SHADER_VERTEX, vShaderFile ) );
	m_shaderStages.Add( new IGLUShaderStage( IGLU_SHADER_FROM_FILE | IGLU_SHADER_FRAGMENT, fShaderFile ) );

	// Attach our shader stages & see if they requested any special GL state when invoked
	for (uint i=0; i<m_shaderStages.Size(); i++)
	{
		glAttachShader( m_programID, m_shaderStages[i]->GetShaderID() );
		SetProgramEnables( m_shaderStages[i]->GetShaderRequestedGLEnables() );
		SetProgramDisables( m_shaderStages[i]->GetShaderRequestedGLDisables() );
	}

	// Link the shader stages together
	Link();

	// Copy in IGLU-specific semantics, so we know about them.
	CopySemanticNames();
}

//modified by sunf to support program with only vertex shader and geometry shader used in transform feedback
void IGLUShaderProgram::Load( const char *vShaderFile, const char *gShaderFile, const char *fShaderFile )  
{ 
	// The default constructor cannot create a program ID, so check here...
	if (!m_programID) m_programID = glCreateProgram();

	// If we already loaded shader stages, get rid of them...
	if (m_shaderStages.Size() > 0) m_shaderStages.SetSize( 0 );

	// Load our shader stages
	m_shaderStages.Add( new IGLUShaderStage( IGLU_SHADER_FROM_FILE | IGLU_SHADER_VERTEX, vShaderFile ) );
	m_shaderStages.Add( new IGLUShaderStage( IGLU_SHADER_FROM_FILE | IGLU_SHADER_GEOMETRY, gShaderFile ) );
	if (fShaderFile != NULL)
	{
		m_shaderStages.Add( new IGLUShaderStage( IGLU_SHADER_FROM_FILE | IGLU_SHADER_FRAGMENT, fShaderFile ) );
	}

	// Attach our shader stages & see if they requested any special GL state when invoked
	for (uint i=0; i<m_shaderStages.Size(); i++)
	{
		glAttachShader( m_programID, m_shaderStages[i]->GetShaderID() );
		SetProgramEnables( m_shaderStages[i]->GetShaderRequestedGLEnables() );
		SetProgramDisables( m_shaderStages[i]->GetShaderRequestedGLDisables() );
	}

	// Link the shader stages together
	Link();

	// Copy in IGLU-specific semantics, so we know about them.
	CopySemanticNames();
}

void IGLUShaderProgram::Load( const char *vShaderFile, const char *tcShaderFile, const char * teShaderFile,  const char *gShaderFile, const char *fShaderFile )  
{ 
	// The default constructor cannot create a program ID, so check here...
	if (!m_programID) m_programID = glCreateProgram();

	// If we already loaded shader stages, get rid of them...
	if (m_shaderStages.Size() > 0) m_shaderStages.SetSize( 0 );

	// Load our shader stages
	m_shaderStages.Add( new IGLUShaderStage( IGLU_SHADER_FROM_FILE | IGLU_SHADER_VERTEX, vShaderFile ) );
	m_shaderStages.Add( new IGLUShaderStage( IGLU_SHADER_FROM_FILE | IGLU_SHADER_TESS_CONTROL, tcShaderFile ) );
	m_shaderStages.Add( new IGLUShaderStage( IGLU_SHADER_FROM_FILE | IGLU_SHADER_TESS_EVAL, teShaderFile ) );
	m_shaderStages.Add( new IGLUShaderStage( IGLU_SHADER_FROM_FILE | IGLU_SHADER_GEOMETRY, gShaderFile ) );
	m_shaderStages.Add( new IGLUShaderStage( IGLU_SHADER_FROM_FILE | IGLU_SHADER_FRAGMENT, fShaderFile ) );

	// Attach our shader stages & see if they requested any special GL state when invoked
	for (uint i=0; i<m_shaderStages.Size(); i++)
	{
		glAttachShader( m_programID, m_shaderStages[i]->GetShaderID() );
		SetProgramEnables( m_shaderStages[i]->GetShaderRequestedGLEnables() );
		SetProgramDisables( m_shaderStages[i]->GetShaderRequestedGLDisables() );
	}

	// Link the shader stages together
	Link();

	// Copy in IGLU-specific semantics, so we know about them.
	CopySemanticNames();
}
void IGLUShaderProgram::Load( const char *vShaderFile, const char *tcShaderFile, const char * teShaderFile,  const char *fShaderFile )  
{ 
	// The default constructor cannot create a program ID, so check here...
	if (!m_programID) m_programID = glCreateProgram();

	// If we already loaded shader stages, get rid of them...
	if (m_shaderStages.Size() > 0) m_shaderStages.SetSize( 0 );

	// Load our shader stages
	m_shaderStages.Add( new IGLUShaderStage( IGLU_SHADER_FROM_FILE | IGLU_SHADER_VERTEX, vShaderFile ) );
	m_shaderStages.Add( new IGLUShaderStage( IGLU_SHADER_FROM_FILE | IGLU_SHADER_TESS_CONTROL, tcShaderFile ) );
	m_shaderStages.Add( new IGLUShaderStage( IGLU_SHADER_FROM_FILE | IGLU_SHADER_TESS_EVAL, teShaderFile ) );
	m_shaderStages.Add( new IGLUShaderStage( IGLU_SHADER_FROM_FILE | IGLU_SHADER_FRAGMENT, fShaderFile ) );

	// Attach our shader stages & see if they requested any special GL state when invoked
	for (uint i=0; i<m_shaderStages.Size(); i++)
	{
		glAttachShader( m_programID, m_shaderStages[i]->GetShaderID() );
		SetProgramEnables( m_shaderStages[i]->GetShaderRequestedGLEnables() );
		SetProgramDisables( m_shaderStages[i]->GetShaderRequestedGLDisables() );
	}

	// Link the shader stages together
	Link();

	// Copy in IGLU-specific semantics, so we know about them.
	CopySemanticNames();
}
void IGLUShaderProgram::CreateFromString( const char *vShader, const char *fShader )
{
	// The default constructor cannot create a program ID, so check here...
	if (!m_programID) m_programID = glCreateProgram();

	// If we already loaded shader stages, get rid of them...
	if (m_shaderStages.Size() > 0) m_shaderStages.SetSize( 0 );

	// Load our shader stages
	m_shaderStages.Add( new IGLUShaderStage( IGLU_SHADER_FROM_STRING | IGLU_SHADER_VERTEX, vShader ) );
	m_shaderStages.Add( new IGLUShaderStage( IGLU_SHADER_FROM_STRING | IGLU_SHADER_FRAGMENT, fShader ) );

	// Attach our shader stages & see if they requested any special GL state when invoked
	for (uint i=0; i<m_shaderStages.Size(); i++)
	{
		glAttachShader( m_programID, m_shaderStages[i]->GetShaderID() );
		SetProgramEnables( m_shaderStages[i]->GetShaderRequestedGLEnables() );
		SetProgramDisables( m_shaderStages[i]->GetShaderRequestedGLDisables() );
	}

	// Link the shader stages together
	Link();

	// Copy in IGLU-specific semantics, so we know about them.
	CopySemanticNames();
}

// Reload any shaders.
bool IGLUShaderProgram::Reload( void )
{
	// Disable the current shader
	if (m_currentlyEnabled) 
		Disable();

	// Detach all the current shaders
	for (uint i=0; i<m_shaderStages.Size(); i++)
		glDetachShader( m_programID, m_shaderStages[i]->GetShaderID() );

	// Reload all the shaders
	for (uint i=0; i<m_shaderStages.Size(); i++)
		m_shaderStages[i]->ReloadShader();

	// Attach our shader stages & see if they requested any special GL state when invoked
	for (uint i=0; i<m_shaderStages.Size(); i++)
	{
		glAttachShader( m_programID, m_shaderStages[i]->GetShaderID() );
		SetProgramEnables( m_shaderStages[i]->GetShaderRequestedGLEnables() );
		SetProgramDisables( m_shaderStages[i]->GetShaderRequestedGLDisables() );
	}

	// Link the shader stages together
	Link();

	// Copy in IGLU-specific semantics, so we know about them.
	CopySemanticNames();

	return false;
}

void IGLUShaderProgram::CreateFromString( const char *vShader, const char *gShader, const char *fShader )
{
	// The default constructor cannot create a program ID, so check here...
	if (!m_programID) m_programID = glCreateProgram();

	// If we already loaded shader stages, get rid of them...
	if (m_shaderStages.Size() > 0) m_shaderStages.SetSize( 0 );

	// Load our shader stages
	m_shaderStages.Add( new IGLUShaderStage( IGLU_SHADER_FROM_STRING | IGLU_SHADER_VERTEX, vShader ) );
	m_shaderStages.Add( new IGLUShaderStage( IGLU_SHADER_FROM_STRING | IGLU_SHADER_GEOMETRY, gShader ) );
	m_shaderStages.Add( new IGLUShaderStage( IGLU_SHADER_FROM_STRING | IGLU_SHADER_FRAGMENT, fShader ) );

	// Attach our shader stages & see if they requested any special GL state when invoked
	for (uint i=0; i<m_shaderStages.Size(); i++)
	{
		glAttachShader( m_programID, m_shaderStages[i]->GetShaderID() );
		SetProgramEnables( m_shaderStages[i]->GetShaderRequestedGLEnables() );
		SetProgramDisables( m_shaderStages[i]->GetShaderRequestedGLDisables() );
	}

	// Link the shader stages together
	Link();

	// Copy in IGLU-specific semantics, so we know about them.
	CopySemanticNames();
}

bool IGLUShaderProgram::Enable( void )
{
	PushProgram();

	// Enable any textures we know about
	for (uint i=0; i<m_activeTex.Size(); i++){
		m_activeTex[i]->m_tex->Bind( GL_TEXTURE0 + m_activeTex[i]->m_texUnit );
	
		// If we're assigning to a shadow sampler, we need to set the compare mode
		if (m_activeTexFlags[i] & IGLU_SHADER_TEX_IS_SHADOW)
			m_activeTex[i]->m_tex->SetTextureCompareMode( IGLU_TEX_COMPARE_REF );
	}
	
	//Enable any images
	for(uint i=0; i<m_activeImage.Size(); i++)		
		m_activeImage[i]->m_tex->BindToImageUnit( m_activeImage[i]->m_texUnit );

	m_currentlyEnabled = true;
	return true;
}

bool IGLUShaderProgram::Disable( void )
{
	// Disable any textures we know about
	for (uint i=0; i<m_activeTex.Size(); i++)
	{
		// If we're unassigning a shadow sampler, we need to set the compare mode back to none,
		//    or other uses of this texture may have unexpected behavior.
		if (m_activeTexFlags[i] & IGLU_SHADER_TEX_IS_SHADOW)
		{
			glActiveTexture( GL_TEXTURE0 + m_activeTex[i]->m_texUnit );
			m_activeTex[i]->m_tex->SetTextureCompareMode( IGLU_TEX_COMPARE_NONE );
		}

		// Unbind
		m_activeTex[i]->m_tex->Unbind( GL_TEXTURE0 + m_activeTex[i]->m_texUnit );
	}

	//Disable any images
	for(uint i=0; i<m_activeImage.Size(); i++)
		m_activeImage[i]->m_tex->UnbindFromImageUnit( m_activeImage[i]->m_texUnit );

	// Ok, now actually disable the shader
	m_currentlyEnabled = false;
	PopProgram();
	return true;
}


IGLUShaderVariable &IGLUShaderProgram::operator[] ( const char *varName )                      
{ 
	m_tmpVar = IGLUShaderVariable( m_programID, varName, this ); 
	return m_tmpVar; 
}


void iglu::IGLUShaderProgram::PushProgram( void )
{
	glGetIntegerv( GL_CURRENT_PROGRAM, &m_prevProgram );
	if (m_prevProgram != m_programID)
	{
		GetCurrentEnabledState();
		SetInvocationState( IGLU_ON_SHADER_START );
		glUseProgram( m_programID );
	}
}


void iglu::IGLUShaderProgram::PopProgram( void )
{
	if (m_prevProgram != m_programID)
	{
		glUseProgram( m_prevProgram );
		SetInvocationState( IGLU_ON_SHADER_END );
	}
}

void IGLUShaderProgram::SetProgramEnables ( uint stateFlags )
{
	// Set these state flags to be enabled
	m_invokeStateEnable |= stateFlags;

	// Label these state at ones that need changing on Enable()/Disable()
	m_invokeStateMask   |= stateFlags;
}

void IGLUShaderProgram::SetProgramDisables( uint stateFlags )
{
	// Set these state flags to be enabled
	m_invokeStateDisable |= stateFlags;

	// Label these state at ones that need changing on Enable()/Disable()
	m_invokeStateMask   |= stateFlags;
}

void IGLUShaderProgram::GetCurrentEnabledState( void )
{
	// Set the default state on everything to DISABLED.
	m_pushedState = 0x0;

	// If we either enable or disable various states upon program initialization,
	//    go ahead and store copies of our state so we can restore it.  To reduce
	//    cost, we do not store all state, only those bits the user asked us to.
	if ( m_invokeStateMask & IGLU_GLSL_BLEND )             
		m_pushedState |= glIsEnabled( GL_BLEND ) ? IGLU_GLSL_BLEND : 0;
	if ( m_invokeStateMask & IGLU_GLSL_DEPTH_TEST )        
		m_pushedState |= glIsEnabled( GL_DEPTH_TEST ) ? IGLU_GLSL_DEPTH_TEST : 0;
	if ( m_invokeStateMask & IGLU_GLSL_STENCIL_TEST )      
		m_pushedState |= glIsEnabled( GL_STENCIL_TEST ) ? IGLU_GLSL_STENCIL_TEST : 0;
	if ( m_invokeStateMask & IGLU_GLSL_SCISSOR_TEST )      
		m_pushedState |= glIsEnabled( GL_SCISSOR_TEST ) ? IGLU_GLSL_SCISSOR_TEST : 0;
	if ( m_invokeStateMask & IGLU_GLSL_CULL_FACE )        
		m_pushedState |= glIsEnabled( GL_CULL_FACE ) ? IGLU_GLSL_CULL_FACE : 0;
	if ( m_invokeStateMask & IGLU_GLSL_VARY_POINT_SIZE )   
		m_pushedState |= glIsEnabled( GL_VERTEX_PROGRAM_POINT_SIZE ) ? IGLU_GLSL_VARY_POINT_SIZE : 0;
	if ( m_invokeStateMask & IGLU_GLSL_RASTERIZE_DISCARD ) 
		m_pushedState |= glIsEnabled( GL_RASTERIZER_DISCARD ) ? IGLU_GLSL_RASTERIZE_DISCARD : 0;
	if ( m_invokeStateMask & IGLU_GLSL_LINE_SMOOTH )       
		m_pushedState |= glIsEnabled( GL_LINE_SMOOTH ) ? IGLU_GLSL_LINE_SMOOTH : 0;
	if ( m_invokeStateMask & IGLU_GLSL_MULTISAMPLE )       
		m_pushedState |= glIsEnabled( GL_MULTISAMPLE ) ? IGLU_GLSL_MULTISAMPLE : 0;
	if ( m_invokeStateMask & IGLU_GLSL_LOGIC_OP )          
		m_pushedState |= glIsEnabled( GL_LOGIC_OP ) ? IGLU_GLSL_LOGIC_OP : 0;
	if ( m_invokeStateMask & IGLU_GLSL_COLOR_LOGIC_OP )    
		m_pushedState |= glIsEnabled( GL_COLOR_LOGIC_OP ) ? IGLU_GLSL_COLOR_LOGIC_OP : 0;
	if ( m_invokeStateMask & IGLU_GLSL_SAMPLE_SHADING )    
		m_pushedState |= glIsEnabled( GL_SAMPLE_SHADING ) ? IGLU_GLSL_SAMPLE_SHADING : 0;
	if ( m_invokeStateMask & IGLU_GLSL_SAMPLE_MASK )    
		m_pushedState |= glIsEnabled( GL_SAMPLE_MASK ) ? IGLU_GLSL_SAMPLE_MASK : 0;

	
}

bool IGLUShaderProgram::IsStateEnabled( uint bitVector, uint stateFlag )
{
	return (m_invokeStateMask & stateFlag) && (bitVector & stateFlag);
}

bool IGLUShaderProgram::IsStateDisabled( uint bitVector, uint stateFlag )
{
	return (m_invokeStateMask & stateFlag) && (bitVector & stateFlag);
}

void IGLUShaderProgram::SetInvocationState( int startEnd )
{
	// If we're starting to use our shader, set the invocation state that the user
	//    specified using the class methods SetProgramEnables() and SetProgramDisables().
	//    If we're disabling the shader, restore the pushed glEnable() state we found 
	//    upon shader invocation
	uint enState  = ( startEnd == IGLU_ON_SHADER_START ? m_invokeStateEnable : m_pushedState );
	uint disState = ( startEnd == IGLU_ON_SHADER_START ? m_invokeStateDisable : (~m_pushedState) );

	if ( IsStateEnabled(enState,IGLU_GLSL_BLEND) )             glEnable( GL_BLEND );
	if ( IsStateEnabled(enState,IGLU_GLSL_DEPTH_TEST) )        glEnable( GL_DEPTH_TEST );
	if ( IsStateEnabled(enState,IGLU_GLSL_STENCIL_TEST) )      glEnable( GL_STENCIL_TEST );
	if ( IsStateEnabled(enState,IGLU_GLSL_SCISSOR_TEST) )      glEnable( GL_SCISSOR_TEST );
	if ( IsStateEnabled(enState,IGLU_GLSL_CULL_FACE) )         glEnable( GL_CULL_FACE );
	if ( IsStateEnabled(enState,IGLU_GLSL_VARY_POINT_SIZE) )   glEnable( GL_VERTEX_PROGRAM_POINT_SIZE );
	if ( IsStateEnabled(enState,IGLU_GLSL_RASTERIZE_DISCARD) ) glEnable( GL_RASTERIZER_DISCARD );
	if ( IsStateEnabled(enState,IGLU_GLSL_LINE_SMOOTH) )       glEnable( GL_LINE_SMOOTH );
	if ( IsStateEnabled(enState,IGLU_GLSL_MULTISAMPLE) )       glEnable( GL_MULTISAMPLE );
	if ( IsStateEnabled(enState,IGLU_GLSL_LOGIC_OP) )          glEnable( GL_LOGIC_OP );
	if ( IsStateEnabled(enState,IGLU_GLSL_COLOR_LOGIC_OP) )    glEnable( GL_COLOR_LOGIC_OP );
	if ( IsStateEnabled(enState,IGLU_GLSL_SAMPLE_SHADING) )    glEnable( GL_SAMPLE_SHADING );
	if ( IsStateEnabled(enState,IGLU_GLSL_SAMPLE_MASK) )       glEnable( GL_SAMPLE_MASK );

	if ( IsStateDisabled(disState,IGLU_GLSL_BLEND) )             glDisable( GL_BLEND );
	if ( IsStateDisabled(disState,IGLU_GLSL_DEPTH_TEST) )        glDisable( GL_DEPTH_TEST );
	if ( IsStateDisabled(disState,IGLU_GLSL_STENCIL_TEST) )      glDisable( GL_STENCIL_TEST );
	if ( IsStateDisabled(disState,IGLU_GLSL_SCISSOR_TEST) )      glDisable( GL_SCISSOR_TEST );
	if ( IsStateDisabled(disState,IGLU_GLSL_CULL_FACE) )         glDisable( GL_CULL_FACE );
	if ( IsStateDisabled(disState,IGLU_GLSL_VARY_POINT_SIZE) )   glDisable( GL_VERTEX_PROGRAM_POINT_SIZE );
	if ( IsStateDisabled(disState,IGLU_GLSL_RASTERIZE_DISCARD) ) glDisable( GL_RASTERIZER_DISCARD );
	if ( IsStateDisabled(disState,IGLU_GLSL_LINE_SMOOTH) )       glDisable( GL_LINE_SMOOTH );
	if ( IsStateDisabled(disState,IGLU_GLSL_MULTISAMPLE) )       glDisable( GL_MULTISAMPLE );
	if ( IsStateDisabled(disState,IGLU_GLSL_LOGIC_OP) )          glDisable( GL_LOGIC_OP );
	if ( IsStateDisabled(disState,IGLU_GLSL_COLOR_LOGIC_OP) )    glDisable( GL_COLOR_LOGIC_OP );
	if ( IsStateDisabled(disState,IGLU_GLSL_SAMPLE_SHADING) )    glDisable( GL_SAMPLE_SHADING );
	if ( IsStateDisabled(disState,IGLU_GLSL_SAMPLE_MASK) )       glDisable( GL_SAMPLE_MASK );
}

// Relinks the program and checks for any errors.
int IGLUShaderProgram::Link( void )
{
	GLint linked=0;
	glLinkProgram( m_programID );
	glGetProgramiv( m_programID, GL_LINK_STATUS, &linked);
	m_isLinked = (linked != 0);
	if (!linked) return PrintLinkerError();
	return IGLU_NO_ERROR;
}


// Grabs the shader log (e.g., compiler errors) for a specified shader. 
//     (The filename is just there to be pretty)
int IGLUShaderProgram::PrintLinkerError( void )
{
	if (!m_verbose) return IGLU_ERROR_GLSL_LINK_FAILED;

	char buf[4096];
	GLsizei theLen;
	fprintf(stderr, "-------------------------------------------------------------------------------\n");
	fprintf(stderr, "*** Error!  GLSL failed to link....\n" );
	fprintf(stderr, "-------------------------------------------------------------------------------\n");
	glGetProgramInfoLog( m_programID, 4095, &theLen, buf );
	fprintf(stderr, "%s\n\n", buf);

	return IGLU_ERROR_GLSL_LINK_FAILED;
}

void IGLUShaderProgram::CopySemanticNames( void )
{
	// Copy semantic names from the shader stages.  If there are collisions (i.e., multiple GLSL shader
	//    stages that define the same names) print a warning, and use the name from the first shader that
	//    defined it.  Collisions *should* not be a problem, since "in" semantic names only make sense 
	//    in the vertex shader and "out" semantic names only make sense in the fragment shader....
	for (int i=0; i<___IGLU_SEM_TYPE_COUNT; i++)
	{
		m_semanticNames[i] = 0;
		const char *ptr = 0;
		for (uint j=0; j<m_shaderStages.Size(); j++)
		{
			const char *curName = m_shaderStages[j]->GetSemanticVariableName(IGLUSemanticType(i));
			if (!ptr && curName)
				ptr = m_semanticNames[i] = curName;
			else if (ptr && curName)
				printf("*** Warning:  Multiple shader stages define the same IGLU semantic type!\n");
		}
	}
}

int IGLUShaderProgram::SetTransformFeedbackVaryings( const char *glslVarName )
{
	glTransformFeedbackVaryings( m_programID, 1, &glslVarName, GL_INTERLEAVED_ATTRIBS );
	return Link();
}

int IGLUShaderProgram::SetTransformFeedbackVaryings( int numOutputs, 
													 const char **glslVarNames, 
													 GLenum bufferMode )
{
	glTransformFeedbackVaryings( m_programID, numOutputs, glslVarNames, bufferMode );
	return Link();
}

