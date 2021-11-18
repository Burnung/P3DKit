#include "common/P3D_Utils.h"
#include "geo/comp.h"
#include "Shader.h"
#include "geo/hairModel.h"
#include "MeshGpu.h"
#include "LineGPU.h"
#include "Camera.h"
#include "userGeoGpu.h"
#pragma once
BEGIN_P3D_NS

class P3DScene : public P3DObject {
public:
	P3DScene();
	virtual ~P3DScene();
public:
	void render();
	void release();
	void addComp(pComp tComp,pShader tShader = nullptr);
	void addHair(pHairModel tHair, pShader tShader = nullptr);
	void addUserGeoGPU(pUserGeoGPU tGeo);
	void updateMesh();
	pCamera getCamera() {
		return mCamera;
	}
	void updateHair(pHairModel, pDataBlock);

	void removeNode(const std::string& nodeName);

	pComp findComp(const std::string& tName);
	pHairModel findHair(const std::string& tName);

public:
	pShader defaultShader;
	std::vector<pComp> comps;
	std::vector<pShader> shaders;
	std::vector<pHairModel> hairModels;
	std::vector<pUserGeoGPU> gpuLines;
	std::vector<pRenderable> gpuMeshes;
	std::vector<pUserGeoGPU> userGeos;
	pCamera mCamera = nullptr;
};
typedef std::shared_ptr<P3DScene> pScene;
END_P3D_NS