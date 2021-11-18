#pragma once
#include "engine/P3dEngine.h"
#include "common/NdArray.h"
#include "engine/renderFrame.h"

class HairGLRender {
public:
	HairGLRender() = default;
	void init(const std::string& hairPath, const std::string& vsPath, const std::string& psPath,P3D::uint32 mrt = 1);
	void setWinSize(int w, int h);
	P3D::PNdArrayF render(float camX, float camY, float camZ);
	void setOrthWidth(float w);
	void setModelEuler(float px, float py, float pz, float rotx, float roty, float rotz);
	
public:
	P3D::uint32 mrt;
	P3D::uint32 winW = 512, winH = 512;
	P3D::pComp mComp;
	P3D::pWindow mWindow;
	P3D::pScene mScene;
	P3D::pCamera mCamera;
	P3D::pRenderFrame mFbo;

};


