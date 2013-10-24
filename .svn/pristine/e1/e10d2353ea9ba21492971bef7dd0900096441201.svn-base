/******************************************************************/
/* igluOBJMaterial.cpp                                            */
/* -----------------------                                        */
/*                                                                */
/* Defines a reader for OBJ material files                        */
/*                                                                */
/* Chris Wyman (10/20/2011)                                       */
/******************************************************************/


#include "iglu.h"

using namespace iglu;

// Declare static members of our IGLUOBJMaterialReader class.

IGLUArray1D<IGLUTexture2D::Ptr>  IGLUOBJMaterialReader::s_matlTexture;
IGLUArray1D<IGLUOBJMaterial *>   IGLUOBJMaterialReader::s_matl;
IGLUTextureBuffer::Ptr           IGLUOBJMaterialReader::s_matlCoefBuf;   
IGLUTexture::Ptr                 IGLUOBJMaterialReader::s_matlTexArray;    


namespace { // anonymous namespace for stuff used inside this file.

// This is essentially the constructor for our material structure.  
//      We're hiding it here, for no particularly good reason.
//      It's somewhat inefficient, but keeps our definition 
//      looking like a struct
void InitializeMaterial( IGLUOBJMaterial *mtl )
{
	if (!mtl) return;
	mtl->m_matlName      = 0;
	mtl->m_ambTexId      = -1;
	mtl->m_difTexId      = -1;
	mtl->m_specTexId     = -1;
	mtl->m_amb           = vec3::Zero();
	mtl->m_dif           = vec3::Zero();
	mtl->m_spec          = vec3::Zero();
	mtl->m_trans         = vec3::Zero();
	mtl->m_dissolve      = 1.0;
	mtl->m_illumModel    = 1;
	mtl->m_reflSharpness = 0;
	mtl->m_shininess     = 1;
}

void AddDefaultMaterial( void )
{
	// Only want to add a default material at the beginning
	if (IGLUOBJMaterialReader::s_matl.Size() > 0)
		return;

	// Create a new material for our default
	IGLUOBJMaterialReader::s_matl.Add( new IGLUOBJMaterial() );
	InitializeMaterial( IGLUOBJMaterialReader::s_matl[0] );
	IGLUOBJMaterialReader::s_matl[0]->m_matlName = strdup( "__DEFAULT__" );	
	IGLUOBJMaterialReader::s_matl[0]->m_amb      = vec3(0.1);
	IGLUOBJMaterialReader::s_matl[0]->m_dif      = vec3(0.9);
}

// Some global variables for this file
IGLUFramebuffer::Ptr    matlArrFBO      = 0;    // A FBO to contain 's_matlTexArray'
IGLUShaderProgram::Ptr  createTexArray  = 0;    // The shader used to create 's_matlTexArray'
IGLUBuffer::Ptr         mtlBuf          = 0;    // A buffer used as the storage for 's_matlCoefBuf'

};  // End: anonymous namespace to hide globals inside this file


IGLUOBJMaterial *IGLUOBJMaterialReader::GetNamedMaterial( char *mtlName )
{
	for (uint i=0; i < s_matl.Size(); i++)
		if (!strcmp(mtlName, s_matl[i]->m_matlName)) 
			return s_matl[i];

	return NULL;
}

int IGLUOBJMaterialReader::GetNamedMaterialId( char *mtlName )
{
	for (uint i=0; i < s_matl.Size(); i++)
		if (!strcmp(mtlName, s_matl[i]->m_matlName)) 
			return int(i);
	return -1;
}

bool IGLUOBJMaterialReader::FinalizeMaterialsForRendering( void )
{
	// Create a Texture2DArray to hold all of our material textures...  We'll create it by rendering into it.
	if (s_matlTexture.Size() > 0)
	{
		s_matlTexArray = new IGLURenderTexture2DArray( 512, 512, s_matlTexture.Size() );
		matlArrFBO = new IGLUFramebuffer();
		matlArrFBO->AttachTexture( IGLU_COLOR0, (IGLURenderTexture *)s_matlTexArray );

		// Load the shader that will create our texture array out of the individual textures.
		createTexArray = new IGLUShaderProgram( "shaders/toArray.vert.glsl", "shaders/toArray.geom.glsl", "shaders/toArray.frag.glsl" );
		createTexArray->SetProgramDisables( IGLU_GLSL_DEPTH_TEST | IGLU_GLSL_BLEND ); 

		// Store our textures into the texture array
		matlArrFBO->Bind();
		for (uint i=0; i<s_matlTexture.Size(); i++)
		{
			createTexArray[ "outputLayer" ] = int(i);
			IGLUDraw::Fullscreen( createTexArray, s_matlTexture[i] );
		}
		matlArrFBO->Unbind();
	}

	// Now create our texture array that stores information about each material
	float *matlArrayData = (float *)malloc( 16 * sizeof(float) * s_matl.Size() );
	for (uint i=0; i<s_matl.Size(); i++)
	{
		float *mtlPtr = &matlArrayData[i*16];
		*mtlPtr++ = s_matl[i]->m_amb.X();
		*mtlPtr++ = s_matl[i]->m_amb.Y();
		*mtlPtr++ = s_matl[i]->m_amb.Z();
		*mtlPtr++ = s_matl[i]->m_idxRefract;
		*mtlPtr++ = s_matl[i]->m_dif.X();
		*mtlPtr++ = s_matl[i]->m_dif.Y();
		*mtlPtr++ = s_matl[i]->m_dif.Z();
		*mtlPtr++ = s_matl[i]->m_dissolve;
		*mtlPtr++ = s_matl[i]->m_spec.X();
		*mtlPtr++ = s_matl[i]->m_spec.Y();
		*mtlPtr++ = s_matl[i]->m_spec.Z();
		*mtlPtr++ = s_matl[i]->m_shininess;
		*mtlPtr++ = s_matl[i]->m_ambTexId;
		*mtlPtr++ = s_matl[i]->m_difTexId;
		*mtlPtr++ = s_matl[i]->m_specTexId;
		*mtlPtr++ = -1.0f;
	}
	mtlBuf = new IGLUBuffer();
	mtlBuf->SetBufferData( IGLUOBJMaterialReader::s_matl.Size() * 16 * sizeof(float), matlArrayData );
	s_matlCoefBuf = new IGLUTextureBuffer();
	s_matlCoefBuf->BindBuffer( GL_RGBA32F, mtlBuf );
	free( matlArrayData );

	return true;
}


IGLUOBJMaterialReader::IGLUOBJMaterialReader( char *filename ) : 
	IGLUFileParser( filename )
{
	// Some vars needed during our loop through the material file
	char keyword[64], token[512];
	int curMtl = -1;

	// If this is our first material file, make sure to add a default material
	if (s_matl.Size()<=0) 
		AddDefaultMaterial();

	// Loop through the lines in the material file...
	while ( this->ReadNextLine() != NULL )
	{
		// Each material line starts with a keyword/keyletter
		this->GetLowerCaseToken( keyword );

		// Are we starting a new material?
		if (!strcmp("newmtl", keyword ))
		{
			// Create a new material
			curMtl = s_matl.Add( new IGLUOBJMaterial() );
			InitializeMaterial( s_matl[curMtl] );

			// Grab the material name.
			this->GetToken( token );
			s_matl[curMtl]->m_matlName = strdup(token);
		}
		// We found the ambient term for the current material
		else if (!strcmp("ka", keyword ) && curMtl>=0 )
			s_matl[curMtl]->m_amb = this->GetVec3();
		// We found the diffuse term for the current material
		else if (!strcmp("kd", keyword ) && curMtl>=0 )
			s_matl[curMtl]->m_dif = this->GetVec3();
		// We found the specular term for the current material
		else if (!strcmp("ks", keyword ) && curMtl>=0 )
			s_matl[curMtl]->m_spec = this->GetVec3();
		// We found the transmissive color term for the current material
		else if (!strcmp("tf", keyword ) && curMtl>=0 )
			s_matl[curMtl]->m_trans = this->GetVec3();
		// We found the specular shininess for the current material
		else if (!strcmp("ns", keyword ) && curMtl>=0 )
			s_matl[curMtl]->m_shininess = this->GetFloat();
		// We found the surface alpha for the current material
		else if ((!strcmp("tr", keyword ) || !strcmp("d", keyword )) && curMtl>=0 )
			s_matl[curMtl]->m_dissolve = this->GetFloat();
		// We found the index of refraction for the current material
		else if (!strcmp("ni", keyword ) && curMtl>=0 )
			s_matl[curMtl]->m_idxRefract = this->GetFloat();
		// We found the illumination model for the current material
		else if (!strcmp("illum", keyword ) && curMtl>=0 )
			s_matl[curMtl]->m_idxRefract = this->GetInteger();
		// We found the texture for the ambient component of the current texture
		else if (!strcmp("map_ka", keyword ) && curMtl>=0 )
		{
			// (TODO) Technically, this may not work...  .mtl files can contain
			//        options between the keyword and filename.  We're ignoring 
			//        that possibility here...
			char *texFile = this->GetCurrentLinePointer();
			s_matl[curMtl]->m_ambTexId = s_matlTexture.Add( new IGLUTexture2D( texFile ) );
		}
		// We found the texture for the diffuse component of the current texture
		else if (!strcmp("map_kd", keyword ) && curMtl>=0 )
		{
			// (TODO) Technically, this may not work...  .mtl files can contain
			//        options between the keyword and filename.  We're ignoring 
			//        that possibility here...
			char *texFile = this->GetCurrentLinePointer();
			s_matl[curMtl]->m_difTexId = s_matlTexture.Add( new IGLUTexture2D( texFile ) );
		}
		// We found the texture for the ambient component of the current texture
		else if (!strcmp("map_ks", keyword ) && curMtl>=0 )
		{
			// (TODO) Technically, this may not work...  .mtl files can contain
			//        options between the keyword and filename.  We're ignoring 
			//        that possibility here...
			char *texFile = this->GetCurrentLinePointer();
			s_matl[curMtl]->m_specTexId = s_matlTexture.Add( new IGLUTexture2D( texFile ) );
		}


		
	}
}


