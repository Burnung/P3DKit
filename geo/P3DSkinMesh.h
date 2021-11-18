#pragma once
#include "comp.h"
#include "P3DSkeleton.h"
#include "P3DAnimation.h"
#include "common/P3D_Utils.h"

BEGIN_P3D_NS



class P3DSkinMesh :public P3DObject {
public:
	P3DSkinMesh() = default;
	virtual ~P3DSkinMesh() {
		comps.clear();
		skeleton = nullptr;
	};

	void addComp(const std::string& compName, pComp tComp);
	pComp getComp(const std::string& compName);
	
	void updateBs(const std::vector<float32>& bsW);

	void updateSkeletonComps();
	void setKeyAnimationFrame(int frameId);

	void updateByBones(const std::unordered_map<std::string, std::vector<float32>>& boneInfo);
	std::vector<float32> getBoneBaseEuler(std::string& boneName);
	void printSkeleton(bool bTranspose = false);
private:
	void updateSkeletonComp(pComp tComp, pSkinInfo skinInfo);

public:
	std::string meshName;
	std::vector<pComp> comps;
	std::vector<pSkinInfo> skinInfos;
	pSkeleton skeleton;

};
typedef std::shared_ptr<P3DSkinMesh> pSkinMesh;


END_P3D_NS
