#include "renderNodeBase.hpp"
BEGIN_P3D_NS

const RENDERNODE_TYPE RENDERNODE_CUSTOM = "custom";
class RenderNodeCustom:public RenderNodeBase {
public:
	RenderNodeCustom(const std::string& tName, const std::string& shaderName, const std::string& renderFrameName = "", const std::vector<float32>& winRatio = { 1.0f,1.0f }) : RenderNodeBase(tName) {
		renderNodeType = RENDERNODE_CUSTOM;
		defaultShaderName = shaderName;
		defaultShader = sceneManager->getShader(shaderName);
		initRenderFrame(renderFrameName, winRatio);
	}
	RenderNodeCustom(const std::string& tName, std::string vsPath, std::string fsPath, const  std::string& renderFrameName, const std::vector<float32>& winRatio = { 1.0f,1.0f }) : RenderNodeBase(tName) {
		renderNodeType = RENDERNODE_CUSTOM;
		//auto tDir = P3DEngine::instance().getShaderRootDir(renderNodeType);
		//getRealShaderPath(tDir, vsPath, fsPath);
		if (Utils::isAbsPath(vsPath) && Utils::isAbsPath(fsPath)) {
			//P3D_THROW_RUNTIME("RenderNodeCustom Shader path should be absolute path");
			defaultShaderName = loadShader(vsPath, fsPath);
			defaultShader = sceneManager->getShader(defaultShaderName);
		}
		initRenderFrame(renderFrameName, winRatio);
	}
};


END_P3D_NS