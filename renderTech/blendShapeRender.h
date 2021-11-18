#pragma once
#include "engine/P3dEngine.h"
#include "common/NdArray.h"
#include "engine/renderFrame.h"
BEGIN_P3D_NS
class BlendShapeRender{
public:
	BlendShapeRender() = default;
	void init(const std::vector<std::string>& objPathes, const std::string& vsPath, const std::string& psPath, P3D::uint32 mrt = 1);
	void setWinSize(int w, int h);
	P3D::PNdArrayF render(const std::vector<float32>& bsCoeff, float camX, float camY, float camZ);
	void setOrthWidth(float w);
	void setModelEuler(float px, float py, float pz, float rotx, float roty, float rotz);
	pRenderFrame renderFbo(const std::vector<float32>& bsCoeff, float32 camX, float32 camY, float32 camZ);
	pShader getShader() {
		return shader;
	}
	pCamera getCamera() { return mCamera; }

	pWindow getWin() { return mWindow; }

public:
	P3D::uint32 mrt;
	P3D::uint32 winW = 512, winH = 512;
	P3D::pComp mComp;
	P3D::pWindow mWindow;
	P3D::pScene mScene;
	P3D::pCamera mCamera;
	P3D::pRenderFrame mFbo;
	P3D::pShader shader;
};
END_P3D_NS
