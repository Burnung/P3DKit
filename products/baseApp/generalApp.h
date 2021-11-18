#pragma once
#include "baseApp.h"
#include "engine/mgs/renderNodeManager.hpp"
#include "engine/mgs/sceneManager.hpp"
#include "scene_pares.h"
BEGIN_P3D_NS

class GeneralApp :public BaseApp {
public:
	GeneralApp(int w, int h, const std::string& tName = "demo") :BaseApp(w, h, tName) {}
public:
	virtual void onInit();
	virtual void onUpdateRender(float deltTime);
	virtual void onUpdateUI();
	virtual void onShuwdown() {};
public:
	void initFromCfgs(cstring sceneCfg="", cstring rednerNodeCfg="", cstring uiCfg="");
public:
	pRenderNodeManager renderNodeManager;
	pSceneManager sceneManager;
	pUIManager uiManager;

};



END_P3D_NS
