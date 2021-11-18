#pragma once

#include <vector>

#include <array>
#include <string>
#include "common/mathIncludes.h"
#include <unordered_map>
#include "common/NdArray.h"

BEGIN_P3D_NS
struct CommonSingleStrand {
	std::vector<Vec3f> pos;
	struct JointInfo{
		std::vector<Vec4i16> jointId;
		std::vector<Vec4f> jointWeights;
	};
	JointInfo jointInfo;
};

class HairModel :public P3DObject{
public:

	bool loadFromFile(const std::string& hairPath);
	bool loadFromJson(const std::string& hairPath);
	bool loadFromNdArray(PNdArrayF srcData);
	bool dumpHair(const std::string& hairPaht,int cap = 0);
	void dumpJointInfo(const std::string& fPath);
	void loadJointInfo(const std::string& fPath);

	void convertData();
	std::vector<pDataBlock> getGPUData();
	std::unordered_map<std::string, pDataBlock> getJointGPUData(bool sortHair = false);

	void setModelMat(const Mat4f& m);	
	PNdArrayF dumpToNdArray();
private:
	pDataBlock sortHair(pDataBlock vps, pDataBlock tl);
public:
	uint32 allVex = 0;
	uint32 lineNum = 0;
	Mat4f modelMat = Mat4f::getIdentity();
	std::vector<std::shared_ptr<CommonSingleStrand>> srcStrands;
	std::vector<std::shared_ptr<CommonSingleStrand>> dstStrands;
	std::vector<Vec3f> vps;
	std::vector<int> ncs;
	std::string name;

	
};
typedef std::shared_ptr<HairModel> pHairModel;
END_P3D_NS