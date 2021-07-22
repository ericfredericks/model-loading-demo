#ifndef SHAPE_H
#define SHAPE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glad/glad.h>
#include <objbin_loader.h>



typedef struct
{
	unsigned int numFacePtrs;
	unsigned int *facePtrs;
	unsigned int *IDs;
	unsigned int *faceAmts;
} Group;
int group_load(Group*,Objbin*,int); 
void group_free(Group*);


#endif
