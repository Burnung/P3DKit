#include "renderNodeBase.hpp"
BEGIN_P3D_NS

const RENDERNODE_TYPE RENDERNODE_3D = "3D";
class RenderNode3D :public RenderNodeBase {
public:
	RenderNode3D(const std::string& tName, const std::string& shaderName, const std::string& renderFrameName,const std::vector<float32>& winRatio = { 1.0f,1.0f }) : RenderNodeBase(tName) {
		renderNodeType = RENDERNODE_3D;
		defaultShaderName = shaderName;
		defaultShader = sceneManager->getShader(shaderName);
		initRenderFrame(renderFrameName, winRatio);
	}
	RenderNode3D(const std::string& tName, std::string vsPath, std::string fsPath, const  std::string& renderFrameName, const std::vector<float32>& winRatio = { 1.0f,1.0f }) : RenderNodeBase(tName) {
		renderNodeType = RENDERNODE_3D;
		auto tDir = P3DEngine::instance().getShaderRootDir(renderNodeType);
		
		getRealShaderPath(tDir, vsPath, fsPath);
		defaultShaderName = loadShader(vsPath, fsPath);
		defaultShader = sceneManager->getShader(defaultShaderName);
		initRenderFrame(renderFrameName, winRatio);
	}
};


END_P3D_NS