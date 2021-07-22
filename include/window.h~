#ifndef WINDOW_H
#define WINDOW_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define DEADZONE (0.05f)
typedef struct
{
	float xAxisL;
	float yAxisL;
	float xAxisR;
	float yAxisR;
	unsigned int escPress;
	unsigned int lMouseClick;
	unsigned int lMouseHold;
	double mouseX;
	double mouseY;
} Input;
void input_init(Input*);
void input_loop(Input*);

GLFWwindow *window;
Input input;


#endif
