//
// Created by bunring on 19-3-11.
//

//#if IS_WINDOWS || IS_MACOS

#include <GL/glew.h>
#include "common/P3D_Utils.h"
#include"common/RenderCommon.h"
#include "OGLWIidow.h"
#include "OGLCommon.h"
#include "OGLDefaultRenderFrame.hpp"
#include "P3dEngine.h"
BEGIN_P3D_NS
GLWindow::~GLWindow() {
	release();
}

GLWindow::GLWindow(uint32 width, uint32 height):P3DWindow(width,height) {
	appType = APP_OPENGL;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	
	//glfwWindowHint(GLFW_SAMPLES, 128);
	//glfwWindowHint(GLFW_SAMPLES, 4);
	//glfwSwapInterval(1);
	//int fs = fullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW;

	win = glfwCreateWindow(width, height, "Facial", nullptr, nullptr);

	if (win == nullptr) {
		logOut.log("Could not create glfw Window \n");
		char* tmpError;
		glfwGetError((const char**)&tmpError);
		int aaa = 0;
	}
	glfwMakeContextCurrent(win);
	glfwSetInputMode(win, GLFW_STICKY_KEYS, GLFW_TRUE);
	glfwSetInputMode(win, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
	//glfwSetKeyCallback(window, Engine::keyCallback);
	//glfwSetMouseButtonCallback(window, Engine::mouseButtonCallback);
	//glfwSetCursorPosCallback(window, Engine::cursorPosCallback);
	//glfwSetScrollCallback(window, scrollCallback);
	if (glewInit() != GLEW_OK) {
		logOut.log("create glew error");
	}
	logOut.log(Utils::makeStr("Successfully created OpenGL-window, version %d.%d",
		glfwGetWindowAttrib(win, GLFW_CONTEXT_VERSION_MAJOR),
		glfwGetWindowAttrib(win, GLFW_CONTEXT_VERSION_MINOR)));

	logOut.log(Utils::makeStr("GLSL-version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION)));

	float lineRange[10];
	glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineRange);
	glGetFloatv(GL_SMOOTH_LINE_WIDTH_GRANULARITY,&(lineRange[5]));
	glGetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE,&(lineRange[8]));
	//glEnable(GL_MULTISAMPLE);
	//glDisable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glEnable(GL_ALPHA_TEST);
	
	glEnable(GL_BLEND);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);




	//glLineWidth(1.0);
	GLUtils::init();
	auto defaultFbo = std::make_shared<OGLDefaultRenderFrame>(width, height, 0);
	P3DEngine::instance().setDefaultRenderFrame(defaultFbo);
	//P3DEngine::getSceneManager()->addRenderFrame("$screen", defaultFbo);
}

void GLWindow::setWinSize(uint32 w, uint32 h) {
	this->width = w;
	this->height = h;
	glfwSetWindowSize(win, w, h);
	glViewport(0, 0, w, h);
}
void GLWindow::beginRender() {
	glfwMakeContextCurrent(win);
	auto allRenderFrames = P3DEngine::instance().getAllRenderFrames();
	for (auto t : allRenderFrames) {
		t->apply();
		P3DEngine::instance().clearBuffers(0.0, 0.0, 0.0, 0.0);
		t->apply0();
	}
}
void GLWindow::endRender() {
	glfwPollEvents();
	glfwSwapBuffers(win);
}
void GLWindow::render() {
//	glfwMakeContextCurrent(win);
	beginRender();
//	GLUtils::clearBuffers(1.0, 1.0, 1.0, 0.0);
	scene->render();
	glfwPollEvents();
	glfwSwapBuffers(win);
}
void GLWindow::renderTexture(pTexture tex) {
	glfwMakeContextCurrent(win);
	GLUtils::render2Quad(tex);
	glfwPollEvents();
	glfwSwapBuffers(win);
}
void GLWindow::release() {
	if(win)
		glfwDestroyWindow(win);
	win = nullptr;
}

void GLWindow::setWinTitle(const std::string& title) {
	if (win == nullptr)
		return;
	glfwSetWindowTitle(win, title.c_str());
}
void GLWindow::getKeyboardStates(std::vector<KEY_STATES>& keyStates) {
	if (win == nullptr)
		return;
	for (uint32 i = KEY_NAME_NONE + 1; i < KEY_NAME_NUM; ++i) {
		auto keyState = glfwGetKey(win, glwKeyboardMaps[i]);
		if (keyState == GLFW_PRESS)
			keyStates[i] = KEY_STATE_PRESS;
		else if (keyState == GLFW_RELEASE)
			keyStates[i] = KEY_STATE_RELEASE;
	}

}
void GLWindow::getMouseStates(std::vector<MOUSE_BUTTON_STATES>& mouseStates) {
	if (win == nullptr)
		return;
	for (uint32 i = MOUSE_BUTTON_NONE + 1; i < MOUSE_BUTTON_NUM; ++i) {
		auto mouseState = glfwGetMouseButton(win, glwMouseButtonMaps[i]);
		if (mouseState == GLFW_PRESS)
			mouseStates[i] = MOUSE_STATE_PRESS;
		if (mouseState == GLFW_RELEASE)
			mouseStates[i] = MOUSE_STATE_RELEASE;
	}
}
Vec2f GLWindow::getMousePosition() {
	if (win == nullptr)
		return { 0.0f,0.0f };
	float64 posX, posY;
	glfwGetCursorPos(win, &posX, &posY);
	return { float32(posX),float32(posY) };
}
END_P3D_NS

//#endif
