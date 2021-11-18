#pragma once
#include "vrayRender.h"
#include "common/P3D_Utils.h"
#include <unordered_map>
#include "geo/P3DSkinMesh.h"

BEGIN_P3D_NS
class VrayRenderController :public P3DObject{
public:
	VrayRenderController() = default;
	~VrayRenderController() {
		mRender=nullptr;
		mMesh=nullptr;
	}
public:
	void initFromConfig(const std::string& cfgPath);
	void setAnimation(const std::unordered_map<std::string, std::vector<float32> >&bonePos, const std::vector<float32>& bs);
	void renderImg(const std::string& imgPath,float32 gamma);
	std::vector<float32> getBoneBaseEuler(std::string& boneName);
	PNdArrayF renderRaw();

private:
	std::shared_ptr<VrayRender> mRender;
	pSkinMesh mMesh;

	std::unordered_map<std::string, std::string> nodeMap;
};
END_P3D_NS
