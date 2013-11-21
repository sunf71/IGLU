/******************************************************************/
/* igluOBJReader.h                                                */
/* -----------------------                                        */
/*                                                                */
/* Defines a reader for OBJ model files and associated materials  */
/*                                                                */
/* Chris Wyman (10/20/2011)                                       */
/******************************************************************/
/*
add Geometry Instancing draw function
Sun Feng 10/28/2013
*/
/*
simply replace IGLU1DARRAY  with STL vector
Sun Feng 11/14/2013
*/

/*
add glm construction fuction, code from Nate Robins, 1997  
*/
#ifndef IGLU_OBJ_READER_H
#define IGLU_OBJ_READER_H

#pragma warning( disable: 4996 )


#include "igluModel.h"
#include "iglu/glstate/igluVertexArrayObject.h"
#include "glmModel.h"
#include <vector>
namespace iglu {
struct IGLUOBJTri
{
	IGLUOBJTri(char *m, char *g, char*o) : mtlName(m), grpName(g), objName(o) {}
	int vIdx[3], nIdx[3], tIdx[3], matlID, objectID;
	char *mtlName, *grpName, *objName;
};
enum IGLUModelParams {
	IGLU_OBJ_DEFAULT_STATE      = 0x0000,
	IGLU_OBJ_CENTER             = 0x0001,  // Recenter model around the origin
	IGLU_OBJ_UNITIZE            = 0x0002,  // Resize model so it ranges from [-1..1]
	IGLU_OBJ_COMPACT_STORAGE    = 0x0004,  // Attempt to reuse vertices shared between triangles
	IGLU_OBJ_NO_MATERIALS       = 0x0008,  // Do no load an .mtl file.
	IGLU_OBJ_NO_OBJECTS			= 0x000F
};


struct IGLUOBJTri;
class  IGLUBuffer;

class IGLUOBJReader : public IGLUFileParser, public IGLUModel
{
public:
	// Constructor reads from the file.
	IGLUOBJReader( char *filename, int params=IGLU_OBJ_DEFAULT_STATE );
	// Constructor from GLMmodel
	IGLUOBJReader( GLMmodel* model, int parms = IGLU_OBJ_DEFAULT_STATE);
	virtual ~IGLUOBJReader();

	// Get some data about the object file
	uint GetTriangleCount( void ) const       { return m_objTris.size(); }

	// Get OpenGL buffers for vertex/triangle/normal data
	IGLUVertexArray::Ptr &GetVertexArray( void )         { return m_vertArr; }

	// Get vertex attrib array data
	uint GetArrayBufferStride( void );

	// Implementation of the necessary IGLUModel virtual methods
	virtual int Draw( IGLUShaderProgram::Ptr &shader );
	//GI via Instance Data passed from vertex attribute array, InstanceBO is the IGLUBuffer contain instanceData
	virtual int DrawMultipleInstances(IGLUShaderProgram::Ptr & shader, IGLUBuffer::Ptr &InstanceBO, int numOfInstances);
	//GI via Instance Data passed from vertex attribute array, bufferId is the id of OpenGL Buffer
	virtual int DrawMultipleInstances(IGLUShaderProgram::Ptr & shader, GLuint bufferId, int numOfInstances);
	//GI via Instance Data passed from texture
	virtual int DrawMultipleInstances(IGLUShaderProgram::Ptr & shader, IGLUTextureBuffer::Ptr &InstanceTex, int numOfInstances);
	//GI via Instance Data passed from Uniform Buffer, note the uniform buffer should be set up before calling this method 
	virtual int DrawMultipleInstances(IGLUShaderProgram::Ptr & shader, int numOfInstances);
	virtual bool HasVertices ( void ) const			{ return m_hasVertices; }
	virtual bool HasTexCoords( void ) const         { return m_hasTexCoords; }
	virtual bool HasNormals  ( void ) const         { return m_hasNormals; }
	virtual bool HasMatlID   ( void ) const         { return m_hasMatlID; }
	virtual bool HasObjectID ( void ) const			{ return m_hasObjectID; } 

	// A pointer to a IGLUOBJReader could have type IGLUOBJReader::Ptr
	typedef IGLUOBJReader *Ptr;

	std::vector<vec3>& GetVertecies()
	{
		return m_objVerts;
	}

	std::vector<IGLUOBJTri *>& GetTriangles()
	{
		return m_objTris;
	}
	std::vector<vec3>& GetNormals()
	{
		return m_objNorms;
	}
	std::vector<vec2>& GetTexCoords()
	{
		return m_objTexCoords;
	}
private:
	bool m_compactFormat, m_loadMtlFile, m_assignObjects;
	uint  m_curMatlId;
	uint  m_curObjectId;
	// When drawing we need to bind the vertex buffer to the appropriate 
	//    shader inputs, so we need to know which shader to use.  However, there
	//    is overhead in the setup, so if the same shader is used repeatedly on
	//    the same object, we can reuse the settings from last time.
	GLuint m_shaderID;

	// Our buffer(s) storing the vertex array for this geometry
	IGLUVertexArray::Ptr m_vertArr;

	// Basic geometric definitions
	std::vector<vec3> m_objVerts;
	std::vector<vec3> m_objNorms;
	std::vector<vec2> m_objTexCoords;

	// Basic geometric triangles definitions
	std::vector<IGLUOBJTri *> m_objTris;
	
	// Locations for material file(s)
	std::vector<char *> m_objMtlFiles;


	// Array of object names inside the file
	std::vector<char *> m_objObjectNames;

	// Does the user want us to resize and center the object around the origin?
	//    Some/many models use completely arbitrary coordinates, so it's difficult
	//    to know how big and where they'll be relative to each other
	bool m_resize, m_center;

	// Information about the OBJ file we read
	bool m_hasVertices, m_hasNormals, m_hasTexCoords, m_hasMatlID, m_hasObjectID;

	// What is the stride of the data?
	GLuint m_vertStride, m_vertOff, m_normOff, m_texOff, m_matlIdOff, m_objectIdOff;

	// When drawing, sometimes we need to setup our vertex array to work with
	//    the currently selected shader.  This method does that.
	int SetupVertexArray( IGLUShaderProgram::Ptr &shader );
	int SetupVertexArrayForGI(IGLUShaderProgram::Ptr &shader,  GLuint buffferId );
	int SetupVertexArrayForGI(IGLUShaderProgram::Ptr &shader,  IGLUBuffer::Ptr &InstanceBO );
	// Indicies in OBJ files may be relative (i.e., negative).  OBJ files also
	//    use a Pascal array indexing scheme (i.e., start from 1).  These methods
	//    fixes these issues to give an array index we can actually use.
	unsigned int GetVertexIndex( int relativeIdx );
	unsigned int GetNormalIndex( int relativeIdx );
	unsigned int GetTextureIndex( int relativeIdx );

	// These methods make sure the vertex array and element array buffers are set up correctly
	void GetArrayBuffer( void );
	void GetElementArrayBuffer( void );
	void GetCompactArrayBuffer( void );

	// Read appropriate facet tokens
	void Read_V_Token( IGLUOBJTri *tri, int idx, char *token=0 );
	void Read_VT_Token( IGLUOBJTri *tri, int idx, char *token=0 );
	void Read_VN_Token( IGLUOBJTri *tri, int idx, char *token=0 );
	void Read_VTN_Token( IGLUOBJTri *tri, int idx, char *token=0 );

	// Helper function for adding data to a float array for our GPU-packed array
	void AddDataToArray( float *arr, int startIdx, int matlID, int objectID, vec3 *vert, vec3 *norm, vec2 *tex );

	// Helper for centering & resizing the geometry in the array before sending it to OpenGL
	void CenterAndResize( float *arr, int numVerts );

	// This declares an ugly type FnParserPtr that points to one of the Read_???_Token methods
	typedef void (iglu::IGLUOBJReader::*FnParserPtr)(IGLUOBJTri *, int, char *); 

	// Copy from prior triangle facets when triangulating
	void CopyForTriangleFan( IGLUOBJTri *newTri );
	void CopyForTriangleFan( const IGLUOBJTri * lastTri, IGLUOBJTri* newTri);
	// A method that is called after finding an 'f' line that identifies
	//    the appropriate Read_???_Token() method to call when parsing
	void SelectReadMethod( FnParserPtr *pPtr );

	//Checks that an f line has enough vertices to be parsed as a triangle.
	//For example, it returns false if the f line has only two entries on it
	bool IsValidFLine( FnParserPtr *pPtr);
	
	//Determines if objName has been seen before in this obj file
	int GetObjectID( char *objName );

	
};


// End namespace iglu
}



#endif

