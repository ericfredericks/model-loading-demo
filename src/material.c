#include "material.h"
#define mID (this->ID)
#define mDiffuse (this->diffuse)
#define mDiffuseMap (this->diffuseMap)



static int setUpTexture(GLuint*,const char*,const char*,float[4]);



void material_init()
{
	stbi_set_flip_vertically_on_load(1);
}
int material_load(Material *this,Objbin *preModel,int materialIndex,const char *path)
{
	memset(this,0,sizeof(Material));

	mID = materialIndex;
	memcpy(mDiffuse,preModel->diffuse+(materialIndex*3),3*sizeof(float));
	mDiffuse[0] = (float)pow((double)mDiffuse[0],2.2);
	mDiffuse[1] = (float)pow((double)mDiffuse[1],2.2);
	mDiffuse[2] = (float)pow((double)mDiffuse[2],2.2);
	mDiffuse[3] = 1.f;
	
	char *diffuseMapName = preModel->diffuseMaps+preModel->diffuseMapPtrs[materialIndex];
	if (diffuseMapName[0]!='.')
	{
		if (!setUpTexture(&mDiffuseMap,diffuseMapName,path,mDiffuse))
			goto error;
	}
	return 1;
error:
	free(diffuseMapName);
	material_free(this);
	return 0;
}
void material_free(Material *this)
{
	glDeleteTextures(1,&mDiffuseMap);
}


#undef mID
#undef mDiffuse
#undef mDiffuseMap


static int setUpTexture(GLuint (*out),const char *filename,const char *path,float borderColor[4])
{
	int w,h,n;
	unsigned char *data;

	char *stbiFile = malloc(strlen(path)+strlen(filename)+1);
	if (stbiFile == NULL)
		return 0;
	strcpy(stbiFile,path);
	strcat(stbiFile,filename);
	stbiFile[strlen(path)+strlen(filename)-1]='\0';

	data = stbi_load(stbiFile,&w,&h,&n,0);
	if (data == NULL)
	{
		printf("stb_image: %s\n",stbi_failure_reason());
		return 0;
	}

	GLenum internalFormat = GL_RGB;
	if (n == 4)
		internalFormat = GL_RGBA;

	glGenTextures(1,out);
	glBindTexture(GL_TEXTURE_2D,(*out));
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,borderColor);
	glTexImage2D(GL_TEXTURE_2D,0,GL_SRGB_ALPHA,w,h,0,internalFormat,GL_UNSIGNED_BYTE,data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
	free(stbiFile);
	return 1;
}




