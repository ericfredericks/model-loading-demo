#include "window.h"
#define mXAxisL (this->xAxisL)
#define mYAxisL (this->yAxisL)
#define mXAxisR (this->xAxisR)
#define mYAxisR (this->yAxisR)
#define mEscPress (this->escPress)
#define mLMouseClick (this->lMouseClick)
#define mLMouseHold (this->lMouseHold)
#define mMouseX (this->mouseX)
#define mMouseY (this->mouseY)


void input_init(Input *this)
{
	memset(this,0,sizeof(Input));
}
void input_loop(Input *this)
{
	mXAxisL = mYAxisL = 0.f;
	mXAxisR = mYAxisR = 0.f;
	GLFWgamepadstate state;
	if (glfwGetGamepadState(GLFW_JOYSTICK_1,&state))
	{
		mXAxisL = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
		mYAxisL = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
		mXAxisR = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
		mYAxisR = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
		if (fabsf(mXAxisL) < DEADZONE)
			mXAxisL = 0.f;
		if (fabsf(mYAxisL) < DEADZONE)
			mYAxisL = 0.f;
		if (fabsf(mXAxisR) < DEADZONE)
			mXAxisR = 0.f;
		if (fabsf(mYAxisR) < DEADZONE)
			mYAxisR = 0.f;
	}
	mXAxisL -= (glfwGetKey(window,GLFW_KEY_LEFT) == GLFW_PRESS);
	mXAxisL += (glfwGetKey(window,GLFW_KEY_RIGHT) == GLFW_PRESS);
	mYAxisL -= (glfwGetKey(window,GLFW_KEY_UP) == GLFW_PRESS);
	mYAxisL += (glfwGetKey(window,GLFW_KEY_DOWN) == GLFW_PRESS);

	mEscPress = (glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS);

	mLMouseHold = mLMouseClick;
	mLMouseClick = (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
		
	glfwGetCursorPos(window,&mMouseX,&mMouseY);
}


#undef mXAxisL
#undef mYAxisL
#undef mXAxisR
#undef mYAxisR
#undef mEscPress
#undef mLMouseClick
#undef mLMouseHold
#undef mMouseX
#undef mMouseY
