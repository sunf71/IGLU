/******************************************************************/
/* igluOBJReader.cpp                                              */
/* -----------------------                                        */
/*                                                                */
/* Defines a reader for OBJ model files and associated materials  */
/*                                                                */
/* Chris Wyman (10/20/2011)                                       */
/******************************************************************/

#include "iglu.h"
#include <float.h>

using namespace iglu;
#define   s_matlTexture  IGLUOBJMaterialReader::s_matlTexture
#define	s_matl  IGLUOBJMaterialReader::s_matl


IGLUOBJReader::IGLUOBJReader( char *filename, int params ) :
	IGLUFileParser( filename ), IGLUModel(), m_vertArr(0),
	m_hasTexCoords(false), m_hasNormals(false), m_hasVertices(false), m_shaderID(0),
	m_hasMatlID(true), m_curMatlId(0), m_curObjectId(0), m_hasObjectID(true)
{
	// Check the parameters
	m_resize        = params & IGLU_OBJ_UNITIZE ? true : false;
	m_center        = params & IGLU_OBJ_CENTER ? true : false;
	m_compactFormat = params & IGLU_OBJ_COMPACT_STORAGE ? true : false;
	m_loadMtlFile   = params & IGLU_OBJ_NO_MATERIALS ? false : true;
	m_assignObjects = params & IGLU_OBJ_NO_OBJECTS ? true : false ;

	// Create the data structure to interface with OpenGL for drawing this object.
	m_vertArr = new IGLUVertexArray();

	// For clarity (later), we have a pointer to the Read_???_Token() method we will
	//    use when reading a facet.  This will be set when we first see a 'f' line.
	FnParserPtr fnFacetParsePtr = NULL;

	// OK, we have a parser.  Now parse through the file
	IGLUOBJTri *curTri = 0;
	char *linePtr = 0, *mtlFilePtr = 0;
	char keyword[64], vertToken[128];
	char fname[256];
	char *currentObj = 0, *currentGrp = 0, *currentMtl = 0;
	int tmpMatlId, tmpObjId;
	
	/*std::vector<IGLUOBJTri *> _objTris;*/
	while ( (linePtr = this->ReadNextLine()) != NULL )
	{
		
		// Each OBJ line starts with a keyword/keyletter
		this->GetLowerCaseToken( keyword );
		
		switch( keyword[0] )
		{
		case 'v': 
			if (keyword[1] == 'n')        // We found a normal!
				m_objNorms.push_back( this->GetVec3() );
			else if (keyword[1] == 't')   // We found a texture coordinate!
				m_objTexCoords.push_back( this->GetVec2() );
			else if (keyword[1] == 0 )    // We found a vertex!
				m_objVerts.push_back( this->GetVec3() );  
			break;
		case 'm': // We found the name of a material file!
			this->GetToken( fname );
			mtlFilePtr = (char *)malloc( strlen(fname)+strlen(fileDirectory)+1 );
			sprintf( mtlFilePtr, "%s%s", fileDirectory, fname );
			m_objMtlFiles.push_back( mtlFilePtr ); 
			if (m_loadMtlFile) { 
				// Should be able to do as a local...  but not inside switch?  Do it the hard way.
				delete( new IGLUOBJMaterialReader( mtlFilePtr ));  // Load it.
			}
			break;
		case 'o': // We found a name for the object following this flag
			this->GetToken( fname );
			currentObj = strdup( fname );
			
			//Check if we have already found this object name
			tmpObjId = GetObjectID(currentObj);
			if(-1 != tmpObjId){
				m_curObjectId = uint(tmpObjId);
			}else{ //It is a new object. push_back it to the list. 
				m_objObjectNames.push_back(currentObj);
				m_curObjectId = m_objObjectNames.size() - 1;
			}
			
			break;
		case 'g': // We found a name for the group following this flag
			this->GetToken( fname );
			currentGrp = strdup( fname );
			break;
		case 'u': // We found the name of the material we'll be using
			this->GetToken( fname );
			currentMtl = strdup( fname );
			tmpMatlId = IGLUOBJMaterialReader::GetNamedMaterialId( fname );
			if (tmpMatlId >= 0)
				m_curMatlId = uint(tmpMatlId);
			break;
		case 's':
			// There's a smoothing command.  We're ignoring these.
			break;
		case 'f': // We found a facet!
			curTri = new IGLUOBJTri( currentMtl, currentGrp, currentObj);
			curTri->matlID = m_curMatlId;
			curTri->objectID = m_curObjectId;
			
			//Ensure the 'f' line has at least three entries. 
			if(!IsValidFLine(&fnFacetParsePtr)){
				continue;
			}


			// There are multiple different formats for 'f' lines.  Decide which it is,
			//    and select the appropriate class member we'll use to read this line.
			SelectReadMethod( &fnFacetParsePtr );
		

			// Read first three set of indices on this line
			((*this).*(fnFacetParsePtr))( curTri, 0, NULL );
			((*this).*(fnFacetParsePtr))( curTri, 1, NULL );
			((*this).*(fnFacetParsePtr))( curTri, 2, NULL );
			m_objTris.push_back( curTri );
			//_objTris.push_back( curTri );
			// Do we have more vertices in this facet?  Check to see if the 
			//    next token is non-empty (if empty the [0]'th char == 0)
			//    If we do, we'll have to triangulate the facet, so make a new tri
			this->GetToken( vertToken );
			while ( vertToken[0] ) 
			{
				curTri = new IGLUOBJTri( currentMtl, currentGrp, currentObj);
				curTri->matlID = m_curMatlId;
				curTri->objectID = m_curObjectId;
				CopyForTriangleFan( curTri );
				//CopyForTriangleFan( _objTris[_objTris.size()-1],curTri );
				((*this).*(fnFacetParsePtr))( curTri, 2, vertToken );
				m_objTris.push_back( curTri );
			    //_objTris.push_back( curTri );
				this->GetToken( vertToken );
			}
			break;

		default:  // We have no clue what to do with this line....
			this->WarningMessage("Found corrupt line in OBJ.  Unknown keyword '%s'", keyword);
		}
	}

	// No need to keep the file hanging around open.
	CloseFile();

	// Create the GPU buffers for this object, so we have them laying around later.
	if (m_compactFormat)
		GetCompactArrayBuffer();
	else
	{
		GetArrayBuffer();
		GetElementArrayBuffer();
	}
}

uint AddGLMmaterial(GLMmaterial* matl)
{


	if (s_matl.Size()<=0) 
		AddDefaultMaterial();
		
	if (matl == NULL)
	{
		return s_matl.Size()-1;
	}
	uint curMtl = s_matl.Add( new IGLUOBJMaterial() );
	InitializeMaterial( s_matl[curMtl] );

	
	s_matl[curMtl]->m_matlName = matl->name;
	for(int i=0; i<3; i++)
	{
		s_matl[curMtl]->m_amb[i] = matl->ambient[i];
		s_matl[curMtl]->m_dif[i] = matl->diffuse[i];
		//s_matl[curMtl]->m_dissolve
		s_matl[curMtl]->m_spec[i] = matl->specular[i];
		s_matl[curMtl]->m_trans[i] = matl->emissive[i];
	}
			
	s_matl[curMtl]->m_shininess = matl->shininess;
	s_matl[curMtl]->m_idxRefract = matl->refraction;
	// We found the illumination model for the current material		
	s_matl[curMtl]->m_illumModel = matl->shader;	
	if ( strlen(matl->ambient_map ) > 0)
	{
		s_matl[curMtl]->m_ambTexId = s_matlTexture.Add( new IGLUTexture2D( matl->ambient_map ) );
	}
	if ( strlen(matl->diffuse_map) > 0)
	{
		s_matl[curMtl]->m_difTexId = s_matlTexture.Add( new IGLUTexture2D( matl->diffuse_map ) );
	}
	if ( strlen(matl->specular_map) >0 )
		s_matl[curMtl]->m_specTexId = s_matlTexture.Add( new IGLUTexture2D( matl->specular_map ) );
		
	return curMtl;
}


IGLUOBJReader::IGLUOBJReader( GLMmodel* model, int params):IGLUFileParser( model->pathname ), IGLUModel(), m_vertArr(0),
	m_hasTexCoords(false), m_hasNormals(false), m_hasVertices(false), m_shaderID(0),
	m_hasMatlID(true), m_curMatlId(0), m_curObjectId(0), m_hasObjectID(true)
{
	// Check the parameters
	m_resize        = params & IGLU_OBJ_UNITIZE ? true : false;
	m_center        = params & IGLU_OBJ_CENTER ? true : false;
	m_compactFormat = params & IGLU_OBJ_COMPACT_STORAGE ? true : false;
	m_loadMtlFile   = params & IGLU_OBJ_NO_MATERIALS ? false : true;
	m_assignObjects = params & IGLU_OBJ_NO_OBJECTS ? true : false ;

	m_hasVertices = model->numvertices > 0  ? true : false;
	m_hasNormals = model->numnormals > 0 ? true : false;
	m_hasTexCoords = model->numtexcoords > 0 ? true : false;
	// Create the data structure to interface with OpenGL for drawing this object.
	m_vertArr = new IGLUVertexArray();

	if (m_resize || m_center)
		glmUnitize(model);
	// Basic geometric definitions
	m_objVerts.reserve(model->numvertices);
	for(int i=0; i<model->numvertices; i++)
	{
		vec3 vertex;
		for(int j=0; j<3; j++)
		{
			vertex[j] = model->vertices[(i+1)*3+j];
		}
		m_objVerts.push_back(vertex);
	}
	m_objNorms.reserve(model->numnormals);
	for(int i=0; i<model->numnormals; i++)
	{
		vec3 normal;
		for(int j=0; j<3; j++)
		{
			normal[j] = model->normals[(i+1)*3+j];
		}
		m_objNorms.push_back(normal);
	}

	m_objTexCoords.reserve(model->numtexcoords);
	for(int i=0; i<model->numtexcoords; i++)
	{
		vec2 tex;
		for(int j=0; j<2; j++)
		{
			tex[j] = model->texcoords[(i+1)*2+j];
		}
		m_objTexCoords.push_back(tex);
	}

	// Basic geometric triangles definitions
	unsigned int triangle_count = 0u;
	unsigned int group_count = 0u;
	m_objTris.reserve(model->numtriangles);
	 for ( GLMgroup* obj_group = model->groups;
        obj_group != 0;
        obj_group = obj_group->next, group_count++ )
	 {
		 unsigned int num_triangles = obj_group->numtriangles;
		 if ( num_triangles == 0 ) continue;


		 //inti mtl
		 uint materialId = AddGLMmaterial(&model->materials[obj_group->material]);

		  for ( unsigned int i = 0; i < obj_group->numtriangles; ++i, ++triangle_count ) 
		  {
			  IGLUOBJTri * tri = new IGLUOBJTri(obj_group->mtlname,obj_group->name,obj_group->name);
			  unsigned int tindex = obj_group->triangles[i];     
			  for(int k=0; k<3; k++)
			  {
				  tri->vIdx[k] = model->triangles[ tindex ].vindices[k] - 1; 
				  tri->nIdx[k] = model->triangles[ tindex ].nindices[k] - 1; 
				  tri->tIdx[k] =  model->triangles[ tindex ].tindices[k] - 1; 
			  }	
			  tri->matlID = materialId;
			  m_objTris.push_back(tri);
		  }
	 }

	if (m_compactFormat)
		GetCompactArrayBuffer();
	else
	{
		GetArrayBuffer();
		GetElementArrayBuffer();
	}
}
IGLUOBJReader::~IGLUOBJReader()
{
	// Free strings contining the mtllib filenames
	for (uint i=0; i<m_objMtlFiles.size(); i++)
		free( m_objMtlFiles[i] );

	// Get rid of our vertex array
	delete m_vertArr;
}

unsigned int IGLUOBJReader::GetVertexIndex( int relativeIdx )
{
	if (relativeIdx == 0)
		this->WarningMessage("Unexpected OBJ vertex index of 0!");
	return relativeIdx > 0 ? relativeIdx-1 : m_objVerts.size()+relativeIdx;
}

unsigned int IGLUOBJReader::GetNormalIndex( int relativeIdx )
{
	if (relativeIdx == 0)
		this->WarningMessage("Unexpected OBJ normal index of 0!");
	return relativeIdx > 0 ? relativeIdx-1 : m_objNorms.size()+relativeIdx;
}

unsigned int IGLUOBJReader::GetTextureIndex( int relativeIdx )
{
	if (relativeIdx == 0)
		this->WarningMessage("Unexpected OBJ texture coord index of 0!");
	return relativeIdx > 0 ? relativeIdx-1 : m_objTexCoords.size()+relativeIdx;
}

int IGLUOBJReader::GetObjectID( char *objName )
{
	//Search the array of objects for one with the same name.
	//If found return the index to that object
	//Otherwise push_back the new name to the list

	for(uint i = 0; i < m_objObjectNames.size(); i++){
		if (!strcmp(objName, m_objObjectNames[i])) 
			return int(i);
	} 

	return -1;

}
void IGLUOBJReader::Read_V_Token( IGLUOBJTri *tri, int idx, char *token )
{
	char vertToken[128], *tPtr = vertToken;
	int vIdx;

	// If the user didn't pass in a token, read one
	if (!token)
		this->GetToken( vertToken );
	else 
		tPtr = token;

	// Parse a vertex & normal from the token
	sscanf( tPtr, "%d", &vIdx );

	// Resolve these into indicies in our data structure (not the OBJ file)
	tri->vIdx[idx] = GetVertexIndex( vIdx );
	tri->nIdx[idx] = -1;
	tri->tIdx[idx] = -1;
}

void IGLUOBJReader::Read_VN_Token( IGLUOBJTri *tri, int idx, char *token )
{
	char vertToken[128], *tPtr = vertToken;
	int vIdx, nIdx;

	// If the user didn't pass in a token, read one
	if (!token)
		this->GetToken( vertToken );
	else 
		tPtr = token;

	// Parse a vertex & normal from the token
	sscanf( tPtr, "%d//%d", &vIdx, &nIdx );

	// Resolve these into indicies in our data structure (not the OBJ file)
	tri->vIdx[idx] = GetVertexIndex( vIdx );
	tri->nIdx[idx] = GetNormalIndex( nIdx );
	tri->tIdx[idx] = -1;
}

void IGLUOBJReader::Read_VT_Token( IGLUOBJTri *tri, int idx, char *token )
{
	char vertToken[128], *tPtr = vertToken;
	int vIdx, tIdx;

	// If the user didn't pass in a token, read one
	if (!token)
		this->GetToken( vertToken );
	else 
		tPtr = token;

	// Parse a vertex & normal from the token
	sscanf( tPtr, "%d/%d", &vIdx, &tIdx );

	// Resolve these into indicies in our data structure (not the OBJ file)
	tri->vIdx[idx] = GetVertexIndex( vIdx );
	tri->nIdx[idx] = -1;
	tri->tIdx[idx] = GetTextureIndex( tIdx );
}

void IGLUOBJReader::Read_VTN_Token( IGLUOBJTri *tri, int idx, char *token )
{
	char vertToken[128], *tPtr = vertToken;
	int vIdx, tIdx, nIdx;

	// If the user didn't pass in a token, read one
	if (!token)
		this->GetToken( vertToken );
	else 
		tPtr = token;

	// Parse a vertex & normal from the token
	sscanf( tPtr, "%d/%d/%d", &vIdx, &tIdx, &nIdx );

	// Resolve these into indicies in our data structure (not the OBJ file)
	tri->vIdx[idx] = GetVertexIndex( vIdx );
	tri->nIdx[idx] = GetNormalIndex( nIdx );
	tri->tIdx[idx] = GetTextureIndex( tIdx );
}
void IGLUOBJReader::CopyForTriangleFan( const IGLUOBJTri * lastTri, IGLUOBJTri* newTri)
{
	newTri->vIdx[0] = lastTri->vIdx[0]; 
	newTri->nIdx[0] = lastTri->nIdx[0];
	newTri->tIdx[0] = lastTri->tIdx[0];
	newTri->vIdx[1] = lastTri->vIdx[2];
	newTri->nIdx[1] = lastTri->nIdx[2];
	newTri->tIdx[1] = lastTri->tIdx[2];

}
void IGLUOBJReader::CopyForTriangleFan( IGLUOBJTri *newTri )
{
	newTri->vIdx[0] = m_objTris[m_objTris.size()-1]->vIdx[0]; 
	newTri->nIdx[0] = m_objTris[m_objTris.size()-1]->nIdx[0];
	newTri->tIdx[0] = m_objTris[m_objTris.size()-1]->tIdx[0];
	newTri->vIdx[1] = m_objTris[m_objTris.size()-1]->vIdx[2];
	newTri->nIdx[1] = m_objTris[m_objTris.size()-1]->nIdx[2];
	newTri->tIdx[1] = m_objTris[m_objTris.size()-1]->tIdx[2];
}

void IGLUOBJReader::SelectReadMethod( FnParserPtr *pPtr )
{
	int v, t, n; // garbage vars
	char vertToken[128];
	char *lPtr = this->GetCurrentLinePointer();

	// Non-destructively parse the next token
	StripLeadingTokenToBuffer( lPtr, vertToken );

	if (strstr(vertToken, "//"))                               // Then it has the v//n format
	{
		*pPtr = &iglu::IGLUOBJReader::Read_VN_Token;
		m_hasVertices = m_hasNormals = true;
	}

	else if ( sscanf(vertToken, "%d/%d/%d", &v, &t, &n) == 3 ) // Then it has the v/t/n format
	{
		*pPtr = &iglu::IGLUOBJReader::Read_VTN_Token;
		m_hasVertices = m_hasNormals = m_hasTexCoords = true;
	}

	else if ( sscanf(vertToken, "%d/%d", &v, &t) == 2 )        // Then it has the v/t format
	{
		*pPtr = &iglu::IGLUOBJReader::Read_VT_Token;
		m_hasVertices = m_hasTexCoords = true;
	}else                                                     // Then it has the v format
		*pPtr = &iglu::IGLUOBJReader::Read_V_Token;
		m_hasVertices = true;
	}
	
bool IGLUOBJReader::IsValidFLine(FnParserPtr *pPtr)
{

	//Garbage variables. This will break if it comes accross an entry on a line
	//longer than 128 characters. 
	char c0[128];
	char c1[128];
	char c2[128];
	char vertToken[128]; 

	// Non-destructively parse the next token
	char *lPtr = this->GetCurrentLinePointer();
	StripLeadingTokenToBuffer( lPtr, vertToken );

	//Make sure we have at least three entries in this vertex
	if(sscanf(lPtr, "%s %s %s", c0, c1, c2) >= 3){
		return true;
	}else{
		this->WarningMessage("Corrupt 'f': %s", lPtr);
		return false; 
	}
}

void IGLUOBJReader::AddDataToArray( float *arr, int startIdx, int matlID, int objectID, vec3 *vert, vec3 *norm, vec2 *tex )
{
	// We'd better have a vertex, or this whole rendering process is stupid....
	assert( vert );

	
	int i = startIdx;
	
	// push_back the material ID
	arr[i++] = m_loadMtlFile ? float(matlID) : 0.0;
	
	// push_back the object ID
	arr[i++] = m_assignObjects ? float(objectID) : 0.0f;

	// push_back the vertex
	arr[i++] = vert->X();
	arr[i++] = vert->Y();
	arr[i++] = vert->Z();

	// If this vertex has a normal, push_back it.
	if (norm)
	{
		arr[i++] = norm->X();
		arr[i++] = norm->Y();
		arr[i++] = norm->Z();
	}

	// If this vertex has a texture coordinate, push_back it.
	if (tex)
	{
		arr[i++] = tex->X();
		arr[i++] = tex->Y();
	}
}

void IGLUOBJReader::CenterAndResize( float *arr, int numVerts )
{
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

	int startIdx   = m_vertOff / sizeof(float);
	int vertStride = m_vertStride / sizeof(float);
	float minX = FLT_MAX, minY = FLT_MAX, minZ = FLT_MAX;
	float maxX = FLT_MIN, maxY = FLT_MIN, maxZ = FLT_MIN;

	// Walk through vertex array, find max & min (x, y, z)
	for (int i=0; i<numVerts; i++)
	{
		int idx = startIdx + i*vertStride;
		minX = iglu::min( minX, arr[idx+0] );
		maxX = iglu::max( maxX, arr[idx+0] );
		minY = iglu::min( minY, arr[idx+1] );
		maxY = iglu::max( maxY, arr[idx+1] );
		minZ = iglu::min( minZ, arr[idx+2] );
		maxZ = iglu::max( maxZ, arr[idx+2] );
	}

	// Find the center of the object & the width of the object
	float ctrX = (m_resize || m_center ? 0.5*(minX+maxX) : 0.0 );
	float ctrY = (m_resize || m_center ? 0.5*(minY+maxY) : 0.0 ); 
	float ctrZ = (m_resize || m_center ? 0.5*(minZ+maxZ) : 0.0 );
	float deltaX = 0.5*(maxX-minX), deltaY = 0.5*(maxY-minY), deltaZ = 0.5*(maxZ-minZ);
	float delta = ( m_resize ? 
		            iglu::max( deltaX, iglu::max( deltaY, deltaZ ) ) :
	                1 );

	// Walk through the vertex array and update the positions
	for (int i=0; i<numVerts; i++)
	{
		int idx = startIdx + i*vertStride;
		arr[idx+0] = (arr[idx+0] - ctrX ) / delta;
		arr[idx+1] = (arr[idx+1] - ctrY ) / delta;
		arr[idx+2] = (arr[idx+2] - ctrZ ) / delta;
	}
}

void IGLUOBJReader::GetCompactArrayBuffer( void )
{
	// Create an OBJ vert ID -> element array vert ID map.  Init all entries to 0xFFFFFFFF.
	//    Also, create mapping vertID -> last normal ID used for this vertex
	//          create mapping vertID -> last tex coord used for this vertex
	uint *vertMapping = (uint *)malloc( m_objVerts.size() * sizeof( uint ) );
	uint *normMapping = (uint *)malloc( m_objVerts.size() * sizeof( uint ) );
	uint *texMapping  = (uint *)malloc( m_objVerts.size() * sizeof( uint ) );
	assert( vertMapping );
	memset( vertMapping, 0xFF, m_objVerts.size() * sizeof( uint ) );
	memset( normMapping, 0xFF, m_objVerts.size() * sizeof( uint ) );
	memset( texMapping, 0xFF, m_objVerts.size() * sizeof( uint ) );

	//   We'll have 1 float for a material ID
	//   We'll have 3 floats (x,y,z) for each of the 3 verts of each triangle 
	//   We'll have 3 floats (x,y,z) for each of the 3 normals of each triangle
	//   We'll have 2 floats (u,v) for each of the 3 texture coordinates of each triangle
	uint  numComponents = 1 + 1 + 3 + (m_hasNormals ? 3 : 0) + (m_hasTexCoords ? 2 : 0);

	float bufSz  = numComponents * sizeof( float ) * (3 * m_objTris.size());
	m_vertStride = numComponents * sizeof( float );
	m_matlIdOff  = 0 * sizeof( float );
	m_objectIdOff = 1 * sizeof(float); 
	m_vertOff    = 2 * sizeof( float );
	m_normOff    = (m_hasNormals ? 5 : 0) * sizeof( float );
	m_texOff     = (m_hasTexCoords ? (m_hasNormals ? 8 : 5) : 0) * sizeof( float );

	// Create a vertex buffer for us to fill up.
	float *tmpBuf = (float *)malloc( bufSz );

	// Create an element array buffer to fill up
	uint elembufSz = 3 * sizeof( unsigned int ) * m_objTris.size();
	uint *tmpElemBuf = (uint *)malloc( elembufSz );

	// We'll need to know the size of our two arrays
	int numArrayElements = 3 * m_objTris.size();  // Known in advance
	int numArrayVerts    = 0;                     // Depends on how many verts are reused.  We'll compute

	for (uint i=0, triNum=0; triNum < m_objTris.size(); i+=3,triNum++ )
	{
		int i0 = m_objTris[triNum]->vIdx[0];
		int i1 = m_objTris[triNum]->vIdx[1];
		int i2 = m_objTris[triNum]->vIdx[2];

		if (    (vertMapping[i0] == 0xFFFFFFFF) // We haven't seen this vertex yet.  push_back to list
		     || (m_hasNormals && normMapping[i0] != m_objTris[triNum]->nIdx[0])     // We saw this vertex...  but w/different normal
			 || (m_hasTexCoords && texMapping[i0] != m_objTris[triNum]->tIdx[0]) )  // We saw this vertex...  but w/different texcoord
		{
			AddDataToArray( tmpBuf, numArrayVerts*numComponents, m_objTris[triNum]->matlID, 
							m_objTris[triNum]->objectID,
							&m_objVerts[m_objTris[triNum]->vIdx[0]], 
							m_hasNormals ? &m_objNorms[m_objTris[triNum]->nIdx[0]] : 0, 
							m_hasTexCoords ? &m_objTexCoords[m_objTris[triNum]->tIdx[0]] : 0 );
			tmpElemBuf[i]   = numArrayVerts;
			vertMapping[i0] = numArrayVerts++;
			normMapping[i0] = m_objTris[triNum]->nIdx[0];
			texMapping[i0]  = m_objTris[triNum]->tIdx[0];
		}
		else                               // We've already seen vertex; reuse it.
			tmpElemBuf[i] = vertMapping[i0];

		if (    (vertMapping[i1] == 0xFFFFFFFF) // We haven't seen this vertex yet.  push_back to list
		     || (m_hasNormals && normMapping[i1] != m_objTris[triNum]->nIdx[1])     // We saw this vertex...  but w/different normal
			 || (m_hasTexCoords && texMapping[i1] != m_objTris[triNum]->tIdx[1]) )  // We saw this vertex...  but w/different texcoord
		{
			AddDataToArray( tmpBuf, numArrayVerts*numComponents, m_objTris[triNum]->matlID, 
							m_objTris[triNum]->objectID,
							&m_objVerts[m_objTris[triNum]->vIdx[1]], 
							m_hasNormals ? &m_objNorms[m_objTris[triNum]->nIdx[1]] : 0, 
							m_hasTexCoords ? &m_objTexCoords[m_objTris[triNum]->tIdx[1]] : 0 );
			tmpElemBuf[i+1]   = numArrayVerts;
			vertMapping[i1] = numArrayVerts++;
			normMapping[i1] = m_objTris[triNum]->nIdx[1];
			texMapping[i1]  = m_objTris[triNum]->tIdx[1];
		}
		else                               // We've already seen vertex; reuse it.
			tmpElemBuf[i+1] = vertMapping[i1];

		if (    (vertMapping[i2] == 0xFFFFFFFF) // We haven't seen this vertex yet.  push_back to list
		     || (m_hasNormals && normMapping[i2] != m_objTris[triNum]->nIdx[2])     // We saw this vertex...  but w/different normal
			 || (m_hasTexCoords && texMapping[i2] != m_objTris[triNum]->tIdx[2]) )  // We saw this vertex...  but w/different texcoord
		{
			AddDataToArray( tmpBuf, numArrayVerts*numComponents, m_objTris[triNum]->matlID, 
							m_objTris[triNum]->objectID,
							&m_objVerts[m_objTris[triNum]->vIdx[2]], 
							m_hasNormals ? &m_objNorms[m_objTris[triNum]->nIdx[2]] : 0, 
							m_hasTexCoords ? &m_objTexCoords[m_objTris[triNum]->tIdx[2]] : 0 );
			tmpElemBuf[i+2]   = numArrayVerts;
			vertMapping[i2] = numArrayVerts++;
			normMapping[i2] = m_objTris[triNum]->nIdx[2];
			texMapping[i2]  = m_objTris[triNum]->tIdx[2];
		}
		else                               // We've already seen vertex; reuse it.
			tmpElemBuf[i+2] = vertMapping[i2];
	}

	// If the user asked us to resize & center the object, do that.
	if (m_resize || m_center)
		CenterAndResize( tmpBuf, numArrayVerts );

	// Copy our arrays into their GPU buffers
	m_vertArr->SetVertexArray( numArrayVerts * m_vertStride, tmpBuf, IGLU_STATIC|IGLU_DRAW );
	m_vertArr->SetElementArray( GL_UNSIGNED_INT, elembufSz, tmpElemBuf, IGLU_STATIC|IGLU_DRAW );

	// Free our temporary copy of the data
	free( tmpBuf );
	free( tmpElemBuf );
	free( vertMapping );
	
}

void IGLUOBJReader::GetArrayBuffer( void )
{
	// Using our EXTREMELY naive approach...
	//   We'll have 1 float for a material ID
	//   We'll have 1 float for an object ID
	//   We'll have 3 floats (x,y,z) for each of the 3 verts of each triangle 
	//   We'll have 3 floats (x,y,z) for each of the 3 normals of each triangle
	//   We'll have 2 floats (u,v) for each of the 3 texture coordinates of each triangle
	uint  numComponents = 1 + 1 + 3 + (m_hasNormals ? 3 : 0) + (m_hasTexCoords ? 2 : 0);

	float bufSz  = numComponents * sizeof( float ) * (3 * m_objTris.size());
	m_vertStride = numComponents * sizeof( float );
	m_matlIdOff  = 0 * sizeof( float );
	m_objectIdOff = 1 * sizeof(float); 
	m_vertOff    = 2 * sizeof( float );
	m_normOff    = (m_hasNormals ? 5 : 0) * sizeof( float );
	m_texOff     = (m_hasTexCoords ? (m_hasNormals ? 8 : 5) : 0) * sizeof( float );

	// Create a vertex array buffer for us to fill up.
	float *tmpBuf = (float *)malloc( bufSz );

	// For our very, very early reader, we'll use the MOST NAIVE approach
	for (uint i=0, triNum=0; i<3*numComponents*m_objTris.size(); i+=3*numComponents,triNum++ )
	{
		AddDataToArray( tmpBuf, i, m_objTris[triNum]->matlID,
						m_objTris[triNum]->objectID,
			            &m_objVerts[m_objTris[triNum]->vIdx[0]], 
						m_hasNormals ? &m_objNorms[m_objTris[triNum]->nIdx[0]] : 0, 
						m_hasTexCoords ? &m_objTexCoords[m_objTris[triNum]->tIdx[0]] : 0 );
		
		AddDataToArray( tmpBuf, i+numComponents, m_objTris[triNum]->matlID,
						m_objTris[triNum]->objectID,
			            &m_objVerts[m_objTris[triNum]->vIdx[1]], 
						m_hasNormals ? &m_objNorms[m_objTris[triNum]->nIdx[1]] : 0, 
						m_hasTexCoords ? &m_objTexCoords[m_objTris[triNum]->tIdx[1]] : 0 );
		
		AddDataToArray( tmpBuf, i+2*numComponents, m_objTris[triNum]->matlID,
						m_objTris[triNum]->objectID,
			            &m_objVerts[m_objTris[triNum]->vIdx[2]], 
						m_hasNormals ? &m_objNorms[m_objTris[triNum]->nIdx[2]] : 0, 
						m_hasTexCoords ? &m_objTexCoords[m_objTris[triNum]->tIdx[2]] : 0 );
	}

	// If the user asked us to resize & center the object, do that.
	if (m_resize || m_center)
		CenterAndResize( tmpBuf, 3 * m_objTris.size() );

	// Copy our element array into the buffer
	m_vertArr->SetVertexArray( bufSz, tmpBuf, IGLU_STATIC|IGLU_DRAW );

	// Free our temporary copy of the data
	free( tmpBuf );
}

uint IGLUOBJReader::GetArrayBufferStride( void )
{
	return m_vertStride;
}

void IGLUOBJReader::GetElementArrayBuffer( void )
{
	// We'll have one index for each of the 3 verts of each triangle (i.e., GL_TRIANGLES)
	uint bufSz = 3 * sizeof( unsigned int ) * m_objTris.size();

	// Create a buffer for us to fill up.
	uint *tmpBuf = (uint *)malloc( bufSz );

	// For our very, very early reader, we'll use the MOST NAIVE approach
	for (uint i=0; i<3*m_objTris.size(); i++)
		tmpBuf[i] = i;

	// Copy our element array into the buffer
	m_vertArr->SetElementArray( GL_UNSIGNED_INT, bufSz, tmpBuf, IGLU_STATIC|IGLU_DRAW );

	// Free our temporary copy of the data
	free( tmpBuf );
}
int IGLUOBJReader::SetupVertexArrayForGI( IGLUShaderProgram::Ptr & shader, IGLUBuffer::Ptr &InstanceBO)
{
	bool vertAvail = HasVertices();   // && (shader[ iglu::IGLU_ATTRIB_VERTEX ] != 0);
	bool normAvail = HasNormals();    // && (shader[ iglu::IGLU_ATTRIB_NORMAL ] != 0);
	bool texAvail  = HasTexCoords();  // && (shader[ iglu::IGLU_ATTRIB_TEXCOORD ] != 0);
	bool matlAvail = HasMatlID();     // && (shader[ iglu::IGLU_ATTRIB_MATL_ID ] != 0);
	bool objectAvail = HasObjectID(); // && (shader[iglu::IGLU_ATTRIB_OBJECT_ID] != 0);

	// We need *at least* a vertex semantic
	if (!vertAvail) return IGLU_ERROR_NO_GLSL_VERTEX_SEMANTIC;

	// Setup the attributes needed for this geometry
	m_vertArr->EnableAttribute( IGLU_ATTRIB_VERTEX, 3, GL_FLOAT, m_vertStride, BUFFER_OFFSET(m_vertOff));
	if (normAvail)  m_vertArr->EnableAttribute( IGLU_ATTRIB_NORMAL, 
		                                        3, GL_FLOAT, m_vertStride, BUFFER_OFFSET(m_normOff));
	if (texAvail)   m_vertArr->EnableAttribute( IGLU_ATTRIB_TEXCOORD, 
		                                        2, GL_FLOAT, m_vertStride, BUFFER_OFFSET(m_texOff));
	if (matlAvail)  m_vertArr->EnableAttribute( IGLU_ATTRIB_MATL_ID, 
		                                        1, GL_FLOAT, m_vertStride, BUFFER_OFFSET(m_matlIdOff));
	if(objectAvail) m_vertArr->EnableAttribute( IGLU_ATTRIB_OBJECT_ID, 
                                                1, GL_FLOAT, m_vertStride, BUFFER_OFFSET(m_objectIdOff));
		// we bind here the instance data buffer
	//InstanceBO->Bind();
	m_vertArr->Bind();
	GLuint id = InstanceBO->GetBufferID();
	glBindBuffer(GL_ARRAY_BUFFER,id);
	//InstanceBO->Bind();
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(float)*8, (void*)0);

	// we set up the vertex attribute divisor to 1 as we would like to step once for every instance
	// we use the ARB version of the function as GLEW incorrectly misses the definition of core instanced arrays
	glVertexAttribDivisorARB(5, 1);
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(float)*8, (void*)(4*sizeof(float)));
	glVertexAttribDivisorARB(6, 1);
	//InstanceBO->Unbind();
	m_vertArr->Unbind();
	//InstanceBO->Unbind();
	
	return IGLU_NO_ERROR;
}
int IGLUOBJReader::SetupVertexArrayForGI( IGLUShaderProgram::Ptr & shader, GLuint bufferId)
{
	bool vertAvail = HasVertices();   // && (shader[ iglu::IGLU_ATTRIB_VERTEX ] != 0);
	bool normAvail = HasNormals();    // && (shader[ iglu::IGLU_ATTRIB_NORMAL ] != 0);
	bool texAvail  = HasTexCoords();  // && (shader[ iglu::IGLU_ATTRIB_TEXCOORD ] != 0);
	bool matlAvail = HasMatlID();     // && (shader[ iglu::IGLU_ATTRIB_MATL_ID ] != 0);
	bool objectAvail = HasObjectID(); // && (shader[iglu::IGLU_ATTRIB_OBJECT_ID] != 0);

	// We need *at least* a vertex semantic
	if (!vertAvail) return IGLU_ERROR_NO_GLSL_VERTEX_SEMANTIC;

	// Setup the attributes needed for this geometry
	m_vertArr->EnableAttribute( IGLU_ATTRIB_VERTEX, 3, GL_FLOAT, m_vertStride, BUFFER_OFFSET(m_vertOff));
	if (normAvail)  m_vertArr->EnableAttribute( IGLU_ATTRIB_NORMAL, 
		                                        3, GL_FLOAT, m_vertStride, BUFFER_OFFSET(m_normOff));
	if (texAvail)   m_vertArr->EnableAttribute( IGLU_ATTRIB_TEXCOORD, 
		                                        2, GL_FLOAT, m_vertStride, BUFFER_OFFSET(m_texOff));
	if (matlAvail)  m_vertArr->EnableAttribute( IGLU_ATTRIB_MATL_ID, 
		                                        1, GL_FLOAT, m_vertStride, BUFFER_OFFSET(m_matlIdOff));
	if(objectAvail) m_vertArr->EnableAttribute( IGLU_ATTRIB_OBJECT_ID, 
                                                1, GL_FLOAT, m_vertStride, BUFFER_OFFSET(m_objectIdOff));
		// we bind here the instance data buffer
	//InstanceBO->Bind();
	m_vertArr->Bind();
	
	glBindBuffer(GL_ARRAY_BUFFER, bufferId);
	//InstanceBO->Bind();
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(float)*8, (void*)0);
	glVertexAttribDivisorARB(5, 1);
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(float)*8, BUFFER_OFFSET(4*sizeof(float)));
	// we set up the vertex attribute divisor to 1 as we would like to step once for every instance
	// we use the ARB version of the function as GLEW incorrectly misses the definition of core instanced arrays
	
	glVertexAttribDivisorARB(6, 1);
	//InstanceBO->Unbind();
	m_vertArr->Unbind();
	//InstanceBO->Unbind();
	
	return IGLU_NO_ERROR;
}
int IGLUOBJReader::SetupVertexArray( IGLUShaderProgram::Ptr & )//shader )
{
	//m_shaderID = shader->GetProgramID();
                                      
	// Identify what sort of attributes for shader data should be enabled...  
	//   -> Initially, we also checked that our shader accepted this attribute.
	//      But this required all sorts of nonsense that messed up the IGLU interface
	//      and led to strange, hard-to-debug errors.  Instead, we're just going to
	//      *always* pass down all attributes enabled by the geometry to their assigned
	//      attribute slot.  If there's no such attribute, GLSL simply won't use it!
	bool vertAvail = HasVertices();   // && (shader[ iglu::IGLU_ATTRIB_VERTEX ] != 0);
	bool normAvail = HasNormals();    // && (shader[ iglu::IGLU_ATTRIB_NORMAL ] != 0);
	bool texAvail  = HasTexCoords();  // && (shader[ iglu::IGLU_ATTRIB_TEXCOORD ] != 0);
	bool matlAvail = HasMatlID();     // && (shader[ iglu::IGLU_ATTRIB_MATL_ID ] != 0);
	bool objectAvail = HasObjectID(); // && (shader[iglu::IGLU_ATTRIB_OBJECT_ID] != 0);

	// We need *at least* a vertex semantic
	if (!vertAvail) return IGLU_ERROR_NO_GLSL_VERTEX_SEMANTIC;

	// Setup the attributes needed for this geometry
	m_vertArr->EnableAttribute( IGLU_ATTRIB_VERTEX, 3, GL_FLOAT, m_vertStride, BUFFER_OFFSET(m_vertOff));
	if (normAvail)  m_vertArr->EnableAttribute( IGLU_ATTRIB_NORMAL, 
		                                        3, GL_FLOAT, m_vertStride, BUFFER_OFFSET(m_normOff));
	if (texAvail)   m_vertArr->EnableAttribute( IGLU_ATTRIB_TEXCOORD, 
		                                        2, GL_FLOAT, m_vertStride, BUFFER_OFFSET(m_texOff));
	if (matlAvail)  m_vertArr->EnableAttribute( IGLU_ATTRIB_MATL_ID, 
		                                        1, GL_FLOAT, m_vertStride, BUFFER_OFFSET(m_matlIdOff));
	if(objectAvail) m_vertArr->EnableAttribute( IGLU_ATTRIB_OBJECT_ID, 
                                                1, GL_FLOAT, m_vertStride, BUFFER_OFFSET(m_objectIdOff));
	return IGLU_NO_ERROR;
}

//GI Via vertex array ,each instance has two vec4 in InstanceBO
int IGLUOBJReader::DrawMultipleInstances(IGLUShaderProgram::Ptr & shader,  IGLUBuffer::Ptr &InstanceBO, int numOfInstances)
{
	// If the shader in question is not already enabled, enable it!
	bool wasShaderBound = shader->IsEnabled();
	if (!wasShaderBound)
		shader->Enable();

	// We may already have the vertex array setup to work with this shader.
	//    Check that.  If not, re-set up the vertex array.
	//if (m_shaderID != shader->GetProgramID())
	if (m_shaderID == 0)
	{
		m_shaderID = shader->GetProgramID();
		int err = this->SetupVertexArrayForGI( shader, InstanceBO);
		if (err != IGLU_NO_ERROR)
			return err;
	}

	m_vertArr->DrawElementsInstanced(GL_TRIANGLES, 3* GetTriangleCount(),numOfInstances);
	// If we started with the shader disabled, disable it again now.
	if (!wasShaderBound)
		shader->Disable();

	return IGLU_NO_ERROR;
}

//GI via vertex array, each instance has two vec4 input in buffer
int IGLUOBJReader::DrawMultipleInstances(IGLUShaderProgram::Ptr & shader,  GLuint bufferId, int numOfInstances)
{
	// If the shader in question is not already enabled, enable it!
	bool wasShaderBound = shader->IsEnabled();
	if (!wasShaderBound)
		shader->Enable();

	// We may already have the vertex array setup to work with this shader.
	//    Check that.  If not, re-set up the vertex array.
	//if (m_shaderID != shader->GetProgramID())
	if (m_shaderID == 0)
	{
		m_shaderID = shader->GetProgramID();
		int err = this->SetupVertexArrayForGI( shader, bufferId);
		if (err != IGLU_NO_ERROR)
			return err;
	}

	m_vertArr->DrawElementsInstanced(GL_TRIANGLES, 3* GetTriangleCount(),numOfInstances);
	// If we started with the shader disabled, disable it again now.
	if (!wasShaderBound)
		shader->Disable();

	return IGLU_NO_ERROR;
}

//GI via Instance Data passed from texture
int IGLUOBJReader::DrawMultipleInstances(IGLUShaderProgram::Ptr & shader, IGLUTextureBuffer::Ptr &InstanceTex, int numOfInstances)
{
	// If the shader in question is not already enabled, enable it!
	bool wasShaderBound = shader->IsEnabled();
	if (!wasShaderBound)
		shader->Enable();

	// We may already have the vertex array setup to work with this shader.
	//    Check that.  If not, re-set up the vertex array.
	//if (m_shaderID != shader->GetProgramID())
	if (m_shaderID == 0)
	{
		m_shaderID = shader->GetProgramID();
		int err = this->SetupVertexArray( shader);
		shader["InstanceData"] = InstanceTex;
		if (err != IGLU_NO_ERROR)
			return err;
	}
	
	m_vertArr->DrawElementsInstanced(GL_TRIANGLES, 3* GetTriangleCount(),numOfInstances);
	// If we started with the shader disabled, disable it again now.
	if (!wasShaderBound)
		shader->Disable();

	return IGLU_NO_ERROR;
}
//GI via Instance Data passed from Uniform Buffer
int IGLUOBJReader::DrawMultipleInstances(IGLUShaderProgram::Ptr & shader,  int numOfInstances)
{
	// If the shader in question is not already enabled, enable it!
	bool wasShaderBound = shader->IsEnabled();
	if (!wasShaderBound)
		shader->Enable();

	// We may already have the vertex array setup to work with this shader.
	//    Check that.  If not, re-set up the vertex array.
	//if (m_shaderID != shader->GetProgramID())
	if (m_shaderID == 0)
	{
		m_shaderID = shader->GetProgramID();
		int err = this->SetupVertexArray( shader);		
		
		
		if (err != IGLU_NO_ERROR)
			return err;
	}
	m_vertArr->DrawElementsInstanced(GL_TRIANGLES, 3* GetTriangleCount(),numOfInstances);
	// If we started with the shader disabled, disable it again now.
	if (!wasShaderBound)
		shader->Disable();

	return IGLU_NO_ERROR;
}
int IGLUOBJReader::Draw( IGLUShaderProgram::Ptr &shader )
{
	// If the shader in question is not already enabled, enable it!
	bool wasShaderBound = shader->IsEnabled();
	if (!wasShaderBound)
		shader->Enable();

	// We may already have the vertex array setup to work with this shader.
	//    Check that.  If not, re-set up the vertex array.
	//if (m_shaderID != shader->GetProgramID())
	if (m_shaderID == 0)
	{
		m_shaderID = shader->GetProgramID();
		int err = SetupVertexArray( shader );
		if (err != IGLU_NO_ERROR)
			return err;
	}

	m_vertArr->DrawElements( GL_TRIANGLES, 3* GetTriangleCount() );

	// If we started with the shader disabled, disable it again now.
	if (!wasShaderBound)
		shader->Disable();

	return IGLU_NO_ERROR;
}