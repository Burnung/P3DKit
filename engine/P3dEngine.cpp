#include "P3dEngine.h"
#include "Window.h"
#include "MeshGpu.h"
#include "OPenGL/OGLTexture.h"
#include "OPenGL/OGLCommon.h"
#include "OPenGL/OGLUserMeshGpu.h"
#include "mgs/renderNodeManager.hpp"
#include "mgs/sceneManager.hpp"
#include <filesystem>
BEGIN_P3D_NS

bool P3DEngine::init(ApplyType t) {
	P3D_ENSURE(t != APP_NVALID, "type invalid");
	appType = t;
	std::filesystem::path parDir(__FILE__);
	parDir = parDir.parent_path().append("shaders");
//	std::cout << "shader root dir is " << parDir.string() << std::endl;

	auto tmpDir = parDir;
	//shaderRootDirs["depthMap"] = parDir.append("depthMap").string();
	shaderRootDirs["imgFilter"] = parDir.append("imgFilter").string();
	parDir = tmpDir;
	shaderRootDirs["blur"] = parDir.append("blur").string();
	parDir = tmpDir;
	shaderRootDirs["3D"] = parDir.append("3D").string();
	parDir = tmpDir;
	shaderRootDirs["skySphere"] = parDir.append("skySphere").string();

	shaderRootDirs["custom"] = "";

}

pWindow P3DEngine::createWindow(uint32 w,uint h) {
	P3D_ENSURE(appType != APP_NVALID, "Please Init P3D Engine");
	auto tWin = P3DWindow::make(appType, w, h);
	wins.push_back(tWin);
	this->w = w, this->h = h;
	if (renderStateFactory == nullptr &&appType == APP_OPENGL) {
		renderStateFactory = std::make_shared<OGLRenderStateFactory>();
	}
	if (globalRenderState == nullptr &&appType == APP_OPENGL) {
		globalRenderState = std::make_shared<OGLGlobalRenderState>();
	}
	return tWin;
}
pWindow P3DEngine::getWindow() {
	if (wins.size() > 0)
		return wins[0];
	return createWindow(1024, 1024);
}
pMeshGPU P3DEngine::createMeshGPU() {
	P3D_ENSURE(appType != APP_NVALID, "Please Init P3D Engine");
	return MeshGPU::make(appType);
}

pShader P3DEngine::createShader() {
	P3D_ENSURE(appType != APP_NVALID, "Please Init P3D Engine");
	return Shader::make(appType);
}
pVertexBuffer P3DEngine::createVertexBuffer() {

	P3D_ENSURE(appType != APP_NVALID, "Please Init P3D Engine");
	return VertexBuffer::make(appType);
}
pTexture P3DEngine::createTexture(uint32 w, uint32 h, TextureType type, SampleType Sample_quality, BUFFER_USE_TYPE usage, PixelFormat pixeLformat,uint32 ssa,uint32 mipNum) {
	P3D_ENSURE(appType != APP_NVALID, "Please Init P3D Engine");
	return Texture::make(appType, w, h, type, Sample_quality, usage, pixeLformat, ssa,mipNum);
}
pTexture P3DEngine::createTexture(pCPuImage tImg,SampleType sample_quality,BUFFER_USE_TYPE usage,uint32 ssa) {
	P3D_ENSURE(appType != APP_NVALID, "Please Init P3D Engine");
	return Texture::makeFromCpu(appType, tImg, sample_quality, usage, ssa);
}
pRenderFrame P3DEngine::createRenderFrame(uint w, uint h, uint ssa) {
	auto ret = RenderFrame::make(appType, w, h, ssa);
	renderFrames.push_back(ret);
	return ret;

}
void P3DEngine::clearDepth() {
	if (appType == APP_OPENGL)
		GLUtils::clearDepth();
}
void P3DEngine::clearBuffers(float r,float g,float b,float a) {
	switch (appType) {
	case APP_OPENGL:
	{
		GLUtils::clearBuffers(r, g, b ,a);
		break;
	}
	}
}
pRenderFrame P3DEngine::getIntenalRenderFrame(uint w, uint h, uint ssa) {
	pRenderFrame ret = nullptr;
	std::vector<pRenderFrame> renderFrames;
	for (auto t : renderFrames) {
		if (t->getWidth() == w && t->getHeight() == h && t->getSsa() == ssa) {
			ret = t;
			break;
		}
	}
	if (!ret) {
		ret = createRenderFrame(w, h, ssa);
		renderFrames.push_back(ret);
	}
	return ret;

}

pUserGeoGPU P3DEngine::createUserGpuGeo() {
	if (appType == APP_OPENGL)
		return std::make_shared<OGLUserMeshGPU>();
}

void P3DEngine::finish() {
	if (appType == APP_OPENGL) {
		GLUtils::finish();
	}
}
void P3DEngine::renderToQuad(pTexture tex) {
}

std::shared_ptr<RenderNodeManager> P3DEngine::getRenderNodeManager() {
	if (renderNodeManager == nullptr)
		renderNodeManager = std::make_shared<RenderNodeManager>();
	return renderNodeManager;
}
std::shared_ptr<SceneManager> P3DEngine::getSceneManager() {
	if (sceneManager == nullptr)
		sceneManager = std::make_shared<SceneManager>();
	return sceneManager;
}

pRenderStateSetting P3DEngine::createRenderStateSetting(const std::string& tName, const std::vector<std::string>& params) {
	return renderStateFactory->createRenderState(tName, params);
}
void P3DEngine::saveGlobalRenderState() {
	globalRenderState->saveRenderState();
}
void P3DEngine::resetGlobalRenderState() {
	globalRenderState->recoverRenderState();
}
END_P3D_NS