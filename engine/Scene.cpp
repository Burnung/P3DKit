#include "Scene.h"
#include "Shader.h"
#include "geo/comp.h"
#include "P3DCore.h"
#include "Camera.h"
BEGIN_P3D_NS
P3DScene::~P3DScene() {
	release();
}
void P3DScene::release() {

	defaultShader = nullptr;
	comps.clear();
	shaders.clear();
	hairModels.clear();
	gpuLines.clear();
	gpuMeshes.clear();
	userGeos.clear();
	mCamera = nullptr;
}
P3DScene::P3DScene() {
	mCamera = std::make_shared<Camera>();
	mCamera->setViewParams({ 0.0,0.0,10.0 }, { 0.0,0.0,0.0 }, { 0.0f,1.0f,0.0f });
	mCamera->setProjParams(60.0, 1.0, 0.01, 100.0f);
}
void P3DScene::addComp(pComp tComp, pShader tShader) {

	//auto gpuMesh = P3DEngine::instance().createMeshGPU();
	auto gpuMesh = P3DEngine::instance().createUserGpuGeo();
	gpuMesh->fromComp(tComp);
	if (tShader) {
		tShader->setCamera(mCamera);
		gpuMesh->setShader(tShader);
	}
	else
		gpuMesh->setShader(defaultShader);

	//gpuMesh->init(tComp->cache_vps, tComp->tl, tComp->uvs, tComp->cache_vns);

	comps.push_back(tComp);

	if (tShader == nullptr)
		shaders.push_back(defaultShader);
	else
		shaders.push_back(tShader);

	gpuMeshes.push_back(gpuMesh);
	std::cout << "gpu mesh count is " << gpuMeshes.size() << std::endl;
	

}
void P3DScene::updateHair(pHairModel cpuHair, pDataBlock vpsData) {
	for (int i = 0; i < hairModels.size(); ++i) {
		if (hairModels[i] == cpuHair) {
			gpuLines[i]->update(vpsData);
			break;
		}
	}
}
void P3DScene::updateMesh() {
	for (int i = 0; i < comps.size(); ++i) {
		//gpuMeshes[i]->update(comps[i]->cache_vps, nullptr, comps[i]->cache_vns);
		using namespace P3D::SHADER_KEY_WORDS;
		gpuMeshes[i]->update(comps[i]->cache_vps, IN_VERT_POS);
		gpuMeshes[i]->update(comps[i]->cache_vns, IN_VERT_NORMAL);
	}
}
void P3DScene::addHair(pHairModel tHair, pShader tShader) {
	auto lineGpu = P3DEngine::instance().createUserGpuGeo();
	auto gpuData = tHair->getGPUData();
	lineGpu->addVertexData(gpuData[0],"InPos",3,0);
	lineGpu->addVertexData(gpuData[2], "hId", 1, 0);
	lineGpu->addElementBuffer(gpuData[1], GeoTYPE::GEO_TYPE_LINES);
	//lineGpu->init(gpuData[0], gpuData[1]);

	if (tShader) {
		tShader->setCamera(mCamera);
		lineGpu->setShader(tShader);
	}
	else
		lineGpu->setShader(defaultShader);
	lineGpu->commit();

	hairModels.push_back(tHair);
	gpuLines.push_back(lineGpu);
}
void P3DScene::addUserGeoGPU(pUserGeoGPU tGeo) {
	tGeo->getShader()->setCamera(mCamera);
	userGeos.push_back(tGeo);
}
void P3DScene::render() {
	P3DEngine::instance().clearBuffers(0.0,0.0,0.0,0.0);

	for (int i = 0; i < hairModels.size(); ++i) {
		gpuLines[i]->setModelMat(hairModels[i]->modelMat);
		gpuLines[i]->draw();
	}
	//P3DEngine::instance().finish();
	for (auto useMesh : userGeos) {
		useMesh->draw();
	}
	//P3DEngine::instance().finish();
	for (int i = 0; i < comps.size(); ++i) {
		gpuMeshes[i]->setModelMat(comps[i]->modelMat);
		gpuMeshes[i]->draw();
	}
//	P3DEngine::instance().finish();
	
}

void P3DScene::removeNode(const std::string& nodeName) {
	// comp
	int i = 0;
	for (i = 0; i < comps.size(); ++i) {
		if(comps[i]->name == nodeName)
			break;
	}
	if (i < comps.size()) {
		comps.erase(comps.begin() + i);
		gpuMeshes.erase(gpuMeshes.begin() + i);
		return;
	}
	i = 0;
	for ( i = 0; i < hairModels.size(); ++i) {
		if(hairModels[i]->name == nodeName)
			break;
	}
	if (i < hairModels.size()) {
		hairModels.erase(hairModels.begin() + i);
		gpuLines.erase(gpuLines.begin() + i);
		return;
	}

	i = 0;
	for ( i = 0; i < userGeos.size(); ++i) {
		if(userGeos[i]->name == nodeName)
			break;
	}
	if (i < userGeos.size()) {
		userGeos.erase(userGeos.begin() + i);
	}
}

pComp P3DScene::findComp(const std::string& tName) {
	for (auto t : comps) {
		if (t->name == tName)
			return t;
	}
	return nullptr;
}

pHairModel P3DScene::findHair(const  std::string& tName) {
	for (auto t : hairModels)
		if (t->name == tName)
			return t;
	return nullptr;
}



END_P3D_NS