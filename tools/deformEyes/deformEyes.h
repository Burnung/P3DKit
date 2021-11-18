#pragma once
#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <math.h>
#include <set>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <chrono>
#include <ctime>
#include <thread>
#include <algorithm>
#include <vector>
#include "common/P3D_Utils.h"
#include "common/NdArray.h"
#include "common/mathVector.hpp"
#include "common/P3D_Utils.h"
#include "embreeScene/embreeScene.h"
#include "embreeScene/embreeDevice.h"
#include "geo/comp.h"

BEGIN_P3D_NS
class EyeDeformer : public P3DObject {
public:
	EyeDeformer() = default;
public:
	void init(const std::string& lEyePath, const std::string& rEyePath, const std::vector<int32>& lHardIdx, const std::vector<int32>& lextraIndex_, \
		const std::vector<int32>& rHardIdx, const std::vector<int32>& rExtraIdx, const std::vector<float32>& lC, const std::vector<float32>& rC);
	void setHeadObj(const std::string& headPath);
	PNdArrayF solve(float32 delta,int neibourSize,float32 mulDelta=5);
	PNdArrayF solveByHardPos(PNdArrayF leftPos, PNdArrayF rightPos, int neibourSize, float32 multDelta);
	PNdArrayF recacleCompNormals(const std::string& objPath);

private:
	void getHardPos(std::vector<int>& idx,std::vector<Vec3f>& newPos,float32 delta);
	bool updateExtraPos(std::vector<int>& idx, std::vector<Vec3f>& newPos,float32 step);

	PNdArrayF commonSolve(std::vector<Vec3f>&newPos, std::vector<int>& hairIdx, int neibourSize, float32 multDelta);

private:
	std::vector<int32> lHardIndex,rHardIndex;
	std::vector<int32> lExtraIndex,rExtraIndex;
	pEmbreeScene embreeScene;
	std::shared_ptr<EmbreeDevice>  embreeDevice;
	pComp headComp = nullptr;
	Vec3f lCenter, rCenter;
	uint32 lEyeId = 0, rEyeId = 0;
	
};




END_P3D_NS

