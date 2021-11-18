#include "generalApp.h"
#include "engine/P3dEngine.h"
#include "libs/imgui/myImgui.h"
BEGIN_P3D_NS

void GeneralApp::onInit() {
	sceneManager = P3DEngine::instance().getSceneManager();
	renderNodeManager = P3DEngine::instance().getRenderNodeManager();
	uiManager = std::make_shared<UIManager>();
	sceneManager->addValue("appExit", Value(0.0f));
}
void GeneralApp::onUpdateRender(float deltTime) {
	sceneManager->updateSkins(deltTime);
	sceneManager->updateRenderables();
	renderNodeManager->renderAll();
}
void GeneralApp::initFromCfgs(cstring sceneCfg, cstring renderNodeCfg, cstring uiCfg ) {
	if (!sceneCfg.empty()) {
		sceneManager->paraseFromFile(sceneCfg);
	}
	if (!renderNodeCfg.empty()) {
		renderNodeManager->paraseFromeFile(renderNodeCfg);
	}
	if (!uiCfg.empty()) {
		uiManager->paraseFromFile(uiCfg);
	}
}
void GeneralApp::onUpdateUI() {
	ImGui::Begin("general opt");
	if (ImGui::Button("reload Shader")) {
		auto allShaders = sceneManager->getAllShaders();
		for (auto t : allShaders)
			t.second->reload();
	}
	ImGui::End();
	uiManager->apply();
	if (dExit) {
		sceneManager->addValue("appExit", Value(1.0f));
	}
}
/*
	virtual void onInit() = 0;
	virtual void onUpdateRender(float deltTime) = 0;
	virtual void onUpdateUI() = 0;
	virtual void onShuwdown() = 0;

	*/
END_P3D_NS