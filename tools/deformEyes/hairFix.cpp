#include "hairFix.h"
#include "geo/meshDataIO.h"
#include "embreeScene/embreeDevice.h"
#include "products/LapDeform/laplacian.h"
BEGIN_P3D_NS

void HairFix::init(const std::string& headPath){
	std::cout << "inter init hairFix" << std::endl;
	//headComp = MeshDataIO::instance().loadObjSrc(headPath);
	embreeDevice = std::make_shared<EmbreeDevice>();

	embreeDevice->init("");
	std::cout << "init embree ok" << std::endl;
	embreeScene = embreeDevice->createScene();

	//embreeScene->addMesh(headComp,std::string("head"));
//	embreeScene->addMesh(headModel, false);
	std::cout << "add head model ok" << std::endl;
}

void HairFix::solve(const std::string& hairPath, const std::vector<float32>&rotVec, const std::vector<float32>& tVec, const std::string& dstPath, int radius /* = 10 */) {
	hair = std::make_shared<HairModel>();
	hair->loadFromFile(hairPath);
	Mat4f modelMat = Mat4f::getIdentity();
	int n = 0;
	for (uint32 r = 0; r < 3; ++r) {
		for (uint32 c = 0; c < 3; ++c)
			modelMat[{r, c}] = rotVec[n++];
		modelMat[{r, 3}] = tVec[r];
	}

	embreeScene->clearScene();
//	embreeScene->createScene();

	auto srcPos = headComp->vps->rawData<Vec3f>();
	auto dstPos = headComp->cache_vps->rawData<Vec3f>();
	auto nP = headComp->vps->getSize() / sizeof(Vec3f);
	for (int i = 0; i < nP; ++i) {
		dstPos[i] = modelMat * dstPos[i];
	}
	embreeScene->addMesh(headComp, false);
//	modelMat = Mat4f::getIdentity();
//	embreeScene->setInstanceMatrix(modelMat);
	//embreeScene->setGeoTransfrom("head", modelMat);
	std::cout << "rotmat is " << std::endl;
	modelMat.print();
	

//	modelMat = Mat4f::getIdentity();
	solveRoot(modelMat,radius);
	hair->dumpHair(dstPath);
}

void HairFix::smoothHairRoot(std::vector<Vec3f>& poses, const Vec3f& dstRootPos,int raduis) {
	auto deltaPos = dstRootPos - poses[0];
	poses[0] = dstRootPos;
	float sigma = 1.0;
	for (int i = 1; i < raduis && i < poses.size(); ++i) {
		auto w = exp(-(i*i) / (sigma*sigma));
		poses[i] += deltaPos * w;
	}
}
void HairFix::solveRoot(const Mat4f& modelMat, int radius) {

	float theta = 1e-3;
	auto invMat = modelMat.inv();
	int nCount = 0;
	#pragma omp parallel for
	for (int i = 0; i < hair->dstStrands.size();++i) {
		auto& sd = hair->dstStrands[i];
		auto p1 = sd->pos[0];
		auto worldP1 = modelMat * p1;
		auto invP = invMat * worldP1;

		auto dir = worldP1.normalize();

		//先判断是不是在头里边
		EmbreeHitInfo hitInfo;
		auto zDir = modelMat * Vec4f(0.0, 1.0, 0.0, 0.0);
		zDir.doNormalize();
		auto isHit1 = embreeScene->getHitInfo(worldP1, {zDir.x,zDir.y,zDir.z}, 1e10, hitInfo);
		// 说明在头内部
		if (isHit1) {
			auto dstPos = invMat * hitInfo.pos;
			smoothHairRoot(sd->pos, dstPos, radius);
		}
		else { //说明在头上 先看下跟头皮的距离 将头发视线方向动
			auto useDir = worldP1.normalize();
			auto isHit2 = embreeScene->getHitInfo(worldP1,useDir, 1e10, hitInfo);
			if (isHit2) {
				auto dstPos = invMat * hitInfo.pos;
				smoothHairRoot(sd->pos, dstPos, radius);
			}
			else {
				zDir = modelMat * Vec4f(0.0, -1.0, 0.0, 0.0);
				zDir.doNormalize();
				auto isHit3 = embreeScene->getHitInfo(worldP1, { zDir.x,zDir.y,zDir.z }, 1e10, hitInfo);
				if (isHit3) {
					auto dstPos = invMat * hitInfo.pos;
					smoothHairRoot(sd->pos, dstPos, radius);
				}
				else {
				//	nCount++;
					Vec4f xDir(1.0, 0.0, 0.0, 0.0);
					xDir = (modelMat * xDir).normalize();
					isHit3 = embreeScene->getHitInfo(worldP1, { xDir.x,xDir.y,xDir.z }, 1e10, hitInfo);
					if (isHit3) {
						auto dstPos = invMat * hitInfo.pos;
						smoothHairRoot(sd->pos, dstPos, radius);
					}
					else {
						isHit3 = embreeScene->getHitInfo(worldP1, { -xDir.x,-xDir.y,-xDir.z }, 1e10, hitInfo);
						if (isHit3) {
							auto dstPos = invMat * hitInfo.pos;
							smoothHairRoot(sd->pos, dstPos, radius);
						}
						else {
							zDir = { 0.0,0.0,1.0,0.0 };
							isHit3 = embreeScene->getHitInfo(worldP1, { zDir.x,zDir.y,zDir.z }, 1e10, hitInfo);
							if (isHit3) {
								auto dstPos = invMat * hitInfo.pos;
								smoothHairRoot(sd->pos, dstPos, radius);
							}
							else {
								isHit3 = embreeScene->getHitInfo(worldP1, { -zDir.x,-zDir.y,-zDir.z }, 1e10, hitInfo);
								if (isHit3) {
									auto dstPos = invMat * hitInfo.pos;
									smoothHairRoot(sd->pos, dstPos, radius);
								}
								else
									nCount++;
							}
						}
					}
				}
				//xDir.x = -xDir.x, xDir.y = -xDir.y, xDir.z = -xDir.z;
			}
		}
	}
	std::cout << "unchange pts is " << nCount << std::endl;
}

PNdArrayF HairFix::solveCommonCollision(PNdArrayF allVps, PNdArrayF tls, PNdArrayF srcVps,const std::vector<float32>& tDir,float32 tDelta) {
	auto compVps = allVps.getDataBlock()->clone();
	auto compTls = std::make_shared<DataBlock>(tls.shape()[0] * 3 * sizeof(uint32),DType::Uint32);
	auto comTlsData = compTls->rawData<uint32>();
	doFor(i, tls.shape()[0]) {
		doFor(k, 3)
			comTlsData[i * 3 + k] = uint32(tls[{int(i), int(k)}]);
	}
	auto headModel = std::make_shared<P3DComp>(compVps, compTls, nullptr, nullptr, nullptr);

	embreeScene->clearScene();
	auto tId = embreeScene->addMesh(headModel, false);
	//std::cout << "begin solve collision,head Id is " << tId << std::endl;
	PNdArrayF retVps(srcVps.shape());
	auto retVpsData = retVps.getDataBlock()->rawData<Vec3f>();
	float t_delta = tDelta;
	auto srcVpsData = srcVps.getDataBlock()->rawData<Vec3f>();
	Vec3f cDir(tDir[0], tDir[1], tDir[2]);
	cDir.doNormalize();
#pragma omp parallel for
	for (int i = 0; i < srcVps.shape()[0]; ++i) {
		//std::cout << "now solve strand " << i<<std::endl;
		auto srcPos = srcVpsData[i];
		srcPos = srcPos - cDir * t_delta;
		EmbreeHitInfo tInfo;
		if (embreeScene->getHitInfo(srcPos, cDir, 1e10, tInfo)) {
			srcPos = tInfo.pos;
			float t_mul = 1.0;
			if (cDir.dot(tInfo.normal) < 0.0)
				t_mul = -1.0;
			srcPos = srcPos + tInfo.normal * t_delta * 20*t_mul;
		}
		srcPos += cDir * t_delta *2.0;
		retVpsData[i] = srcPos;
	}
	std::cout << "solve collision  ok" << std::endl;
	return retVps;

}
PNdArrayF HairFix::solveHairCollisionData2(PNdArrayF srcHairData, PNdArrayF hairRootNormals, PNdArrayF vpsData, PNdArrayF tlsData) {
	std::cout << "inter solve collision" << std::endl;
	auto hair = std::make_shared<HairModel>();
	hair->loadFromNdArray(srcHairData);
	std::cout << "hair pos is";
	hair->dstStrands[10]->pos[10].print();
	auto rootNormals = hairRootNormals.getDataBlock()->rawData<Vec3f>();
	std::cout << "head normal is ";
	rootNormals[10].print();

	auto compVps = vpsData.getDataBlock()->clone();
	auto compTls = std::make_shared<DataBlock>(tlsData.shape()[0] * 3 * sizeof(uint32),DType::Uint32);
	auto compTlsData = compTls->rawData<uint32>();
	doFor(i, tlsData.shape()[0]) {
		doFor(k, 3)
			compTlsData[i * 3 + k] = uint32(tlsData[{int(i), int(k)}]);
	}

	auto headModel = std::make_shared<P3DComp>(compVps, compTls, nullptr, nullptr, nullptr);
	solveHairCollision_(hair, headModel, rootNormals);

	auto retHairArray = hair->dumpToNdArray();
	return retHairArray;
}
PNdArrayF HairFix::solveHairCollisionData(PNdArrayF srcHairData, PNdArrayF hairRootNormals, const std::string& headPath) {
	std::cout << "inter solve collision" << std::endl;
	auto hair = std::make_shared<HairModel>();
	hair->loadFromNdArray(srcHairData);
	std::cout << "hair pos is";
	hair->dstStrands[10]->pos[10].print();
	auto rootNormals = hairRootNormals.getDataBlock()->rawData<Vec3f>();
	std::cout << "head normal is ";
	rootNormals[10].print();
	auto headModel = MeshDataIO::instance().loadObj(headPath);
	solveHairCollision_(hair, headModel, rootNormals);
	
	auto retHairArray = hair->dumpToNdArray();
	return retHairArray;
}

bool HairFix::fitSeg(const Vec3f& starP, Vec3f& endP){
	auto tDir = endP - starP;
	auto tLen = tDir.length();
	tDir.doNormalize();

	EmbreeHitInfo hitInfo;
	float64 halfL = tLen * 0.5;
	bool hasHit = false;
	int randCount = 0;
	float32 d_delta = 1e-2;
	int nCount = 0;
	while (embreeScene->getHitInfo(starP, tDir, tLen, hitInfo)) {
		EmbreeHitInfo innerHit;
		if (!embreeScene->getHitInfo(endP, hitInfo.normal, 1e10, innerHit) || nCount>=10) {
			endP = hitInfo.pos + hitInfo.normal*d_delta;
			break;
		}
		endP = innerHit.pos + innerHit.normal * d_delta;
		tDir = endP - starP;
		tLen = tDir.length();
		tDir.doNormalize();
		hasHit = true;
		++nCount;
	}
	return hasHit;

}

void HairFix::fitRoot(std::vector<Vec3f>& srcStrand, const Vec3f& rootNormal,float delta) {
	const auto& starP = srcStrand[0];
	EmbreeHitInfo hitInfo;
	if (embreeScene->getHitInfo(starP, rootNormal, 1e10, hitInfo)) {
		Vec3f newPos;
		if (hitInfo.normal.dot(rootNormal) > 0)
			newPos = hitInfo.pos + hitInfo.normal * delta;
		else
			newPos = hitInfo.pos + hitInfo.normal*delta;
		auto deltaD = newPos - srcStrand[0];
		for (int i = 0; i < srcStrand.size(); ++i)
			srcStrand[i] += deltaD;
	}
}
void HairFix::solveHairCollision_(pHairModel srcHair, pComp headModel,const Vec3f* const rootNormals) {
	embreeScene->clearScene();
	auto tId = embreeScene->addMesh(headModel, false);
	std::cout << "begin solve collision,head Id is " << tId<<std::endl;
	float t_delta = 1e-3;
	#pragma omp parallel for
	for (int i = 0; i < srcHair->dstStrands.size(); ++i) {
		//std::cout << "now solve strand " << i<<std::endl;
		auto strand = srcHair->dstStrands[i];
		fitRoot(strand->pos, rootNormals[i]);
		Vec3f tStart = strand->pos[0];
		int j = 1;
		while (j < strand->pos.size()) {
			auto tEnd = strand->pos[j];
			auto retEnd = tEnd;
			auto hasHit = fitSeg(tStart, retEnd);
			if (hasHit) {
				auto deltaE = retEnd - tEnd;
				for (int k = j; k < strand->pos.size(); ++k)
					strand->pos[k] += deltaE;
			}
			tStart = retEnd;
			++j;
		}
	}
	std::cout << "solve collision  ok" << std::endl;

}

END_P3D_NS
