#pragma once
#include "products/baseApp/generalApp.h"
#include "engine/mgs/renderNodeManager.hpp"
#include "engine/mgs/sceneManager.hpp"
#include "common/NdArray.h"
#include "engine/Window.h"
#include "geo/meshDataIO.h"
BEGIN_P3D_NS
class GeneralAppPython :public P3DObject {
public:
	GeneralAppPython() = default;
	void init(int w, int h, cstring winName = "pyApp") {
		generalApp = std::make_shared<GeneralApp>(w, h, winName);
		generalApp->init();

		sceneManager = generalApp->sceneManager;
		renderNodeManager = generalApp->renderNodeManager;
		uiManager = generalApp->uiManager;

	}
	void setConfigs(cstring sceneCfg = "", cstring rednerNodeCfg = "", cstring uiCfg = "") {
		generalApp->initFromCfgs(sceneCfg, rednerNodeCfg, uiCfg);
	}
	void runWindow() {
		generalApp->runFrames();
	}
	void runAFrame() {
		generalApp->runAFrame();
	}
	void setRenderNodeFrameSize(const std::string& nodeName, int w, int h) {
		auto tNode =renderNodeManager->getRenderNode(nodeName);
		tNode->resizeRenderFrame(w, h);
	}
	void addTex(const std::string& texName, PNdArrayU8 tImg) {
		auto cpuImg = std::make_shared<CpuImage>(tImg);
		auto tex = P3DEngine::instance().createTexture(cpuImg);
		sceneManager->addTex(texName, tex);
	}
	void resizeWinSize(int w,int h) {
		if (P3DEngine::instance().getWidth() == w && P3DEngine::instance().getHeight() == h)
			return;
		P3DEngine::instance().setWinSize(w, h);
		std::unordered_set<std::string> renderFrameNames;
		auto allRenderNodes = renderNodeManager->getAllRenderNodes();
		for (auto item : allRenderNodes) {
			item.second->reinitRenderFrame();
		}
	}
	void addTexByImg(const std::string& texName, pCPuImage tImg) {
		auto tex = P3DEngine::instance().createTexture(tImg);
		sceneManager->addTex(texName, tex);
	}
	void setCompModelMat(const std::string& tName, const std::vector<float>& matVec) {
		Mat4f tMat(matVec);
		sceneManager->setCompModelMat(tName, tMat);
	}
	void addCompData(const std::string& compName, const std::vector<float>& vpsArray, const std::vector<uint32>& tlArray, const std::vector<float>& uvArray = {}, const std::vector<float> nArray = {}) {
		//std::cout << "begin add comp " << compName << std::endl;
		auto vps = std::make_shared<DataBlock>(vpsArray.data(), vpsArray.size() * sizeof(float), false, DType::Float32);
		auto tls = std::make_shared<DataBlock>(tlArray.data(), tlArray.size() * sizeof(uint32), false, DType::Uint32);
		//std::cout << "vps size is " << vpsArray.size() << "," << vpsArray[0] << " " << vpsArray[vpsArray.size() - 1] << std::endl;
		//std::cout << "vps size is " << tlArray.size() << "," << tlArray[0] << " " << tlArray[tlArray.size() - 1] << std::endl;
		pDataBlock uvs = nullptr;
		pDataBlock nvs = nullptr;
		if (!uvArray.empty())
			uvs = std::make_shared<DataBlock>(uvArray.data(), uvArray.size() * sizeof(float32), false, DType::Float32);
		if (!nArray.empty())
			nvs = std::make_shared<DataBlock>(nArray.data(), nArray.size() * sizeof(float32), false, DType::Float32);

		auto tComp = std::make_shared<P3DComp>(vps, tls, uvs, nvs);
		tComp->name = compName;
		tComp->recalculNormals();
		sceneManager->addComp(compName, tComp);
	}
	void addCompFile(const std::string& compName, const std::string& fpath) {
		auto tComp = MeshDataIO::instance().loadFromFile(fpath);
	
		tComp->recalculNormals();
		tComp->name = compName;
		sceneManager->addComp(compName, tComp);
	}
	void updateComp(const std::string& tName, const std::vector<float>& vpsArray) {
		auto tComp = sceneManager->getComp(tName);
		tComp->cache_vps = std::make_shared<DataBlock>(vpsArray.data(), vpsArray.size() * sizeof(float), true, DType::Float32);
		tComp->recalculNormals();
	}
	PNdArrayF getRenderImg(const std::string& frameName, int slot) {
		auto tFbo = sceneManager->getRenderFrame(frameName);
		auto cpuImg = tFbo->getRenderTextureCpu(slot, true);
		return cpuImg->buffers[0].asFloat();
	}
	PNdArrayU8 getRenderImgU8(const std::string& frameName, int slot) {
		auto tFbo = sceneManager->getRenderFrame(frameName);
		auto cpuImg = tFbo->getRenderTextureCpu(slot, false);
		return cpuImg->buffers[0].asType<uint8>();
	}
	pTexture getRenderTex(const std::string& frameName, int slot) {
		auto tFbo = sceneManager->getRenderFrame(frameName);
		return tFbo->getRenderTexture(slot);
	}
	pWindow getWin() {
		P3DEngine::instance().getWindow();
	}
	std::vector<std::vector<float>> getValuesF(const std::vector<std::string>& valueNames) {
		std::vector<std::vector<float>> ret;
		for (const auto& tName : valueNames)
			ret.push_back(getValueF(tName));
		return ret;
	}
	std::vector<float> getValueF(cstring tName) {
		Value tV;
		if (!sceneManager->getValue(tName,tV))
			return {};
		return tV.toFloats();
	}
	void setValues(const std::vector<std::string>& vNames, const std::vector<std::vector<float>>& vs) {
		for (int i = 0; i < vNames.size(); ++i)
			setValue(vNames[i], vs[i]);
	}
	void setValue(cstring vName, const std::vector<float>& v) {
		Value tV;
		if (!sceneManager->getValue(vName, tV)) {
			tV = Value(v);
			sceneManager->addValue(vName, tV);
		}
		else {
			tV.setValue(v.data(), v.size() * sizeof(float));
		}
	}
	void setCameraPos(const std::string& camName, const std::vector<float>& pos) {
		auto tCamera = sceneManager->getCamera(camName);
		tCamera->setViewParams({ pos[0],pos[1],pos[2] }, { 0.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f });
	}

	void setCameraFov(const std::string& camName,float32 fov) {
		auto tCamera = sceneManager->getCamera(camName);
		tCamera->setProjParams(fov, tCamera->asp, tCamera->nearPlane, tCamera->farPlane);
	}
	void release() {
		sceneManager = nullptr;
		renderNodeManager = nullptr;
		uiManager = nullptr;
		//generalApp->shutDown();
		generalApp = nullptr;
		P3DEngine::instance().releaseAll();
		
	}

public:
	std::shared_ptr<GeneralApp> generalApp;
	pSceneManager sceneManager;
	pRenderNodeManager renderNodeManager;
	pUIManager uiManager;
};


END_P3D_NS
