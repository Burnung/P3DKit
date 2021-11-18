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
#include "geo/hairModel.h"

BEGIN_P3D_NS
class HairFix: public P3DObject {
public:
	HairFix() = default;
public:
	void init(const std::string& headPath);
	void solve(const std::string& hairPaht,const std::vector<float32>& rotMatVec,const std::vector<float32>& tVec,const std::string&dstPath, int radius = 10);
	void solveRoot(const Mat4f& modelMat,int radius);

	//void solveHairCollision(const std::string& srcHairPath, const std::string& headPath, const std::string& dstHairPath);
	PNdArrayF solveHairCollisionData(PNdArrayF srcHairData,PNdArrayF hairRootNormals,const std::string& headPath);
	PNdArrayF solveHairCollisionData2(PNdArrayF srcHairData,PNdArrayF hairRootNormals,PNdArrayF vpsData,PNdArrayF tlsData);

	PNdArrayF solveCommonCollision(PNdArrayF allVps, PNdArrayF tls, PNdArrayF srcVps,const std::vector<float32>& tDir,float32 tDelta);

private:
	void solveHairCollision_(pHairModel srcHair,pComp headModel,const Vec3f* const rootNormals);
	void smoothHairRoot(std::vector<Vec3f>& poses, const Vec3f& dstRootPos,int raduis);
	bool fitSeg(const Vec3f& starP, Vec3f& endP);
	void fitRoot(std::vector<Vec3f>& srcStrand, const Vec3f& rootNormal,float32 delta=1e-2);
private:

	pEmbreeScene embreeScene;
	std::shared_ptr<EmbreeDevice>  embreeDevice;
	pComp headComp = nullptr;
	pHairModel hair;
	Vec3f lCenter, rCenter;
	
};




END_P3D_NS

