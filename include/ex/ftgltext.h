#ifndef FTGLTEXTBUFFER_H
#define FTGLTEXTBUFFER_H
#include <string.h>
#include <glad/glad.h>
#define GL_WITH_GLAD
#include <freetype-gl/freetype-gl.h>
#include <cglm/cglm.h>



typedef struct
{
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
	unsigned int indicesSize;
	mat4 modelMatrix;
} FTGLtext;
int ftgltext_load(FTGLtext*,const char*,texture_font_t*,ftgl_vec4);
void ftgltext_draw(FTGLtext*,GLuint,GLint,GLint,mat4*,mat4*);
void ftgltext_free(FTGLtext*);



#endif
