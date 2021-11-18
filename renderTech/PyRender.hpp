#pragma once
#include "engine/mgs/renderNodeManager.hpp"
#include "engine/mgs/sceneManager.hpp"
#include "common/NdArray.h"
#include "engine/Window.h"
BEGIN_P3D_NS
class PyRender :public P3DObject{
public:
	PyRender() = default;
	~PyRender() = default;
public:
	void init(int w,int h) {
		Utils::init();
		P3DEngine::instance().init(APP_OPENGL);
		win = P3DEngine::instance().createWindow(w, h);
		sceneManager = P3DEngine::instance().getSceneManager();
		renderNodeManager = P3DEngine::instance().getRenderNodeManager();
	}
	void loadRenderCfg(const std::string& fPath) {
		renderNodeManager->paraseFromeFile(fPath);
	}
	void setRenderNodeFrameSize(const std::string& nodeName, int w, int h) {
		auto tNode = renderNodeManager->getRenderNode(nodeName);
		tNode->resizeRenderFrame(w, h);
	}
	void addTex(const std::string& texName, PNdArrayU8 tImg) {
		auto cpuImg = std::make_shared<CpuImage>(tImg);
		auto tex = P3DEngine::instance().createTexture(cpuImg);
		sceneManager->addTex(texName, tex);
	}
	void addTexByImg(const std::string& texName, pCPuImage tImg) {
		auto tex = P3DEngine::instance().createTexture(tImg);
		sceneManager->addTex(texName, tex);
	}
	void setCompModelMat(const std::string&tName, const std::vector<float>& matVec) {
		Mat4f tMat(matVec);
		sceneManager->setCompModelMat(tName, tMat);
	}
	void addCompData(const std::string compName, const std::vector<float>& vpsArray, const std::vector<uint32>& tlArray, const std::vector<float>& uvArray = {}, const std::vector<float> nArray = {}) {
		//std::cout << "begin add comp " << compName << std::endl;
		auto vps = std::make_shared<DataBlock>(vpsArray.data(), vpsArray.size() * sizeof(float), false, DType::Float32);
		auto tls = std::make_shared<DataBlock>(tlArray.data(), tlArray.size() * sizeof(uint32), false, DType::Uint32);
		//std::cout << "vps size is " << vpsArray.size() << "," << vpsArray[0] << " " << vpsArray[vpsArray.size() - 1] << std::endl;
		//std::cout << "vps size is " << tlArray.size() << "," << tlArray[0] << " " << tlArray[tlArray.size() - 1] << std::endl;
		pDataBlock uvs = nullptr;
		pDataBlock nvs = nullptr;
		if (!uvArray.empty())
			uvs = std::make_shared<DataBlock>(uvArray.data(), uvArray.size() * sizeof(float32), false, DType::Float32);
		if(!nArray.empty())
			nvs = std::make_shared<DataBlock>(nArray.data(), nArray.size() * sizeof(float32), false, DType::Float32);

		auto tComp = std::make_shared<P3DComp>(vps, tls, uvs, nvs);
		tComp->name = compName;
		tComp->recalculNormals();
		sceneManager->addComp(compName, tComp);
	}
	void updateComp(const std::string& tName, const std::vector<float>& vpsArray) {
		auto tComp = sceneManager->getComp(tName);
		tComp->cache_vps = std::make_shared<DataBlock>(vpsArray.data(), vpsArray.size() * sizeof(float), true, DType::Float32);
		tComp->recalculNormals();
	}
	void render() {
		win->beginRender();
		sceneManager->updateRenderables();
		renderNodeManager->renderAll();
		win->endRender();
	}
	PNdArrayF getRenderImg(const std::string& frameName, int slot) {
		auto tFbo = sceneManager->getRenderFrame(frameName);
		auto cpuImg = tFbo->getRenderTextureCpu(slot,true);
		return cpuImg->buffers[0].asFloat();
	}
	PNdArrayU8 getRenderImgU8(const std::string& frameName, int slot) {
		auto tFbo = sceneManager->getRenderFrame(frameName);
		auto cpuImg = tFbo->getRenderTextureCpu(slot,false);
		return cpuImg->buffers[0].asType<uint8>();
	}
	pTexture getRenderTex(const std::string& frameName, int slot) {
		auto tFbo = sceneManager->getRenderFrame(frameName);
		return tFbo->getRenderTexture(slot);
	}
	pWindow getWin() {
		return win;
	}
	pSceneManager getSceneManager() {
		return sceneManager;
	}

private:
	pWindow win;
	pSceneManager sceneManager;
	pRenderNodeManager renderNodeManager;
};
END_P3D_NS

