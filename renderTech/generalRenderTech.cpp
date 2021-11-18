#include "generalRenderTech.h"
#include "geo/meshDataIO.h"
#include "loader/P3DLoader.h"

BEGIN_P3D_NS

GeneralRenderTech::GeneralRenderTech() {
	P3DEngine::instance().init(APP_OPENGL);

	imgW = 1024, imgH = 1024;
	
	mWin = P3DEngine::instance().getWindow();
	
	mFbo = P3DEngine::instance().createRenderFrame(imgW, imgH,1);
	mFbo->addRenderTextures(1);
	
	mScene = std::make_shared<P3DScene>();
	mCam = mScene->getCamera();

	mCam->setViewMat(Mat4f::getIdentity());
	mCam->setProjParams(camFov, 1.0, 0.01, 500);
}

void GeneralRenderTech::setImgSize(int w, int h) {
	if (w == imgW && h == imgH)
		return;
	imgW = w, imgH = h;
	mFbo = P3DEngine::instance().createRenderFrame(imgW, imgH, 1);
	mFbo->addRenderTextures(1);

	mCam->setProjParams(camFov, 1.0*imgW / imgH, 0.01, 500);
	mWin->setWinSize(w, h);
}

void GeneralRenderTech::clearScene() {
	for (auto tNode : nodes) {
		mScene->removeNode(tNode);
	}
	nodes.clear();
}
void GeneralRenderTech::addCompData(const std::string& nodeName, PNdArrayF vpsData, PNdArrayF tls, PNdArrayF uvsData, const std::string& shaderName,PNdArrayF uvTl) {
	std::cout << "inner set comp data" << std::endl;

	if (shaders.find(shaderName) == shaders.end()) {
		std::cout << "shader not found" << std::endl;
		return;
	}
	if (nodes.find(nodeName) != nodes.end()) {
		mScene->removeNode(nodeName);
	}
	auto vps = vpsData.getDataBlock()->clone();

	auto uvs = uvsData.getDataBlock()->clone();

	auto tl = std::make_shared<DataBlock>(tls.getDataBlock()->getSize(), DType::Uint32);
	std::cout << "comp_tl size is " << tls.getDataBlock()->getSize() << " " << tls.shape()[0] << std::endl;

	auto tlData = tl->rawData<uint32>();
	for (int i = 0; i < tls.shape()[0]; ++i) {
		for (int j = 0; j < 3; ++j) {
			tlData[i * 3 + j] = uint32(tls[{i, j}]);
		}
	}

	std::cout << "init data ok" << std::endl;
	auto comp = std::make_shared<P3DComp>(vps, tl, uvs, nullptr);
	if (true) {
		auto uvtl = std::make_shared<DataBlock>(tls.getDataBlock()->getSize(), DType::Uint32);
		auto uvTlData = uvtl->rawData<uint32>();
		for (int i = 0; i < uvTl.shape()[0]; ++i) {
			for (int j = 0; j < 3; ++j)
				uvTlData[i * 3 + j] = uint32(uvTl[{i, j}]);
		}
		comp->uvtl = uvtl;
		comp->splitVertex();
	}
	comp->name = nodeName;
	comp->recalculNormals();
	auto tShader = shaders[shaderName];
	//std::cout << "shader is " >> << std::endl;
	mScene->addComp(comp, tShader);
	nodes.insert(nodeName);

}
/*
void  GeneralRenderTech::addComp(const std::string& nodeName, PNdArrayF vps, PNdArrayF tl,const std::string& shaderName) {
	if (shaders.find(shaderName) == shaders.end()) {
		std::cout << "shader not found" << std::endl;
		return;
	}
	if (nodes.find(nodeName) != nodes.end()) {
		mScene->removeNode(nodeName);
	}
	auto vpsBlock = std::make_shared<DataBlock>(vps.getDataBlock()->rawData(), vps.getDataBlock()->getSize(), true, DType::Float32);
	uint32 tlSize = tl.shape()[0];
	auto tlBlock = std::make_shared<DataBlock>(tlSize * sizeof(uint32), DType::Uint32);
	auto tlData = tlBlock->rawData<uint32>();
	for (int i = 0; i < tlSize; ++i) {
		tlData[i] = tl[{i}];
	}

	auto tComp = std::make_shared<P3DComp>(vpsBlock, tlBlock, nullptr, nullptr);
	tComp->name = nodeName;
	auto shader = shaders[shaderName];
	mScene->addComp(tComp, shader);
	nodes.insert(nodeName);
}
*/
void GeneralRenderTech::addComp(const std::string& nodeName, const std::string& objPath, const std::string& shaderName) {

	if (shaders.find(shaderName) == shaders.end()) {
		std::cout << "shader not found" << std::endl;
		return;
	}
	if (nodes.find(nodeName) != nodes.end()) {
		mScene->removeNode(nodeName);
	}
	auto comp = MeshDataIO::instance().loadObj(objPath);
	comp->recalculNormals();
	comp->name = nodeName;
	auto tShader = shaders[shaderName];
	mScene->addComp(comp, tShader);
	nodes.insert(nodeName);

}

void GeneralRenderTech::addHair(const std::string& nodeName, const std::string& hairPath, const std::string& shaderName) {

	if (shaders.find(shaderName) == shaders.end()) {
		std::cout << "shader not found" << std::endl;
		return;
	}
	if (nodes.find(nodeName) != nodes.end()) {
		mScene->removeNode(nodeName);
	}
	auto hairModel = std::make_shared<HairModel>();
	hairModel->loadFromFile(hairPath);
	hairModel->name = nodeName;
	auto tShader = shaders[shaderName];
	
	mScene->addHair(hairModel, tShader);

	nodes.insert(nodeName);

}
void GeneralRenderTech::addTexture(const std::string& texName, PNdArrayU8 tImg) {
	auto cpuImg = std::make_shared<CpuImage>(tImg);
	auto tex = P3DEngine::instance().createTexture(cpuImg);
	texs[texName] = tex;
}
void GeneralRenderTech::setShaderTex(const std::string& shaderName, const std::string& texName,const std::string& uniformName) {
	if (shaders.find(shaderName) == shaders.end() || texs.find(texName) == texs.end()) {
		std::cout << "shader name or tex name error"<<std::endl;
	}
	if (uniformName == "")
		shaders[shaderName]->setUniformTex(texName, texs[texName]);
	else
		shaders[shaderName]->setUniformTex(uniformName, texs[texName]);

}
void GeneralRenderTech::setShaderFloat(const std::string& shaderName, const std::string& uniformName, float v) {
	if (shaders.find(shaderName) == shaders.end()) {
		std::cout << "shader name or  error"<<std::endl;
		return;
	}
	shaders[shaderName]->setUniformF(uniformName, v);

}
void GeneralRenderTech::addShader(const std::string& shaderName, const std::string& vsPath, const std::string& psPath) {
	auto tShader = P3DEngine::instance().createShader();
	tShader->loadFromFile(vsPath, psPath);
	shaders[shaderName] = tShader;
}

void GeneralRenderTech::setNodeTranslate(const std::string& nodeName, const std::vector<float>& srMat, const std::vector<float>& t) {
	auto tMat = Mat4f::getIdentity();
	doFor(r, 3) {
		doFor(c, 3)
			tMat[{r, c}] = srMat[r * 3 + c];
		tMat[{r, 3}] = t[r];
	}

	auto compNode = mScene->findComp(nodeName);
	if (compNode != nullptr) {
		compNode->setModelMat(tMat);
		return;
	}

	auto hairNode = mScene->findHair(nodeName);
	if (hairNode != nullptr) {
		hairNode->setModelMat(tMat);
	}
}

void GeneralRenderTech::setCamFov(float fov) {
	camFov = fov;
	mCam->setProjParams(fov, 1.0*imgW / imgH, 0.01, 500);
}
PNdArrayF GeneralRenderTech::render() {
	mFbo->apply();	
	mScene->render();
	mFbo->apply0();

	auto tImg = mFbo->getRenderTextureCpu(0,true);
	return tImg->buffers[0].asFloat();
}
void GeneralRenderTech::renderWin() {
	mFbo->apply();
	mScene->render();
	mFbo->apply0();
	mWin->renderTexture(mFbo->getRenderTexture(0));
}
END_P3D_NS