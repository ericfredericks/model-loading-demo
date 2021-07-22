#ifndef MAIN_H
#define MAIN_H

#include <math.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include "model.h"

#define PI (3.14159265359)
#define SCR_WIDTH 800.f
#define SCR_HEIGHT 600.f
#define NEAR 0.1f
#define FAR 1000.f
#define SHADOW_WIDTH 1024.f
#define SHADOW_HEIGHT 1024.f



vec3 viewPos = {0.f,0.f,10.f};
vec3 viewFront = {0.f,0.f,-1.f};
vec3 viewUp = {0.f,1.f,0.f};
float viewRotX = 0.f;
float viewRotY = 0.f;
mat4 view = GLM_MAT4_IDENTITY_INIT;
mat4 projection = GLM_MAT4_IDENTITY_INIT;



/* FRAMEBUFFERS */
GLuint linearFBO;
GLuint linearTexture;
GLuint linearRBO;
GLuint postFBO;
GLuint postTexture;



GLuint *shaders;
unsigned int num_shaders = 0;
GLuint *vaos;
unsigned int num_vaos = 0;
Renderable *opaque_queue;
unsigned int opaque_queue_length;
Renderable *transparent_queue;
unsigned int transparent_queue_length;



void context_load();
void gl_load();
void gl_init();
void init();
void end();



#endif
