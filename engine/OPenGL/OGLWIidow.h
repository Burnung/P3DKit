//
// Created by bunring on 19-3-11.
//
//#if IS_WINDOWS || IS_MACOS
#pragma once

#include "common/P3D_Utils.h"
#include"common/RenderCommon.h"
#include "../Window.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>


BEGIN_P3D_NS

class GLWindow : public P3DWindow {
public:
	virtual ~GLWindow();
public:
	explicit GLWindow(uint32 width, uint32 height);
	//virtual void addComp(pComp comp, pShader shader) override;
	virtual void render();
	virtual void endRender();
	virtual void beginRender();
	virtual void renderTexture(const pTexture tex);
	virtual void setWinSize(uint32 w, uint32 h);
	//controller
	virtual void getKeyboardStates(std::vector<KEY_STATES>&);
	virtual void getMouseStates(std::vector<MOUSE_BUTTON_STATES>&);
	virtual Vec2f getMousePosition();
	virtual void setWinTitle(const std::string& title);
	virtual void* getWinHandler() { return win; }
	virtual void release();
private:
	//std::shared_ptr<GLFWwindow> win;
	GLFWwindow* win = nullptr;
};
typedef std::shared_ptr<GLWindow> pGLWindow;

END_P3D_NS
//#endif