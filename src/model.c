#include "model.h" 
#define OBJBIN_LOADER_IMPLEMENTATION  
#include <objbin_loader.h>
#define mVAO (this->VAO)
#define mVBO (this->VBO)
#define mEBO (this->EBO)
#define mFaceAmt (this->faceAmt)
#define mGroups (this->groups)
#define mNumGroups (this->numGroups)
#define mMaterials (this->materials)
#define mNumMaterials (this->numMaterials)




int model_load(Model *this,const char *filename)
{
	memset(this,0,sizeof(Model));

	Objbin preModel;
	if (!objbin_load(&preModel,filename))
		return 0;

	glGenVertexArrays(1,&mVAO);
	glBindVertexArray(mVAO);

	glGenBuffers(1,&mVBO);
	glBindBuffer(GL_ARRAY_BUFFER,mVBO);
	glBufferData(GL_ARRAY_BUFFER,preModel.sizeVertices,preModel.vertices,GL_STATIC_DRAW);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(5*sizeof(float)));
	glEnableVertexAttribArray(2);

	glGenBuffers(1,&mEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,preModel.sizeIndices,preModel.indices,GL_STATIC_DRAW);

	mFaceAmt = preModel.sizeIndices/(sizeof(unsigned int)*3);

	mGroups = malloc(sizeof(Group)*preModel.numGroups);
	if (mGroups == NULL)
	{
		printf("Model: mGroups alloc\n");
		model_free(this);
		return 0;
	}
	for (int i=0;i<preModel.numGroups;i++)
	{
		if (!group_load(&mGroups[i],&preModel,i))
		{
			printf("Model: mGroups load\n");
			model_free(this);
			return 0;
		}
		mNumGroups++;
	}

	mMaterials = malloc(sizeof(Material)*preModel.numMaterials);
	if (mMaterials == NULL)
	{
		printf("Model: mMaterials alloc\n");
		model_free(this);
		return 0;
	}
	material_init();
	char *path;
	path = calloc(strlen(filename)+1,1);
	strcpy(path,filename);
	char *pathEnd = strrchr(path,'/');
	path[pathEnd-path+1]='\0';
	for (int i=0;i<preModel.numMaterials;i++)
	{
		if (!material_load(&mMaterials[i],&preModel,i,path))
		{
			printf("Model: mMaterials load\n");
			free(path);
			model_free(this);
			return 0;
		}
		mNumMaterials++;
	}
	free(path);

	objbin_free(&preModel);
	return 1;
}
void model_free(Model *this)
{
	for (int i=0;i<mNumGroups;i++)
		group_free(&mGroups[i]);
	free(mGroups);
	for (int i=0;i<mNumMaterials;i++)
		material_free(&mMaterials[i]);
	free(mMaterials);
	glDeleteVertexArrays(1,&mVAO);
	glDeleteBuffers(1,&mVBO);
	glDeleteBuffers(1,&mEBO);
}



#undef mVAO
#undef mVBO
#undef mEBO
#undef mGroups
#undef mNumGroups
#undef mMaterials
#undef mNumMaterials
#undef mFaceAmt
