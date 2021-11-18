#pragma once
#include "simpleCuda.h"

class HairCNNRender {
public:
	HairCNNRender() = default;
	~HairCNNRender() { net = nullptr; }
private:
	void loadConvFromFile(pConvOpt conv, const std::string& fpath);
public:
	void init(const std::string& fDir);
	P3D::pUserGeoGPU loadHairData(const std::string& fDir);
	pTensor forward();
	void setInputTexs(const std::vector<P3D::pTexture>& texs);
public:
	std::vector<pTensor> srcInputs;
	pTensor useInput;
	pNet net;
	P3D::pUserGeoGPU bigHairGpu;
};
