#include "main.h"
#define OBJBIN_LOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


 
int main()
{
	context_load();
	gl_load();
	gl_init();
	init();

	while (!glfwWindowShouldClose(window))
	{
		input_loop(&input);
		{	
			const float viewSpeed = 0.05f;
			vec3 viewRight; glm_vec3_crossn(viewFront,viewUp,viewRight);
			vec3 viewFrontScaled; glm_vec3_scale(viewFront,viewSpeed,viewFrontScaled);
			vec3 viewRightScaled; glm_vec3_scale(viewRight,viewSpeed,viewRightScaled);

			if (input.wPress)
				glm_vec3_add(viewPos,viewFrontScaled,viewPos);
			if (input.sPress)
				glm_vec3_sub(viewPos,viewFrontScaled,viewPos);
			if (input.aPress)
				glm_vec3_sub(viewPos,viewRightScaled,viewPos);
			if (input.dPress)
				glm_vec3_add(viewPos,viewRightScaled,viewPos);

			viewRotX += glm_rad(input.yAxisL);
			viewRotY += glm_rad(input.xAxisL);
		}

		vec3 viewPosAddFront; glm_vec3_add(viewPos,viewFront,viewPosAddFront);
		glm_lookat(viewPos,viewPosAddFront,viewUp,view);
		glm_translate(view,(vec3){0.f,-2.f,0.f});
		glm_rotate_x(view,viewRotX,view);
		glm_rotate_y(view,viewRotY,view);

		vec4 frustumCorners[8];
		vec4 cascadeCorners[2][4];
		mat4 viewProj; glm_mat4_mul(projection,view,viewProj);
		mat4 invViewProj; glm_mat4_inv(viewProj,invViewProj);
		glm_frustum_corners(invViewProj,frustumCorners);
		for (int i=0;i<2;i++)
		{
			glm_frustum_corners_at(frustumCorners,
					(i+1)*((FAR-NEAR)/3.f),
					1000.f,cascadeCorners[i]);
		}

		vec3 l; glm_vec3_scale(lightDir,1.f/glm_vec3_norm(lightDir),l);
		vec3 lightZAxis; glm_vec3_cross(l,(vec3){0.f,0.f,1.f},lightZAxis);
		float lightZAngle = glm_vec3_dot(l,(vec3){0.f,0.f,1.f});
		lightZAngle = acos(lightZAngle);
		printf("l: %f lzangle: %f lzaxis: %f\n",l[0],lightZAngle,lightZAxis[0]);
		

		mat4 shadowProjection,shadowView;
		glm_mat4_identity(shadowProjection);
		glm_ortho(-10.f,10.f,-10.f,10.f,-7.5f,7.5f,shadowProjection);
		glm_mat4_identity(shadowView);
		vec3 lightDirPos; glm_vec3_scale(lightDir,-10.f,lightDirPos);
		glm_lookat(lightDirPos,(vec3){0.f,0.f,0.f},(vec3){0.f,1.f,0.f},shadowView);
		glm_mat4_mul(shadowProjection,shadowView,shadowSpaceMatrix);


		/* SHADOW PASS */
		glBindFramebuffer(GL_FRAMEBUFFER,shadowFBO);
		glViewport(0,0,SHADOW_WIDTH,SHADOW_HEIGHT);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);
		glUseProgram(modelShadowShader);
		glUniformMatrix4fv(glGetUniformLocation(modelShadowShader,"shadowSpaceMatrix"),1,GL_FALSE,(GLfloat*)shadowSpaceMatrix);
		{
			/* MARIO */
			glBindVertexArray(mario.VAO);
			glUniformMatrix4fv(glGetUniformLocation(modelShadowShader,"model"),1,GL_FALSE,(GLfloat*)marioMatrix);
			glDrawElements(GL_TRIANGLES,mario.faceAmt*3,GL_UNSIGNED_INT,(void*)0);
			/* FLOOR */
			glBindVertexArray(quadVAO);
			glUniformMatrix4fv(glGetUniformLocation(modelShadowShader,"model"),1,GL_FALSE,(GLfloat*)floorMatrix);
			glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,(void*)0);
		}


		/* LINEAR PASS */
		glBindFramebuffer(GL_FRAMEBUFFER,linearFBO);
		glViewport(0,0,SCR_WIDTH,SCR_HEIGHT);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.1f,0.1f,0.1f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		{
			/* MARIO */
			glUseProgram(modelPhongShader);
			glBindVertexArray(mario.VAO);
			{
				glUniformMatrix4fv(glGetUniformLocation(modelPhongShader,"model"),1,GL_FALSE,(GLfloat*)marioMatrix);
				glUniformMatrix4fv(glGetUniformLocation(modelPhongShader,"view"),1,GL_FALSE,(GLfloat*)view);
				glUniformMatrix4fv(glGetUniformLocation(modelPhongShader,"projection"),1,GL_FALSE,(GLfloat*)projection);
				glUniform1i(glGetUniformLocation(modelPhongShader,"material.specularMapOn"),0);
				glUniform1f(glGetUniformLocation(modelPhongShader,"material.shininess"),1.f);
				glUniform4fv(glGetUniformLocation(modelPhongShader,"material.specular"),1,(GLfloat[4]){0.f,0.f,0.f,0.f});
				glUniform3fv(glGetUniformLocation(modelPhongShader,"dirLight.direction"),1,lightDir);
				glUniform3fv(glGetUniformLocation(modelPhongShader,"dirLight.ambient"),1,(GLfloat[3]){0.4f,0.4f,0.4f});
				glUniform3fv(glGetUniformLocation(modelPhongShader,"dirLight.diffuse"),1,(GLfloat[3]){1.f,1.f,1.f});
				glUniform3fv(glGetUniformLocation(modelPhongShader,"dirLight.specular"),1,(GLfloat[3]){1.f,1.f,1.f});
				for (int i=0;i<mario.numGroups;i++)
				{
					#define GROUP (mario.groups[i])
					for (int j=0;j<GROUP.numFacePtrs;j++)
					{
						glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D,mario.materials[GROUP.IDs[j]].diffuseMap);
						glUniform1i(glGetUniformLocation(modelPhongShader,"material.diffuseMapOn"),
								(mario.materials[GROUP.IDs[j]].diffuseMap != 0));
						glUniform1i(glGetUniformLocation(modelPhongShader,"material.diffuseMap"),0);
						glUniform4fv(glGetUniformLocation(modelPhongShader,"material.diffuse"),1,mario.materials[GROUP.IDs[j]].diffuse);
						glDrawElements(GL_TRIANGLES,GROUP.faceAmts[j]*3,GL_UNSIGNED_INT,(void*)(GROUP.facePtrs[j]*3*sizeof(unsigned int)));
					}
					#undef GROUP
				}
			}
			/* FLOOR */
			glUseProgram(floorShader);
			glBindVertexArray(quadVAO);
			{
				glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D,floorTexture);
				glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D,shadowTexture);
				glUniformMatrix4fv(glGetUniformLocation(floorShader,"model"),1,GL_FALSE,(GLfloat*)floorMatrix);
				glUniformMatrix4fv(glGetUniformLocation(floorShader,"view"),1,GL_FALSE,(GLfloat*)view);
				glUniformMatrix4fv(glGetUniformLocation(floorShader,"projection"),1,GL_FALSE,(GLfloat*)projection);
				glUniformMatrix4fv(glGetUniformLocation(floorShader,"shadowSpaceMatrix"),1,GL_FALSE,(GLfloat*)shadowSpaceMatrix);
				glUniform3fv(glGetUniformLocation(floorShader,"lightDirection"),1,lightDir);
				glUniform1i(glGetUniformLocation(floorShader,"diffuseMap"),0);
				glUniform1i(glGetUniformLocation(floorShader,"shadowMap"),1);
				glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,(void*)0);
			}
		}


		glBindFramebuffer(GL_READ_FRAMEBUFFER,linearFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER,postFBO);
		glBlitFramebuffer(0,0,SCR_WIDTH,SCR_HEIGHT,0,0,SCR_WIDTH,SCR_HEIGHT,GL_COLOR_BUFFER_BIT,GL_NEAREST);


		/* VIEW SRGB */
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glDisable(GL_DEPTH_TEST);
		glUseProgram(screenShader);
		glBindVertexArray(quadVAO);
		{
			glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D,postTexture);
			glUniform1i(glGetUniformLocation(screenShader,"screen"),0);
			glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,(void*)0);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
		GLenum i; while ((i = glGetError()) != GL_NO_ERROR)
			printf("loop: %x\n",i);
	}

	end();
	return EXIT_SUCCESS;
}



void context_load()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);
	glfwWindowHint(GLFW_SAMPLES,4);
	if ((window = glfwCreateWindow(SCR_WIDTH,SCR_HEIGHT,"Colliders",NULL,NULL)) == NULL)
		exit(EXIT_FAILURE);
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		exit(EXIT_FAILURE);
}



void gl_init()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_CULL_FACE);

	/* FRAMEBUFFERS */
	glGenFramebuffers(1,&linearFBO);
	glBindFramebuffer(GL_FRAMEBUFFER,linearFBO);
	{
		glGenTextures(1,&linearTexture);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE,linearTexture);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,4,GL_RGBA,SCR_WIDTH,SCR_HEIGHT,GL_TRUE);
		glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D_MULTISAMPLE,linearTexture,0);
		
		glGenRenderbuffers(1,&linearRBO);
		glBindRenderbuffer(GL_RENDERBUFFER,linearRBO);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER,4,GL_DEPTH24_STENCIL8,SCR_WIDTH,SCR_HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,GL_RENDERBUFFER,linearRBO);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			exit(EXIT_FAILURE);
	}
	glGenFramebuffers(1,&postFBO);
	glBindFramebuffer(GL_FRAMEBUFFER,postFBO);
	{
		glGenTextures(1,&postTexture);
		glBindTexture(GL_TEXTURE_2D,postTexture);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,SCR_WIDTH,SCR_HEIGHT,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,postTexture,0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			exit(EXIT_FAILURE);
	}
	glGenFramebuffers(1,&shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER,shadowFBO);
	{
		glGenTextures(1,&shadowTexture);
		glBindTexture(GL_TEXTURE_2D,shadowTexture);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
		glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,(GLfloat[4]){1.f,1.f,1.f,1.f});
		glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT,SHADOW_WIDTH,SHADOW_HEIGHT,0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,shadowTexture,0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			exit(EXIT_FAILURE);
	}

	/* VAOS */
	float quadVertices[] = {
		1.f,1.f,0.f,	1.f,1.f,	0.f,0.f,1.f,
		-1.f,-1.f,0.f,	0.f,0.f,	0.f,0.f,1.f,
		1.f,-1.f,0.f,	1.f,0.f,	0.f,0.f,1.f,
		-1.f,1.f,0.f,	0.f,1.f,	0.f,0.f,1.f
	};
	unsigned int quadIndices[] = {
		0,1,2,
		0,3,1
	};
	glGenVertexArrays(1,&quadVAO);
	glBindVertexArray(quadVAO);
	glGenBuffers(1,&quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER,quadVBO);
	glBufferData(GL_ARRAY_BUFFER,sizeof(quadVertices),quadVertices,GL_STATIC_DRAW);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(5*sizeof(float)));
	glEnableVertexAttribArray(2);
	glGenBuffers(1,&quadEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,quadEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(quadIndices),quadIndices,GL_STATIC_DRAW);


	float cubeVertices[] = {
	    // positions          // normals           // texture coords
	    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
	    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

	    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
	     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
	     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
	    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

	    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
	    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

	    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
	    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};
	glGenVertexArrays(1,&cubeVAO);
	glBindVertexArray(cubeVAO);
	glGenBuffers(1,&cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER,cubeVBO);
	glBufferData(GL_ARRAY_BUFFER,sizeof(cubeVertices),cubeVertices,GL_STATIC_DRAW);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(6*sizeof(float)));
	glEnableVertexAttribArray(2);

	glGenVertexArrays(1,&lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER,cubeVBO);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);

	GLenum i; while ((i = glGetError()) != GL_NO_ERROR)
		printf("gl_init: %x\n",i);
}
void gl_load()
{
	/* SHADERS */
	if (!(floorShader = shaderprog_load("asset/shd/floor.vert",NULL,"asset/shd/floor.frag")))
		exit(EXIT_FAILURE);
	if (!(cubeShader = shaderprog_load("asset/shd/cube.vert",NULL,"asset/shd/cube.frag")))
		exit(EXIT_FAILURE);
	if (!(lightShader = shaderprog_load("asset/shd/light.vert",NULL,"asset/shd/light.frag")))
		exit(EXIT_FAILURE);
	if (!(modelPhongShader = shaderprog_load("asset/shd/modelPhong.vert",NULL,"asset/shd/modelPhong.frag")))
		exit(EXIT_FAILURE);
	if (!(modelShadowShader = shaderprog_load("asset/shd/modelShadow.vert",NULL,"asset/shd/modelShadow.frag")))
		exit(EXIT_FAILURE);
	if (!(screenShader = shaderprog_load("asset/shd/screen.vert",NULL,"asset/shd/screen.frag")))
		exit(EXIT_FAILURE);

	/* MODELS */
	if (!(model_load(&mario,"asset/obj/Mario/mario")))
		exit(EXIT_FAILURE);

	/* TEXTURES */
	stbi_set_flip_vertically_on_load(1);
	int w,h,n;
	unsigned char *data;
	if ((data = stbi_load("asset/tex/get.png",&w,&h,&n,0)) == NULL)
		exit(EXIT_FAILURE);
	glGenTextures(1,&floorTexture);
	glBindTexture(GL_TEXTURE_2D,floorTexture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,0,GL_SRGB_ALPHA,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	if ((data = stbi_load("asset/tex/container2.png",&w,&h,&n,0)) == NULL)
		exit(EXIT_FAILURE);
	glGenTextures(1,&crateDiffuse);
	glBindTexture(GL_TEXTURE_2D,crateDiffuse);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D,0,GL_SRGB_ALPHA,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	if ((data = stbi_load("asset/tex/container2_specular.png",&w,&h,&n,0)) == NULL)
		exit(EXIT_FAILURE);
	glGenTextures(1,&crateSpecular);
	glBindTexture(GL_TEXTURE_2D,crateSpecular);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D,0,GL_SRGB_ALPHA,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	GLenum i; while ((i = glGetError()) != GL_NO_ERROR)
		printf("gl_load: %x\n",i);
}
void init()
{
	input_init(&input);

	glm_mat4_identity(projection);
	glm_perspective(glm_rad(45.f),SCR_WIDTH/SCR_HEIGHT,0.1f,1000.f,projection);



	glm_scale(marioMatrix,(vec3){0.01f,0.01f,0.01f});
	glm_mat4_identity(lightMatrix);
	glm_translate(lightMatrix,lightPos);
	glm_scale(lightMatrix,(vec3){0.2f,0.2f,0.2f});

	glm_rotate_x(floorMatrix,glm_rad(-90.f),floorMatrix);
	glm_scale(floorMatrix,(vec3){1000.f,1000.f,1000.f});
}
void end()
{
	/* TEXTURES */
	glDeleteTextures(1,&floorTexture);
	glDeleteTextures(1,&crateDiffuse);
	glDeleteTextures(1,&crateSpecular);

	/* MODELS */
	model_free(&mario);

	/* FRAMEBUFFERS */
	glDeleteFramebuffers(1,&linearFBO);
	glDeleteTextures(1,&linearTexture);
	glDeleteRenderbuffers(1,&linearRBO);
	glDeleteFramebuffers(1,&postFBO);
	glDeleteTextures(1,&postTexture);

	/* VAOS */
	glDeleteVertexArrays(1,&quadVAO);
	glDeleteBuffers(1,&quadVBO);
	glDeleteBuffers(1,&quadEBO);
	glDeleteVertexArrays(1,&cubeVAO);
	glDeleteBuffers(1,&cubeVBO);

	/* SHADERS */
	glDeleteProgram(floorShader);
	glDeleteProgram(cubeShader);
	glDeleteProgram(lightShader);
	glDeleteProgram(modelPhongShader);
	glDeleteProgram(modelShadowShader);
	glDeleteProgram(screenShader);
	
	glfwTerminate();
}
