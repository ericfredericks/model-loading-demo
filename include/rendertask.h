#ifndef GL_RENDERTASK_H
#define GL_RENDERTASK_H
#include <glad/glad.h>



#define DIFFUSE_MAP 0
#define SPECULAR_MAP 1
#define NORMAL_MAP 2
#define MAX_TEXTURES 3
typedef union
{ struct
{
	GLuint vao;
	GLuint shader;
	GLuint ubo;
	GLuint baseIndex;
	GLuint numIndices;
	GLenum primitive;
	GLuint textures[MAX_TEXTURES];
	GLint texturesOn[MAX_TEXTURES];
};
} RenderTask;



#endif
