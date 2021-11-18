#pragma once
#include "P3DBone.h"
BEGIN_P3D_NS

class P3DSkeleton :public P3DObject {
public:
	//simple key frame
	struct SkeletonKeyFrameAni {
		std::vector<Mat4f> l2pMats;
	};
public:
	pBone findBoneByName(const std::string& tName);
	std::shared_ptr<SkeletonKeyFrameAni> createKeyFrameAni();
	void addKeyFrameAnimation(std::shared_ptr<SkeletonKeyFrameAni> keyAni) {
		keyFrameAniSeq.push_back(keyAni);
	}
	//展开所有骨骼变换
	void flattenBones();
	bool applyAnimation(int frameId);
	bool updateByBones(const std::unordered_map<std::string,std::vector<float32>>& boneInfos);
	bool resetBones();
	std::vector<float32> getBoneBaseEuler(std::string& boneName);
	void print(bool bTranspose = false);
private:
	void flattenBone(pBone tBone);
public:
	std::vector<std::shared_ptr<P3DBone>> rootBones;
	std::vector<std::shared_ptr<P3DBone>> bones;
	std::vector<std::shared_ptr<SkeletonKeyFrameAni>> keyFrameAniSeq;
};
typedef std::shared_ptr<P3DSkeleton> pSkeleton;

END_P3D_NS
