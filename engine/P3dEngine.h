#pragma once
#include "../common/P3D_Utils.h"

#include "Window.h"

#include "MeshGpu.h"
#include "LineGPU.h"
#include "Texture.h"
#include "renderFrame.h"
#include "p3dVertexBuffer.h"
#include <list>
//#include "mgs/sceneManager.hpp"
//#include "mgs/renderNodeManager.hpp"

BEGIN_P3D_NS
class SceneManager;
class RenderNodeManager;
class P3DEngine :public SingletonBase<P3DEngine>, P3DObject {
	P3D_SINGLETON_DECLEAR(P3DEngine)
public:
	bool init(ApplyType tType = APP_OPENGL);

	pWindow createWindow(uint32 w, uint h);
	pWindow getWindow();

	pMeshGPU createMeshGPU();

	pShader createShader();

	pVertexBuffer createVertexBuffer();

	pTexture createTexture(uint32 w,uint32 h,TextureType type,SampleType Sample_quality,BUFFER_USE_TYPE usave,PixelFormat pixeLformat,uint32 ssa = 1,uint32 mipNum = 1);
	pTexture createTexture(pCPuImage tImg, SampleType sample_quality=SAMPLE_LINER, BUFFER_USE_TYPE usage=READ_ONLY, uint32 ssa=1);
	pUserGeoGPU  createUserGpuGeo();

	pRenderFrame createRenderFrame(uint w, uint h, uint ssa);

	pRenderFrame getIntenalRenderFrame(uint w, uint h, uint ssa);

	pRenderStateSetting createRenderStateSetting(const std::string& tName, const std::vector<std::string>& params);
	void saveGlobalRenderState();
	void resetGlobalRenderState();
	void renderToQuad(pTexture tex);

	void clearBuffers(float r, float g,float b,float a = 1.0);
	void clearDepth();

	void finish();
//file systeam
	std::string getShaderRootDir(const std::string& tType="custom") { 
		if (shaderRootDirs.find(tType) == shaderRootDirs.end())
			return "";
		return shaderRootDirs[tType];
	}
	//manager systeam
	void setDefaultRenderFrame(pRenderFrame tFrame) {
		defaultFrame = tFrame;
		renderFrames.push_back(defaultFrame);
	}
	pRenderFrame getDefaultRenderFrame() {
		return defaultFrame;
	}
	const std::list<pRenderFrame>& getAllRenderFrames() {
		return renderFrames;
	}
	uint32 getWidth() {
		return w;
	}
	uint32 getHeight() {
		return h;
	}
	void setWinSize(int w, int h) {
		wins[0]->setWinSize(w, h);
		this->w = w, this->h = h;
	}
	void releaseAll() {
		renderFrames.clear();
		defaultFrame = nullptr;
		shaderRootDirs.clear();
		sceneManager = nullptr;
		renderNodeManager = nullptr;
		renderStateFactory = nullptr;
		globalRenderState = nullptr;
		//wins.clear();
	}
	std::shared_ptr<RenderNodeManager> getRenderNodeManager();
	std::shared_ptr<SceneManager> getSceneManager();
	
private:
	ApplyType appType = APP_NVALID;
	std::list<pRenderFrame> renderFrames;
	pRenderFrame defaultFrame;
	uint32 w, h;

	std::vector<pWindow> wins;
	std::unordered_map<std::string,std::string> shaderRootDirs;
	//scene render guanli 
	std::shared_ptr<SceneManager> sceneManager = nullptr;
	std::shared_ptr<RenderNodeManager> renderNodeManager = nullptr;
	pRenderStateFactory renderStateFactory = nullptr;
	pRenderState globalRenderState = nullptr;

};


END_P3D_NS