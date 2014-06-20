#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "SceneHelper.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include "iglu.h"
#define MAXLINELENGTH 255
using namespace std;
namespace iglu
{

	glm::vec3 parseVec3(char* buffer)
	{
		glm::vec3 result;
		char *ptr;
		ptr = StripLeadingNumber( buffer, &result[0] );
		ptr = StripLeadingNumber( ptr, &result[1] );
		ptr = StripLeadingNumber( ptr, &result[2] );
		return result;
	}
	SceneData::SceneData(const char* fileName)
	{
		FILE *sceneFile = fopen( fileName, "r" );
		if (!sceneFile) 
		{
			printf( "Scene::Scene() unable to open scene file '%s'!\n", fileName );
			return;
		}

		char buf[ MAXLINELENGTH ], token[256], *ptr;
		while( fgets(buf, MAXLINELENGTH, sceneFile) != NULL )  
		{
			// Is this line a comment?
			ptr = StripLeadingWhiteSpace( buf );
			if (ptr == 0 || ptr[0] == '\n' || ptr[0] == '#') continue;
			// Nope.  So find out what the command is...
			ptr = StripLeadingTokenToBuffer( ptr, token );
			MakeLower( token );

			// Take different measures, depending on the command.  You will need to add more!
			if (!strcmp(token, "")) continue;
			else if (!strcmp(token,"camera"))  
				loadCamera( ptr, sceneFile);
			else if (!strcmp(token,"light")) 
				loadLights(ptr,sceneFile);
			else if (!strcmp(token,"object"))		
				loadObjModel( ptr, sceneFile );		
			else
				printf( "Unknown scene command '%s' in Scene::Scene()!\n", token );
		}
	}

	void SceneData::loadCamera(char* typeString, FILE* file)
	{
		char type[10];
		char* ptr = StripLeadingTokenToBuffer( typeString, type );
		if (!strcmp(type,"pinhole"))
		{
			_camera = CameraData(file);
			_camera.pinhole = true;
		}
		else
		{
			_camera = CameraData(file);
			_camera.pinhole = false;
		}

	}
	void SceneData::loadLights(char* type, FILE* file)
	{
		Light nl(file);
		_lights.push_back(nl);
	}

	void SceneData::loadObjModel(char* linePtr, FILE*file)
	{
		char token[256], *ptr;
		ptr = StripLeadingTokenToBuffer( linePtr, token );

		SceneObject* objPtr = NULL;
		//if (!strcmp(token,"sphere"))
		//	//UnhandledKeyword( file, "object type", token );
		//	objPtr = new Sphere( file, this );
		//else if (!strcmp(token,"parallelogram"))
		//	objPtr = new Quad( file, this );
		//else if (!strcmp(token,"texparallelogram"))
		//	objPtr = new Quad( file, this );
		//else if (!strcmp(token,"testquad"))
		//	objPtr = new Quad( file, this );
		//else if (!strcmp(token,"testdisplacedquad"))
		//	objPtr = new Quad( file, this );
		//else if (!strcmp(token,"noisyquad"))
		//	objPtr = new Quad( file, this );
		//else if (!strcmp(token,"triangle") || !strcmp(token,"tri"))
		//	objPtr = new Triangle( file, this );
		//else if (!strcmp(token,"textri"))
		//	objPtr = new Triangle( file, this );
		//else if (!strcmp(token,"cyl") || !strcmp(token,"cylinder"))
		//	objPtr = new Cylinder( file, this );
		if (!strcmp(token,"mesh"))
		{	
			ptr = StripLeadingTokenToBuffer( ptr, token );
			if (!strcmp(token,"obj"))
			{
				objPtr = new ObjModelObject(ptr, file );
			}

		}
		else if(!strcmp(token,"plane"))
			objPtr = new PlaneObject(ptr,file);
		/*else if (!strcmp(token,"group"))
		objPtr = new GroupObject( ptr, file );*/
		else
			printf("Unknown object type '%s' in LoadObject()!\n", token);

		if (objPtr)
		{
			_objs.push_back(objPtr);
		}
	}


	CameraData::CameraData(FILE* f):resX(0),resY(0)
	{
		// Search the scene file.
		char buf[ MAXLINELENGTH ], token[256], *ptr;
		while( fgets(buf, MAXLINELENGTH, f) != NULL )  
		{
			// Is this line a comment?
			ptr = StripLeadingWhiteSpace( buf );
			if (ptr[0] == '#') continue;

			// Nope.  So find out what the command is...
			ptr = StripLeadingTokenToBuffer( ptr, token );
			MakeLower( token );

			// Take different measures, depending on the command.
			if (!strcmp(token,"end")) break;
			if (!strcmp(token,"eye")) 
				eye = parseVec3(ptr);
			else if (!strcmp(token,"at")) 
				at = parseVec3(ptr);
			else if (!strcmp(token,"up")) 
				up = parseVec3(ptr);
			/*else if (!strcmp(token,"w") || !strcmp(token,"width") )  
			{
			ptr = StripLeadingTokenToBuffer( ptr, token );
			s->SetWidth( (int)atof( token ) );
			}
			else if (!strcmp(token,"h") || !strcmp(token,"height") )  
			{
			ptr = StripLeadingTokenToBuffer( ptr, token );
			s->SetHeight( (int)atof( token ) );
			}
			*/
			/*else if (!strcmp(token, "matrix"))
			{
			if (!mat) 
			mat = new Matrix4x4( f, ptr );
			else
			(*mat) *= Matrix4x4( f, ptr );
			}			*/
			else if (!strcmp(token,"fovy"))  
				ptr = StripLeadingNumber( ptr, &fovy );
			else if (!strcmp(token,"near"))  
				ptr = StripLeadingNumber( ptr, &zNear );
			else if (!strcmp(token,"far"))  
				ptr = StripLeadingNumber( ptr, &zFar );	
			else if (!strcmp(token,"res") || !strcmp(token, "resolution"))
			{
				ptr = StripLeadingTokenToBuffer( ptr, token );
				resX = ( (int)atof( token ) );
				ptr = StripLeadingTokenToBuffer( ptr, token );
				resY = ( (int)atof( token ) );
			}
			else
				printf("Unknown command '%s' when loading Camera!\n", token);
		}
	}

	Light::Light(FILE* f)
	{
		//默认产生阴影
		castShadow = 1;

		// Search the scene file.
		char buf[ MAXLINELENGTH ], token[256], *ptr;
		while( fgets(buf, MAXLINELENGTH, f) != NULL )  
		{
			// Is this line a comment?
			ptr = StripLeadingWhiteSpace( buf );
			if (ptr[0] == '#') continue;

			// Nope.  So find out what the command is...
			ptr = StripLeadingTokenToBuffer( ptr, token );
			MakeLower( token );

			// Take different measures, depending on the command.
			if (!strcmp(token,"end")) break;
			if (!strcmp(token,"pos")) 
				pos = parseVec3(ptr);
			else if (!strcmp(token,"at")) 
				at = parseVec3(ptr);
			else if (!strcmp(token,"color")) 
				color = parseVec3(ptr);	
			else if (!strcmp(token,"shadow")) 
			{
				float shadow = 1;
				ptr = StripLeadingNumber(ptr,&shadow);
				castShadow = shadow > 0 ? 1 : 0;
			}
			else
				printf("Unknown command '%s' when loading Light!\n", token);
		}
	}

	ObjModelObject::ObjModelObject(char* name, FILE* f)
	{
		_unitizeFlag = false;
		glm::mat4 m = glm::translate(glm::mat4(1.0),glm::vec3(5,0,0));
		glm::vec4 a(1.0,0.0,0.0,1.0);
		a = m*a;
		_type = SceneObjType::mesh;

		char buf[ MAXLINELENGTH ], token[256], *ptr;
		ptr = StripLeadingTokenToBuffer( name, token );
		_objName = strdup(token);
		// Now find out the other model parameters
		while( fgets(buf, MAXLINELENGTH, f) != NULL )  
		{
			// Is this line a comment?
			ptr = StripLeadingWhiteSpace( buf );
			if (ptr[0] == '#') continue;

			// Nope.  So find out what the command is...
			ptr = StripLeadingTokenToBuffer( ptr, token );
			MakeLower( token );

			// Take different measures, depending on the command.
			if (!strcmp(token,"end")) break;
			else if (!strcmp(token,"file"))
			{
				ptr = StripLeadingTokenToBuffer( ptr, token );
				_objFileName = strdup(token);
			}
			else if(!strcmp(token,"material"))
			{
				ptr = StripLeadingTokenToBuffer( ptr, token );
				_materialName = strdup(token);
			}
			else if (!strcmp(token,"matrix"))
			{
				ptr = StripLeadingTokenToBuffer( ptr, token );
				MakeLower( token );
				if (!strcmp(token,"translate"))
				{
					glm::vec3 tran = parseVec3(ptr);
					glm::mat4 m = glm::translate(glm::mat4(1.0),tran);
					_transform *= glm::translate(glm::mat4(1.0),tran);
				}
				else if(!strcmp(token,"rotate"))
				{
					float ang = 0;
					ptr = StripLeadingNumber(ptr,&ang);
					glm::vec3 axis = parseVec3(ptr);
					_transform *= glm::rotate(glm::mat4(1.0),ang,axis);
				}
				else if(!strcmp(token,"scale"))
				{
					glm::vec3 scale = parseVec3(ptr);
					_transform *= glm::scale(glm::mat4(1.f),scale);
				}

			}
			else if(!strcmp(token,"unitize"))
			{
				ptr = StripLeadingTokenToBuffer( ptr, token );
				MakeLower( token );
				if (!strcmp(token,"true"))
					_unitizeFlag = true;
			}
		}
	}

	PlaneObject::PlaneObject(char* name, FILE* f)
	{
		_type = SceneObjType::parallelogram;
		char buf[ MAXLINELENGTH ], token[256], *ptr;

		_objName = strdup(name);
		// Now find out the other model parameters
		while( fgets(buf, MAXLINELENGTH, f) != NULL )  
		{
			// Is this line a comment?
			ptr = StripLeadingWhiteSpace( buf );
			if (ptr[0] == '#') continue;

			// Nope.  So find out what the command is...
			ptr = StripLeadingTokenToBuffer( ptr, token );
			MakeLower( token );

			// Take different measures, depending on the command.
			if (!strcmp(token,"end")) break;
			else if (!strcmp(token,"anchor"))
			{
				_anchor = parseVec3(ptr);
			}
			else if(!strcmp(token,"material"))
			{
				ptr = StripLeadingTokenToBuffer( ptr, token );
				_materialName = strdup(token);
			}
			else if (!strcmp(token,"v1"))
			{
				_v1 = parseVec3(ptr);
			}
			else if (!strcmp(token,"v2"))
			{
				_v2 = parseVec3(ptr);
			}
			else if (!strcmp(token,"matrix"))
			{
				ptr = StripLeadingTokenToBuffer( ptr, token );
				MakeLower( token );
				if (!strcmp(token,"translate"))
				{
					glm::vec3 tran = parseVec3(ptr);
					_transform *= glm::translate(glm::mat4(1.0),tran);
				}
				else if(!strcmp(token,"rotate"))
				{
					float ang = 0;
					ptr = StripLeadingNumber(ptr,&ang);
					glm::vec3 axis = parseVec3(ptr);
					_transform *= glm::rotate(glm::mat4(1.0),ang,axis);
				}
				else if(!strcmp(token,"scale"))
				{
					glm::vec3 scale = parseVec3(ptr);
					_transform *= glm::scale(glm::mat4(1.f),scale);
				}
			}

		}
	}
}