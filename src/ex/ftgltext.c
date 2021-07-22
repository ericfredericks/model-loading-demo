#include "ftgltext.h"
#define mVAO (this->VAO)
#define mVBO (this->VBO)
#define mEBO (this->EBO)
#define mIndicesSize (this->indicesSize)
#define mModelMatrix (this->modelMatrix)



static int ftgltext_add(char**,unsigned int*,unsigned int*,
		char**,unsigned int*,unsigned int*,
		texture_font_t*,const char*,ftgl_vec2,ftgl_vec4);



int ftgltext_load(FTGLtext *this,const char *text,
		texture_font_t *font,ftgl_vec4 color)
{
	memset(this,0,sizeof(FTGLtext));

	char *vertices,*indices;
	if ((vertices = malloc(1)) == NULL)
		goto error;
	if ((indices = malloc(1)) == NULL)
		goto error;
	unsigned int verticesSize = 0;
	unsigned int indicesSize = 0;
	unsigned int verticesBufferSize = 0;
	unsigned int indicesBufferSize = 0;

	if (!ftgltext_add(&vertices,&verticesBufferSize,&verticesSize,
			&indices,&indicesBufferSize,&indicesSize,
			font,text,(ftgl_vec2){{0.f,0.f}},color))
		goto error;

	glGenVertexArrays(1,&mVAO);
	glBindVertexArray(mVAO);
	glGenBuffers(1,&mVBO);
	glBindBuffer(GL_ARRAY_BUFFER,mVBO);
	glBufferData(GL_ARRAY_BUFFER,verticesSize,vertices,GL_STATIC_DRAW);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,9*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,9*sizeof(float),(void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2,4,GL_FLOAT,GL_FALSE,9*sizeof(float),(void*)(5*sizeof(float)));
	glEnableVertexAttribArray(2);
	glGenBuffers(1,&mEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,indicesSize,indices,GL_STATIC_DRAW);

	mIndicesSize = indicesSize/sizeof(unsigned int);

	glm_mat4_identity(mModelMatrix);

	free(vertices);
	free(indices);

	return 1;
error:
	printf("ftgltext_load: error\n");
	return 0;
}
void ftgltext_draw(FTGLtext *this,GLuint shader,GLint atlasUnit,
		GLint atlasID,mat4 *viewMatrix,mat4 *projectionMatrix)
{
	glUseProgram(shader);
	glActiveTexture(GL_TEXTURE0+atlasUnit);
	glBindTexture(GL_TEXTURE_2D,atlasID);
	glUniform1i(glGetUniformLocation(shader,"atlas"),atlasUnit);
	glUniformMatrix4fv(glGetUniformLocation(shader,"model"),1,GL_FALSE,(GLfloat*)mModelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(shader,"view"),1,GL_FALSE,(GLfloat*)*viewMatrix);
	glUniformMatrix4fv(glGetUniformLocation(shader,"projection"),1,GL_FALSE,(GLfloat*)*projectionMatrix);
	glBindVertexArray(mVAO);
	glDrawElements(GL_TRIANGLES,mIndicesSize,GL_UNSIGNED_INT,0);
}
void ftgltext_free(FTGLtext *this)
{
	glDeleteVertexArrays(1,&mVAO);
	glDeleteBuffers(1,&mVBO);
	glDeleteBuffers(1,&mEBO);
}



static int ftgltext_add(char **vertices,unsigned int *verticesBufferSize,unsigned int *verticesSize,
		char **indices,unsigned int *indicesBufferSize,unsigned int *indicesSize,
		texture_font_t *font,const char *text,ftgl_vec2 pen,ftgl_vec4 color)
{
	for (int i=0;i<strlen(text);i++)
	{
		texture_glyph_t *glyph = texture_font_get_glyph(font,text+i);
		float kerning = 0.f;
		if (i>0)
			kerning = texture_glyph_get_kerning(glyph,text+i-1);
		pen.x += kerning;

		float x0 = pen.x + glyph->offset_x;
		float y0 = pen.y + glyph->offset_y;
		float x1 = x0 + glyph->width;
		float y1 = y0 - glyph->height;
		float s0 = glyph->s0;
		float s1 = glyph->s1;
		float t0 = glyph->t0;
		float t1 = glyph->t1;

		unsigned int index = (*verticesSize)/(9*sizeof(float));
		unsigned int nIndices[] = {
			index,index+1,index+2,
			index,index+2,index+3};
		float nVertices[] = {
			x0,y0,0,s0,t0,color.r,color.g,color.b,color.a,
			x0,y1,0,s0,t1,color.r,color.g,color.b,color.a,
			x1,y1,0,s1,t1,color.r,color.g,color.b,color.a,
			x1,y0,0,s1,t0,color.r,color.g,color.b,color.a};
			
		if (*verticesSize+(4*9*sizeof(float)) > *verticesBufferSize)
		{
			while (*verticesSize+(4*9*sizeof(float)) > *verticesBufferSize)
				*verticesBufferSize += 1000;
			if ((*vertices = realloc(*vertices,*verticesBufferSize)) == NULL)
				goto error;
		}
		memcpy(*vertices+*verticesSize,nVertices,4*9*sizeof(float));
		*verticesSize += 4*9*sizeof(float);

		if (*indicesSize+(2*3*sizeof(unsigned int)) > *indicesBufferSize)
		{
			while (*indicesSize+(2*3*sizeof(unsigned int)) > *indicesBufferSize)
				*indicesBufferSize += 1000;
			if ((*indices = realloc(*indices,*indicesBufferSize)) == NULL)
				goto error;
		}
		memcpy(*indices+*indicesSize,nIndices,2*3*sizeof(unsigned int));
		*indicesSize += 2*3*sizeof(unsigned int);

		pen.x += glyph->advance_x;
	}

	return 1;
error:
	printf("ftgltext_add: error\n");
	return 0;
}



#undef mVAO
#undef mVBO
#undef mEBO
#undef mIndicesSize
#undef mModelMatrix
