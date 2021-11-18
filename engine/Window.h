#pragma once

#include "common/P3D_Utils.h"
#include "Scene.h"
#include "Shader.h"
#include "common/RenderCommon.h"
#include "extern/controllerInofs.h"
#include <vector>
//#include <GLFW/glfw3.h>

BEGIN_P3D_NS

class P3DWindow : public P3DObject {
public:
	P3DWindow(uint32 w, uint32 h) : width(w), height(h) {
		scene = std::make_shared<P3DScene>();
	};
public:
	virtual ~P3DWindow() {};
	static std::shared_ptr<P3DWindow> make(uint8 type,uint32 withd,uint32 height);

	void setScene(pScene tScene) {
		this->scene = tScene;
	}
	virtual void setWinSize(uint32 w, uint32 h){};
	void addComp(pComp comp, pShader shader);
	virtual void render() {};
	virtual void renderTexture(const pTexture tex) {};
	virtual Vec2f getMousePosition() { return { 0.0f,0.0f }; }
	pScene getScene() {
		return scene;
	}
	virtual void beginRender() {};
	virtual void endRender()  {};
	//controller
	virtual void getKeyboardStates(std::vector<KEY_STATES>& keyStates) { };
	virtual void getMouseStates(std::vector<MOUSE_BUTTON_STATES>&mouseStates) {};
	virtual void setWinTitle(const std::string& title) {}
	virtual void* getWinHandler() { return nullptr; }
	virtual void release() {};
	
protected:
	ApplyType appType = APP_NVALID;
	pScene scene = nullptr;
	uint32 width;
	uint32 height;
	

};
typedef std::shared_ptr<P3DWindow> pWindow;


END_P3D_NS