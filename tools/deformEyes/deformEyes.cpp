#include "deformEyes.h"
#include "geo/meshDataIO.h"
#include "embreeScene/embreeDevice.h"
#include "products/LapDeform/laplacian.h"
BEGIN_P3D_NS

void EyeDeformer::init(const std::string& lEyePath, const std::string& rEyePath,const std::vector<int32>& lHardIdx, const std::vector<int32>& lextraIndex_, \
		const std::vector<int32>& rHardIdx, const std::vector<int32>& rExtraIdx, const std::vector<float32>& lC, const std::vector<float32>& rC){
	std::cout << "inter init eyeDeformer" << std::endl;
	auto lEyeModel = MeshDataIO::instance().loadObjSrc(lEyePath);
	auto rEyeModel = MeshDataIO::instance().loadObjSrc(rEyePath);
	embreeDevice = std::make_shared<EmbreeDevice>();

	embreeDevice->init("");
	std::cout << "init embree ok" << std::endl;
	embreeScene = embreeDevice->createScene();

	lEyeId = embreeScene->addMesh(lEyeModel, false);
	rEyeId = embreeScene->addMesh(rEyeModel, false);
	
	std::cout << "add eye model ok" << std::endl;

	std::cout << "lhard shape is " << lHardIdx.size() << std::endl;
	lHardIndex = lHardIdx;
	lExtraIndex = lextraIndex_;
	rHardIndex = rHardIdx;
	rExtraIndex = rExtraIdx;

	lCenter = lC;
	rCenter = rC;
	std::cout << "left eye id is " << lEyeId << ",right eye id is " << rEyeId << std::endl;
}

void EyeDeformer::setHeadObj(const std::string& headPath) {
	auto tHead = MeshDataIO::instance().loadObjSrc(headPath);
	if (headComp == nullptr) {
		headComp = tHead;
		headComp->cache_vps = headComp->vps;
		//embreeScene->addMesh(headComp);
	}
	else {
		headComp->cache_vps = tHead->vps;
		//embreeScene->updateScene();
	}
}
PNdArrayF EyeDeformer::solveByHardPos(PNdArrayF leftPos, PNdArrayF rightPos, int neibourSize, float32 multDelta) {
	vector<Vec3f> newPos;
	newPos.reserve(leftPos.shape()[0] + rightPos.shape()[0]);
	for (int i = 0; i < leftPos.shape()[0]; ++i) {
		newPos.push_back({ leftPos[{i,0}],leftPos[{i,1}],leftPos[{i,2}] });
	}
	for (int i = 0; i < rightPos.shape()[0]; ++i) {
		newPos.push_back({ rightPos[{i,0}],rightPos[{i,1}],rightPos[{i,2}] });
	}
	vector<int> hardIdxs;
	hardIdxs.reserve(lHardIndex.size() + rHardIndex.size());
	hardIdxs.insert(hardIdxs.begin(), lHardIndex.begin(), lHardIndex.end());
	hardIdxs.insert(hardIdxs.end(), rHardIndex.begin(), rHardIndex.end());
	return commonSolve(newPos, hardIdxs, neibourSize, multDelta);

}
PNdArrayF EyeDeformer::recacleCompNormals(const std::string& objPath) {
	auto tComp = MeshDataIO::instance().loadObjSrc(objPath);
	tComp->recalculNormals();
	PNdArrayF retNormals({ int(tComp->cache_vps->getSize() / sizeof(Vec3f)) ,3 });
	auto tData = retNormals.getDataBlock();
	tData->copyFromData(tComp->cache_vns);
	return retNormals;
}
PNdArrayF EyeDeformer::commonSolve(std::vector<Vec3f>&newPos,std::vector<int>& hardIdex, int neibourSize, float32 multDelta) {

	auto lapSolver = std::make_shared<LaplacianDeform>();
	lapSolver->init(headComp->cache_vps, headComp->tl);

	bool needUpdate = true;
	int nCouont = 0;
	while (needUpdate) {
		headComp->cache_vps = lapSolver->deformMesh(hardIdex, newPos, neibourSize);
		needUpdate = updateExtraPos(hardIdex, newPos, multDelta);
		//needUpdate = false;
		std::cout << "update count " << nCouont++ << endl;
		//break;
	}
	int nVert = headComp->cache_vps->getSize() / sizeof(float32) / 3;
	PNdArrayF retArray({ nVert,3 });
	auto srcData = headComp->cache_vps->rawData<float32>();
	for (int i = 0; i < nVert; ++i) {
		for (int j = 0; j < 3; ++j)
			retArray[{i, j}] = srcData[i * 3 + j];
	}

	for (int i = 0; i < hardIdex.size(); ++i) {
		for (int j = 0; j < 3; ++j)
			retArray[{hardIdex[i], j}] = newPos[i][j];
	}

	return retArray;
}
PNdArrayF EyeDeformer::solve(float32 delta,int neibourSize,float32 multDelta) {

	//auto vData = headComp->cache_vps->rawData<float32>();

	std::vector<int> hardIdex;
	std::vector<Vec3f> newPos;
	getHardPos(hardIdex, newPos, delta);
	std::cout << "hard index size is " << hardIdex.size() << ",new pos is " << newPos.size() << std::endl;

	//updateExtraPos(hardIdex, newPos, delta);
	return commonSolve(newPos, hardIdex, neibourSize, multDelta);
/*
	bool needUpdate = true;
	int nCouont = 0;
	while (needUpdate) {
		headComp->cache_vps  = lapSolver->deformMesh(hardIdex, newPos, neibourSize);
		needUpdate = updateExtraPos(hardIdex, newPos,delta*multDelta);
		std::cout << "update count " << nCouont++ << endl;
		//break;
	}
	int nVert = headComp->cache_vps->getSize() / sizeof(float32) / 3;
	PNdArrayF retArray({ nVert,3 });
	auto srcData = headComp->cache_vps->rawData<float32>();
	for (int i = 0; i < nVert; ++i) {
		for (int j = 0; j < 3; ++j)
			retArray[{i, j}] = srcData[i * 3 + j];
	}

	for (int i = 0; i < hardIdex.size();++i) {
		for (int j = 0; j < 3; ++j)
			retArray[{hardIdex[i], j}] = newPos[i][j];
	}	
	
	return retArray;*/

}

void EyeDeformer::getHardPos(std::vector<int>& idx, std::vector<Vec3f>& newPos,float32 delta) {
	auto vData = headComp->cache_vps->rawData<float32>();

	std::cout << "lCenter is ";
	lCenter.print();
	for (auto tidx : lHardIndex) {
		idx.push_back(tidx);
		Vec3f tPos = { vData[tidx * 3],vData[tidx * 3 + 1],vData[tidx * 3 + 2] };
		auto tv = (tPos - lCenter).normalize();
		//std::cout << "tPos is ";
		//tPos.print();
		EmbreeHitInfo hitInfo;
		auto isHit = embreeScene->getHitInfo(tPos, tv, 100.0, hitInfo);
		if (isHit && hitInfo.hitId == lEyeId) {
			newPos.push_back(hitInfo.pos + hitInfo.normal * delta);
			//newPos.push_back(tPos);
		//	std::cout << "in left eye" << std::endl;
		}
		else {
			auto tvN = lCenter - tPos;
			auto len = tvN.length();
			tvN.doNormalize();
			auto isHit = embreeScene->getHitInfo(tPos, tvN, len, hitInfo);
			if (isHit&&hitInfo.hitId == lEyeId) {
				newPos.push_back(hitInfo.pos + hitInfo.normal* delta);
				//std::cout << "is in outer" << std::endl;
			}
			else {
				newPos.push_back(tPos);
			}
		}
	}

	for (auto tidx : rHardIndex) {
		idx.push_back(tidx);
		Vec3f tPos = { vData[tidx * 3],vData[tidx * 3 + 1],vData[tidx * 3 + 2] };
		auto tv = (tPos - rCenter).normalize();
		EmbreeHitInfo hitInfo;
		auto isHit = embreeScene->getHitInfo(tPos, tv, 100.0, hitInfo);
		if (isHit && hitInfo.hitId == rEyeId) {
			newPos.push_back(hitInfo.pos + hitInfo.normal * delta);
		}
		else {
			auto tvN = rCenter - tPos;
			auto len = tvN.length();
			tvN.doNormalize();
			auto isHit = embreeScene->getHitInfo(tPos, tvN, len, hitInfo);
			if (isHit && hitInfo.hitId == rEyeId) {
				newPos.push_back(hitInfo.pos + hitInfo.normal * delta);
			}
			else {
				newPos.push_back(tPos);
			}
		}
	}

}

bool EyeDeformer::updateExtraPos(std::vector<int>& idx, std::vector<Vec3f>& newPos,float32 delta) {
	auto vData = headComp->cache_vps->rawData<float32>();

	bool needUpdate = false;

	for (auto tidx : lExtraIndex) {
		Vec3f tPos = { vData[tidx * 3],vData[tidx * 3 + 1],vData[tidx * 3 + 2] };
		Vec3f tV = tPos - lCenter;
		auto len = tV.length();
		tV.doNormalize();
		EmbreeHitInfo hitInfo;
		auto isHit = embreeScene->getHitInfo(tPos, tV, len*10, hitInfo);
		if (isHit && hitInfo.hitId == lEyeId) {
			std::cout << "add extar id " << tidx << std::endl;
			idx.push_back(tidx);
			auto tHitPos = hitInfo.pos + tV * delta;
			//auto tHitPos = hitInfo.pos + hitInfo.normal * delta;
			newPos.push_back(tHitPos);
			needUpdate = true;
		}
	}

	for (auto tidx : rExtraIndex) {
		Vec3f tPos = { vData[tidx * 3],vData[tidx * 3 + 1],vData[tidx * 3 + 2] };
		Vec3f tV = tPos - rCenter;
		auto len = tV.length();
		tV.doNormalize();
		EmbreeHitInfo hitInfo;
		auto isHit = embreeScene->getHitInfo(tPos, tV, len*1.5, hitInfo);
		if (isHit && hitInfo.hitId == rEyeId) {
			//std::cout << "add extar id " << tidx << std::endl;
			idx.push_back(tidx);
			auto tHitPos = hitInfo.pos + tV * delta;
			//auto tHitPos = hitInfo.pos + hitInfo.normal * delta;
			newPos.push_back(tHitPos);
			needUpdate = true;
		}
	}
	return needUpdate;

}
END_P3D_NS