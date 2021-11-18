#pragma once
#include "geo/comp.h"
#include "geo/P3DSkinMesh.h"
#include "sceneManager.hpp"
#include "../P3dEngine.h"
#include <unordered_set>
#include <string>
#include "../P3DLight.hpp"
BEGIN_P3D_NS
typedef std::string RENDERNODE_TYPE;
class RenderNodeBase :public P3DObject {
public:
	//RenderNodeBase(){}
	RenderNodeBase(const std::string& tName) : name(tName) {
		sceneManager = P3DEngine::instance().getSceneManager();
	};
	virtual ~RenderNodeBase() = default;
protected:
	RENDERNODE_TYPE renderNodeType = "Base";
	std::string name;
	std::string defaultShaderName;
	pShader defaultShader;
	std::unordered_set<std::string> renderableNames;
	std::unordered_map<std::string, std::unordered_map<std::string,pShaderUniform>>  renderableShaderUniforms;
	std::unordered_map<std::string, std::string>  renderableShaders;
	std::string renderFrameName;
	pRenderFrame renderFrame;
	std::unordered_map<std::string,pShaderUniform> baseUniforms;
	std::unordered_map<std::string, pRenderStateSetting> renderStateSettings;
	std::string camName;
	pCamera camera;
	pSceneManager sceneManager;
	std::vector<float> winRatio = { 1.0f,1.0f };
//inner methed
public:
	std::string getRenderFrameName()const {
		return renderFrameName;
	}
	void reinitRenderFrame() {
		uint32 w = P3DEngine::instance().getWidth() * winRatio[0];
		uint32 h = P3DEngine::instance().getHeight() * winRatio[1];
		auto tRenderFrame = sceneManager->getRenderFrame(renderFrameName);
		tRenderFrame->resize(w, h);
		sceneManager->addRenderFrame(renderFrameName, tRenderFrame);
		if (camera) {
			float asp = w * 1.0f / h;
			camera->setProjParams(camera->fov, asp, camera->nearPlane, camera->farPlane);
		}
	}
protected:
	void getRealShaderPath(const std::string& tDir, std::string& vsPath,std::string& fsPath) {
		if (Utils::getFileDir(vsPath).empty())
			vsPath = Utils::joinPath(tDir, vsPath);
		if (Utils::getFileDir(fsPath).empty())
			fsPath = Utils::joinPath(tDir, fsPath);
	}
	void initRenderFrame(const std::string& tName, const std::vector<float32>& winRatio) {
		uint32 w = P3DEngine::instance().getWidth() * winRatio[0];
		uint32 h = P3DEngine::instance().getHeight() * winRatio[1];
		this->winRatio = { winRatio[0],winRatio[1] };

		auto tFrame = sceneManager->getRenderFrame(tName);
		if (!tFrame) {
			tFrame = P3DEngine::instance().createRenderFrame(w, h, 1);
		}
		else if (tFrame->getWidth() != w || tFrame->getHeight() != h) {
			P3D_THROW_RUNTIME("RenderNodeBase::initRenderFrame: renderFrame:%s has diff size!", tName.c_str());
		}
		sceneManager->addRenderFrame(tName, tFrame);
		renderFrameName = tName;
		renderFrame = tFrame;
	}
public:
	std::string loadShader(const std::string&vsPath, const std::string& fsPath) {
		auto shaderName = renderNodeType + "_"+Utils::getBaseName(fsPath);
		if (sceneManager->getShader(shaderName))
			return shaderName;
		auto tShader = P3DEngine::instance().createShader();
		tShader->loadFromFile(vsPath, fsPath);
		sceneManager->addShader(shaderName, tShader);
		return shaderName;
	}

public:
	void resizeRenderFrame(int w, int h) {
		if (renderFrame == nullptr) {
			std::cout <<"renderNode "<<name<< " error: no render Frame" << std::endl;
			return;
		}
		if (renderFrameName == "$screen") {
			//auto tWin = P3DEngine::instance().getWindow();
			if (P3DEngine::instance().getWidth() == w && P3DEngine::instance().getHeight() == h)
				return;
			P3DEngine::instance().setWinSize(w, h);
		}
		else {
			if (renderFrame->getWidth() == w && renderFrame->getHeight() == h)
				return;
			renderFrame = P3DEngine::instance().createRenderFrame(w, h, 1);
			sceneManager->addRenderFrame(renderFrameName, renderFrame);
		}
		if (camera) {
			float asp = w * 1.0f / h;
			camera->setProjParams(camera->fov, asp, camera->nearPlane, camera->farPlane);
		}
	}
	void addRenderStateSetting(const std::string& settingName, const std::vector<std::string>&params) {
		renderStateSettings[settingName] = P3DEngine::instance().createRenderStateSetting(settingName, params);
	}
	void addUniform(pShaderUniform tUniform) {
		baseUniforms[tUniform->getLocName()] = tUniform;
	}
	void addRenderableUniform(const std::string& tName, pShaderUniform tv) {
		renderableShaderUniforms[tName][tv->getLocName()] = tv;
	}
	void applyShaderUniforms(pShader tShader, const std::string& renderableName) {
		for (auto& tk : baseUniforms) {
			tShader->addUniform(tk.second);
		}
		//std::cout << "uniroms " << renderableShaderUniforms[renderableName].size() << std::endl;
		for (auto& tk : renderableShaderUniforms[renderableName]) {
			tShader->addUniform(tk.second);
		//	std::cout << "applay " << renderableName << " uniforms" << std::endl;
		}
		int i = 0;
		auto allLits = sceneManager->getAllLights();
		for (const auto& tLit : allLits) {
			auto lit = tLit.second;
			auto tunis = lit->apply(i);
			for (auto tt : tunis)
				tShader->addUniform(tt);
			++i;
		}
	}
	void setCamera(const std::string& tName) {
		camName = tName;
		camera = sceneManager->getCamera(tName);
	}
	void addRenderable(const std::string& tName) {
		renderableNames.insert(tName);
	}
	void addRenderableList(const std::vector<std::string>& tNames) {
		for(auto tName:tNames) 
			renderableNames.insert(tName);
	}
	void setRenderableShader(const std::string & rName, const std::string& sName) {
		renderableShaders[rName] = sName;
	}
	virtual void render() {
		renderFrame = sceneManager->getRenderFrame(renderFrameName);
		renderFrame->apply();
		P3DEngine::instance().saveGlobalRenderState();
		for (const auto& tState : renderStateSettings)
			tState.second->apply();
		camera = sceneManager->getCamera(camName);
		if (defaultShader) {
			defaultShader->setCamera(camera);
		}
		for (const auto& tName : renderableNames) {
			auto renderable = sceneManager->getRenderable(tName);
			if (renderable == nullptr) {
				std::cout << "don't have comp " << tName << std::endl;
				continue;
			}
			auto tShader = defaultShader;
			if (renderableShaders.find(tName) != renderableShaders.end()) {
				tShader = sceneManager->getShader(renderableShaders[tName]);
				tShader->setCamera(camera);
			}
			tShader->clearUniforms();
			applyShaderUniforms(tShader,tName);
			//std::cout << "name " << tName << ",ptr " << renderable.get() << std::endl;
			renderable->setShader(tShader);
			renderable->draw();
		}
		renderFrame->apply0();
		P3DEngine::instance().resetGlobalRenderState();
	}
	pShader getDefaultShader() const { return defaultShader; }
	std::string getDefaultShaderName() const { return defaultShaderName; }

};
typedef std::shared_ptr<RenderNodeBase> pRenderNode;



END_P3D_NS
