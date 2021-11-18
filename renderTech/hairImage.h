#pragma once

#include "common/RenderCommon.h"
#include "engine/P3dEngine.h"
#include "common/NdArray.h"
#include "engine/renderTechnique.h"

BEGIN_P3D_NS

class HairImageTech :P3DObject {
public:
	HairImageTech();

public:
	void init(const std::string& dataDir);

	PNdArrayF render(PNdArray srcImg, const std::vector<float>& modelMat);
	pRenderFrame renderTech(PNdArray srcImg, const std::vector<float>& modelMat);
	pWindow getWin() { return win; }

private:
	pTechnique setupScene(PNdArray srcImg, const std::vector<float>& modelMat);

private:

	pWindow win;
	pUserGeoGPU pHair, pQuad;
	pMeshGPU pHead;
	pShader pHairShader, pHeadShader, pQuadShader;
	
	std::vector<std::vector<Vec3f>> animalFrame;


};


END_P3D_NS
