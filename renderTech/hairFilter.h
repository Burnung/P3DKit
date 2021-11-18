#pragma once
#include "engine/P3dEngine.h"
#include "common/NdArray.h"
#include "engine/renderFrame.h"
#include "common/P3D_Utils.h"
#include "engine/MeshGpu.h"
BEGIN_P3D_NS
class HairFilter :public P3DObject {
public:
	HairFilter() = default;
	virtual ~HairFilter() {};
public:
	void init(const std::string& work_dir);
	PNdArrayU8 filter(PNdArrayU8 srcImg, PNdArrayU8 maskImg,int mode);
	pRenderFrame filterFbo(PNdArrayU8 srcImg, PNdArrayU8 maskImg,int mode);
	pRenderFrame filterFbo(const std::string& srcPath, const std::string& maskPath, int mode);
	pRenderFrame filterFbo(pCPuImage srcImg, pCPuImage maskImg, int mode);

private:
	void setupFbo();
	void addFbo(pRenderFrame& tFbo);
	void drawScene(pRenderFrame tFbo, pShader tShader, pTexture tex);
	void renderGaussianPass1D(int mode);
	void renderSmoothPass(pRenderFrame tFbo, pTexture tex, int mode);
	void renderBlendingPass(pRenderFrame tFbo, pTexture tex1, pTexture tex2);
	void renderDetailPass(pRenderFrame tFbo, pTexture tex,int mode);

private:
	int mWidth = 0, mHeight = 0;
	float32 mSteps = 10.0;
	float32 mSigma = 4.0f;
	pUserGeoGPU quad;
	pMeshGPU quadVbo;
	pShader gradientShader;
	pShader gaussianShader;
	pShader smoothShader;
	pShader rgb2LabShader;
	pShader lab2RgbShader;
	pShader quantizationShader;
	pShader blendShader;
	pShader composeShader;
	pShader detailShader;

	pShader basicShader;

	pRenderFrame gradinetFbo, guassionFbo, rgbFbo, LabFbo, tmpFbo, tmpFbo2, quantFbo;
	pTexture detailTex, srcTex, maskTex;
	pWindow win;
};



END_P3D_NS
