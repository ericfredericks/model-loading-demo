#ifndef MODEL_H
#define MODEL_H
#include <string.h>
#include <glad/glad.h>
#include "group.h"
#include "material.h"



typedef struct
{
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
	unsigned int faceAmt;
	Group *groups;
	unsigned int numGroups;
	Material *materials;
	unsigned int numMaterials;
} Model;
int model_load(Model*,const char*);
void model_free(Model*);



#endif
