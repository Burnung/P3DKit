#include "renderNodeBase.hpp"
#include "../P3dEngine.h"
BEGIN_P3D_NS

//	pRenderNode createRenderNode(RENDERNODE_TYPE tType, std::string& tName, const std::string& vsPath, std::string& fsPath,std::string&renderFrameName,float32 winRatio=1.0) {
	//}
	//pRenderNode createRenderNode(RENDERNODE_TYPE tType, std::string& tName,const std::string& shaderName);
const RENDERNODE_TYPE RENDERNODE_SKY_SPHERE = "skySphere";
class RenderNodeImgSkysphere :public RenderNodeBase {
public:
	//RenderNodeImgFilter() {renderNodeType = "ImgFilter";};
	RenderNodeImgSkysphere(const std::string& tName, const std::string& shaderName, const std::string& renderFrameName,const std::vector<float32>& winRatio = { 1.0f,1.0f }) :RenderNodeBase(tName) {
		renderNodeType = RENDERNODE_SKY_SPHERE;
		defaultShaderName = shaderName;
		defaultShader = sceneManager->getShader(shaderName);
		initRenderFrame(renderFrameName, winRatio);
		//renderFrame = sceneManager->getRenderFrame(renderFrameName);
	}
	RenderNodeImgSkysphere(const std::string& tName, std::string vsPath, std::string fsPath, const  std::string& renderFrameName, const std::vector<float32>& winRatio = { 1.0f,1.0f }) :RenderNodeBase(tName) {
		renderNodeType = RENDERNODE_SKY_SPHERE;
		auto tDir = P3DEngine::instance().getShaderRootDir(renderNodeType);
		if (vsPath.empty())
			vsPath = Utils::joinPath(tDir, "sky_sphere.vert");
		if (fsPath.empty())
			fsPath = Utils::joinPath(tDir, "sky_sphere.frag");
		getRealShaderPath(tDir,vsPath, fsPath);
		defaultShaderName = loadShader(vsPath, fsPath);
		defaultShader = sceneManager->getShader(defaultShaderName);
		initRenderFrame(renderFrameName, winRatio);
	}

};


END_P3D_NS