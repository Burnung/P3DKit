#pragma once
#include "common/P3D_Utils.h"
#include "engine/P3dEngine.h"
//#include "scene_pares.h"
//#include "engine/mgs/renderNodeManager.hpp"

BEGIN_P3D_NS

class BaseApp :public P3DObject {
public:
	BaseApp(uint32 w, uint32 h, const std::string&tittle = "Demo");
	virtual ~BaseApp() = default;
public:
	virtual void onInit() = 0;
	virtual void onUpdateRender(float deltTime) = 0;
	virtual void onUpdateUI() = 0;
	virtual void onShuwdown() = 0;
	virtual void init();

private:
	bool updateBaseUI(float deltaTime);
	//void update();
public:

	void shutDown();
	pScene getScene() {
		return win->getScene();
	}
	void setScene(pScene s) {
		win->setScene(s);
	}
	void run();
	void runFrames();
	bool runAFrame();

protected:

	pWindow win;
	int winW = 0, winH = 0;
	std::string winTittle;
	float64 lastTime;
	bool dExit = false;

};



END_P3D_NS

