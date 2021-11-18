#pragma once
#include "renderNodeBase.hpp"
BEGIN_P3D_NS
const RENDERNODE_TYPE RENDERNODE_BLUR = "blur";
class RenderNodeBlur :public RenderNodeBase {
public:
	RenderNodeBlur(const std::string& tName, const std::string& shaderName, const std::string& renderFrameName, const std::vector<float32>& winRatio = { 1.0f,1.0f }) :RenderNodeBase(tName) {
		renderNodeType = RENDERNODE_BLUR;
		defaultShaderName = shaderName;
		defaultShader = sceneManager->getShader(shaderName);
		initRenderFrame(renderFrameName, winRatio);
		addRenderable("$quad");
		camName = "$normCam";
		camera = sceneManager->getCamera(camName);
		defaultShader->setCamera(camera);
		//renderFrame = sceneManager->getRenderFrame(renderFrameName);
	}
	RenderNodeBlur(const std::string& tName, std::string vsPath, std::string fsPath, const  std::string& renderFrameName, const std::vector<float32>& winRatio = { 1.0f,1.0f }) :RenderNodeBase(tName) {
		renderNodeType = RENDERNODE_BLUR;
		auto tDir = P3DEngine::instance().getShaderRootDir(renderNodeType);
		if (vsPath.empty())
			vsPath = Utils::joinPath(tDir, "quad.vert");
		getRealShaderPath(tDir, vsPath, fsPath);
		defaultShaderName = loadShader(vsPath, fsPath);
		defaultShader = sceneManager->getShader(defaultShaderName);
		initRenderFrame(renderFrameName, winRatio);
		addRenderable("$quad");
		camName = "$normCam";
		camera = sceneManager->getCamera(camName);
		defaultShader->setCamera(camera);
	}
	virtual void render() {

	}
};
END_P3D_NS
