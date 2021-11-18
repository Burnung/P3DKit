#include "renderNodeBase.hpp"
#include <filesystem>
BEGIN_P3D_NS

//	pRenderNode createRenderNode(RENDERNODE_TYPE tType, std::string& tName, const std::string& vsPath, std::string& fsPath,std::string&renderFrameName,float32 winRatio=1.0) {
	//}
	//pRenderNode createRenderNode(RENDERNODE_TYPE tType, std::string& tName,const std::string& shaderName);
const RENDERNODE_TYPE RENDERNODE_IMG_FILTER = "imgFilter";
class RenderNodeImgFilter :public RenderNodeBase {
public:
	//RenderNodeImgFilter() {renderNodeType = "ImgFilter";};
	RenderNodeImgFilter(const std::string& tName, const std::string& shaderName, const std::string& renderFrameName, const std::vector<float32>& winRatio = {1.0,1.0}) :RenderNodeBase(tName) {
		renderNodeType = RENDERNODE_IMG_FILTER;
		defaultShaderName = shaderName;
		defaultShader = sceneManager->getShader(shaderName);
		initRenderFrame(renderFrameName, winRatio);
		addRenderable("$quad");
		camName = "$normCam";
		camera = sceneManager->getCamera(camName);
		defaultShader->setCamera(camera);
		initRenderStateSetting();
		//renderFrame = sceneManager->getRenderFrame(renderFrameName);
	}
	RenderNodeImgFilter(const std::string& tName, std::string vsPath, std::string fsPath, const  std::string& renderFrameName, const std::vector<float32>& winRatio = { 1.0f,1.0f }) :RenderNodeBase(tName) {
		renderNodeType = RENDERNODE_IMG_FILTER;
		auto tDir = P3DEngine::instance().getShaderRootDir(renderNodeType);
		if (vsPath.empty())
			vsPath = Utils::joinPath(tDir, "img_filter.vert");
		getRealShaderPath(tDir,vsPath, fsPath);
		defaultShaderName = loadShader(vsPath, fsPath);
		defaultShader = sceneManager->getShader(defaultShaderName);
		initRenderFrame(renderFrameName, winRatio);
		addRenderable("$quad");
		camName = "$normCam";
		camera = sceneManager->getCamera(camName);
		defaultShader->setCamera(camera);
		initRenderStateSetting();
	}
private:
	void initRenderStateSetting() {
		renderStateSettings["blend"] = P3DEngine::instance().createRenderStateSetting("blend", { "enable" });
		renderStateSettings["depth"] = P3DEngine::instance().createRenderStateSetting("depth", { "disable" });
	}

};


END_P3D_NS