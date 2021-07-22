#ifndef GL_MODEL_H
#define GL_MODEL_H
#include <glad/glad.h>
#include <cglm/cglm.h>



typedef union
{
	struct
	{
		unsigned int ID;
		GLuint *textures;
		unsigned int numTextures;
		GLfloat diffuse[4];
		unsigned int diffuseMapOn;
		GLuint specularMap;
		GLfloat specular[4];
		unsigned int specularMapOn;
	}
} Material;
typedef union
{
	struct
	{
		GLuint vao;
		GLuint shader;
		mat4 matrix;
		unsigned int numNumFacePtrs;
		unsigned int *numFacePtrs;
		unsigned int *facePtrs;
		unsigned int *faceAmts;
		Material *materials;
	};
} Renderable;
