#include "cubemap.h"
#include <stb_image.h>



GLuint cubemap_load(const char **filenames)
{
	GLuint id;
	glGenTextures(1,&id);
	glBindTexture(GL_TEXTURE_CUBE_MAP,id);
	stbi_set_flip_vertically_on_load(0);
	for (int i=0;i<6;i++)
	{
		int w,h,n;
		unsigned char *data;
		if ((data = stbi_load(filenames[i],&w,&h,&n,0)) == NULL)
			exit(EXIT_FAILURE);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0,GL_RGB,w,h,0,GL_RGB,GL_UNSIGNED_BYTE,data);
		stbi_image_free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
	return id;
}
