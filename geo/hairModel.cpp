#include "hairModel.h"
#include <fstream>
#include<iostream>
#include <../common/json_inc.h>

using namespace std;

BEGIN_P3D_NS

bool HairModel::loadFromJson(const std::string& hairPath) {
	srcStrands.clear();
	dstStrands.clear();
	auto jsonStr = Utils::readFileString(hairPath);
	auto rootObj = P3djson::parse(jsonStr);

	int strandNum = rootObj.size();

	srcStrands.reserve(strandNum);
	dstStrands.reserve(strandNum);
	for (auto& stObj : rootObj) {
		auto strand = std::make_shared<CommonSingleStrand>();
		int nPoints = stObj.size();
		strand->pos.reserve(nPoints);
		for (auto& tp : stObj) {
			strand->pos.push_back(Vec3f(tp[0], tp[1], tp[2]));
		}
		if (strand->pos.size() >= 2) {
			srcStrands.push_back(strand);
			auto tSd = std::make_shared<CommonSingleStrand>();
			tSd->pos = strand->pos;
			dstStrands.push_back(tSd);
		}
	}
	return true;

}
bool HairModel::loadFromNdArray(PNdArrayF srcData) {
	srcStrands.clear();
	dstStrands.clear();
	auto nStrand = srcData.shape()[0];
	auto nPoint = srcData.shape()[1];
	srcStrands.resize(nStrand);
	dstStrands.resize(nStrand);
	std::cout << "hair size is " << nStrand << "x"<<nPoint << std::endl;
	for (int i = 0; i < nStrand; ++i) {
		auto strand = std::make_shared<CommonSingleStrand>();
		strand->pos.resize(nPoint);
		for (int j = 0; j < nPoint; ++j)
			strand->pos[j] = { srcData[{i,j,0}],srcData[{i,j,1}],srcData[{i,j,2}] };
		srcStrands[i] = strand;
		auto strand2 = std::make_shared<CommonSingleStrand>();
		strand2->pos = strand->pos;
		dstStrands[i] = strand2;
	}
	return true;
}

PNdArrayF HairModel::dumpToNdArray() {
	std::cout << "ndarray size is " << dstStrands.size() << "x" << dstStrands[0]->pos.size() << std::endl;
	PNdArrayF retHair({ (int)dstStrands.size(),(int)dstStrands[0]->pos.size(),3 });
	for (int i = 0; i < dstStrands.size(); ++i) {
		const auto& strand = dstStrands[i]->pos;
		for (int j = 0; j < strand.size(); ++j)
			for (int k = 0; k < 3; ++k)
				retHair[{i, j, k}] = strand[j][k];
	}
	std::cout << "dump hair ok" << std::endl;
	return retHair;
}

bool HairModel::loadFromFile(const std::string& hairPath) {

	srcStrands.clear();
	dstStrands.clear();

	std::ifstream file;

	file.open(hairPath, std::ios::binary);
	if (!file.is_open()) {
		std::cout << "error open " << hairPath << std::endl;
		return false; 
	}
	std::cout << "beign to load hair: " << hairPath << std::endl;
	int strandNum = 0;
	file.read((char*)&strandNum, sizeof(strandNum));
	srcStrands.reserve(strandNum);
	dstStrands.reserve(strandNum);
	for (int i = 0; i < strandNum; i++) {
		auto strand = std::make_shared<CommonSingleStrand>();
		int nPoints = 0;
		file.read((char*)&nPoints, sizeof(nPoints));
		strand->pos.reserve(nPoints);
		for (int j = 0; j < nPoints; ++j) {
			float p[3];
			file.read((char*)p, sizeof(float) * 3);
			strand->pos.push_back( Vec3f(p[0],p[1],p[2]));
		}
		if (strand->pos.size() >= 2) {
			srcStrands.push_back(strand);
			auto tSd = std::make_shared<CommonSingleStrand>();
			tSd->pos = strand->pos;
			dstStrands.push_back(tSd);
		}
	}
	std::cout << "load Hair Ok,hair strand is " << strandNum << std::endl;

	file.close();
	return true;
}

void HairModel::convertData() {
	vps.clear();
	ncs.clear();
	vps.reserve(srcStrands.size() * 100);
	ncs.reserve(srcStrands.size());
	for (auto strand : srcStrands) {
		ncs.push_back(strand->pos.size());
		for (auto p : strand->pos)
			vps.push_back(p);
	}
}

void HairModel::setModelMat(const Mat4f& m){
	modelMat = m;
}	

std::vector<pDataBlock> HairModel::getGPUData() {
	allVex = 0;
	lineNum = 0;
	for (auto strand : dstStrands) {
		allVex += strand->pos.size();
		if (strand->pos.size() >= 2)
			lineNum += strand->pos.size() - 1;
	}
	auto vpsBuffer = std::make_shared<DataBlock>(allVex * sizeof(Vec3f),DType::Float32);
	auto idBuffer = std::make_shared<DataBlock>(allVex * sizeof(float), DType::Float32);
	auto tlBuffer = std::make_shared<DataBlock>(lineNum * 2 * sizeof(uint32),DType::Uint32);
	uint32 offset = 0;
	uint32 indexOffset = 0;
	uint32* tlRawData = tlBuffer->rawData<uint32>();
	for (auto st : dstStrands) {
		uint32 nSize = st->pos.size();
		vpsBuffer->copyData((uint8*)(st->pos.data()), nSize*sizeof(Vec3f), offset*sizeof(Vec3f));
		uint32 tmpOffset = 0;
		if (nSize >= 2) {
			for (int i = 0; i < nSize - 1; ++i) {
				tlRawData[(indexOffset + i) * 2] = i + offset;
				tlRawData[(indexOffset + i) * 2 + 1] = i + offset + 1;
				tmpOffset++;
			}
			indexOffset += tmpOffset;
		}
		offset += nSize;
	}

	uint32 allOffset = 0;
	auto idData = idBuffer->rawData<float32>();
	for (int i = 0; i < dstStrands.size();++i) {
		auto nSize = dstStrands[i]->pos.size();
		for (int j = 0; j < nSize; ++j)
			idData[allOffset++] = float32(i);
	}

	//auto retTl = sortHair(vpsBuffer, tlBuffer);
	//Utils::writeFileData(R"(D:\Work\Projects\ProNew\Vray\tmp\testNet\menglu_yellow_v2\tl.data)", retTl->rawData<char>(), retTl->getSize());
	/*
	auto vpsBuffer = std::make_shared<DataBlock>(4 * sizeof(Vec3f));
	auto tlBuffer = std::make_shared<DataBlock>(4 * 2 * sizeof(uint32));
	std::vector<Vec3f> vers(4);
	vers[0] = Vec3f(-1.0, 0.0, 0.0);
	vers[1] = Vec3f(0.0, 1.0, 0.0);
	vers[2] = Vec3f(1.0, 0.0, 0.0);
	vers[3] = Vec3f(0.0, -1.0, 0.0);
	vpsBuffer->setFromMem((uint8*)(vers.data()), vpsBuffer->getSize());
	float32* vpsData = vpsBuffer->rawData<float32>();
	uint32 indice[] = { 0,1 , 1, 2, 2, 3, 3, 0};
	tlBuffer->setFromMem((uint8*)indice, tlBuffer->getSize());*/	
	return {vpsBuffer,tlBuffer,idBuffer};
}
bool HairModel::dumpHair(const std::string& hairPaht,int cap) {
	std::ofstream of(hairPaht, std::ios::binary);
	int hairNum = srcStrands.size();
	of.write((const char*)&hairNum, sizeof(hairNum));
	for (auto strand : srcStrands) {
		vector<Vec3f> tmpPos;
		for (uint32 k = 0; k < strand->pos.size(); k += (cap + 1)) {
			tmpPos.push_back(strand->pos[k]);
		}
		uint32 pNum = tmpPos.size();
		of.write((const char*)&pNum, sizeof(pNum));
		uint32 tsize = sizeof(tmpPos[0]) * tmpPos.size();
		of.write((const char*)(tmpPos.data()), tsize);
	}
	of.close();

	return true;
}
void HairModel::dumpJointInfo(const std::string& fPath) {
	std::ofstream of(fPath);
	of << srcStrands.size() << endl;
	for (auto sd : srcStrands) {
		auto& jointInfo = sd->jointInfo;
		of << jointInfo.jointId.size()<< endl;
		for (int j = 0; j < jointInfo.jointId.size(); ++j) {
			of << jointInfo.jointId[j][0] << " " << jointInfo.jointId[j][1] <<" "<<jointInfo.jointId[j][2] << " " << jointInfo.jointId[j][3] << std::endl;
			of << jointInfo.jointWeights[j][0] << " " << jointInfo.jointWeights[j][1] <<" "<<jointInfo.jointWeights[j][2] << " " << jointInfo.jointWeights[j][3] << std::endl;
		}
	}
	of.close();
}

void HairModel::loadJointInfo(const std::string& fPath) {
	std::ifstream inF(fPath);
	auto ttt = inF.is_open();
	uint32 nCount;
	inF >> nCount;
	if (nCount != srcStrands.size()) {
		P3D_THROW_RUNTIME("error hair size should be equal!");
	}
	for (auto strand : srcStrands) {
		int pNum;
		auto& jointInfos = strand->jointInfo;
		inF >> pNum;
		P3D_ENSURE(pNum == strand->pos.size(), "Hairmodel::LoadJintInfo: joint size should equal to pos size!");
		jointInfos.jointId.resize(pNum);
		jointInfos.jointWeights.resize(pNum);
		for (int j = 0; j < pNum; ++j) {
			inF >> jointInfos.jointId[j][0] >> jointInfos.jointId[j][1]>>jointInfos.jointId[j][2]>>jointInfos.jointId[j][3];
			inF >> jointInfos.jointWeights[j][0] >> jointInfos.jointWeights[j][1]>>jointInfos.jointWeights[j][2]>>jointInfos.jointWeights[j][3];
			}
	}


	inF.close();
	int16 maxId = 0;

	for (auto st : srcStrands) {
		auto& hairId = st->jointInfo.jointId;
		for (auto& ti : hairId) {
			maxId = std::max(maxId, std::max(std::max(ti[0], ti[1]), std::max(ti[2], ti[3])));
		}
	}
	std::cout << "max id is " << maxId << std::endl;
}

pDataBlock HairModel::sortHair(pDataBlock vps, pDataBlock tl) {
	uint32* pT1 = tl->rawData<uint32>();
	float* pVps = vps->rawData<float32>();
	int nT1 = tl->getSize() / sizeof(uint32) / 2;
	std::cout << "line num is " << nT1 << std::endl;
	std::vector<float32> zDis(nT1);
	std::vector<uint32> tIdx(nT1);

	for (int i = 0; i < nT1; ++i) {
		zDis[i] = pVps[pT1[i * 2] * 3 + 2] + pVps[pT1[i * 2 + 1] * 3 + 2];
		tIdx[i] = i;
	}
	for (int i = 0; i < zDis.size() - 1; ++i) {
		for (int j = i + 1; j < zDis.size(); ++j) {
			if (zDis[i] > zDis[j]) {
				std::swap(zDis[i], zDis[j]);
				std::swap(tIdx[i], tIdx[j]);
			}
		}
	}
	auto retT1 = std::make_shared<DataBlock>(tl->getSize(), DType::Uint32);
	uint32* retPt1 = retT1->rawData<uint32>();
	for (int i = 0; i < tIdx.size(); ++i) {
		retPt1[i * 2] = pT1[tIdx[i] * 2];
		retPt1[i * 2 + 1] = pT1[tIdx[i] * 2 + 1];
	}
	return retT1;
}

std::unordered_map<std::string, pDataBlock> HairModel::getJointGPUData(bool bsortHair) {
	uint32 allVex = 0;

	lineNum = 0;
	allVex = 0;
	for (auto strand : dstStrands) {
		allVex += strand->pos.size();
		if (strand->pos.size() >= 2)
			lineNum += strand->pos.size() - 1;
	}
	unordered_map<string, pDataBlock> ret;
	std::vector<uint32> Ids;
	std::vector<Vec2f> weights;

	auto jointIds = std::make_shared<DataBlock>(4*allVex*sizeof(int16), DType::Int16);
	auto ttsss = sizeof(int16);
	auto  jointWeight = std::make_shared<DataBlock>(4 * allVex * sizeof(float32), DType::Float32);

	auto vpsBuffer = std::make_shared<DataBlock>(allVex * sizeof(Vec3f),DType::Float32);
	auto tlBuffer = std::make_shared<DataBlock>(lineNum * 2 * sizeof(uint32),DType::Uint32);

	uint32 offset = 0;
	uint32 indexOffset = 0;
	uint32 idx = 0;
	uint32* tlRawData = tlBuffer->rawData<uint32>();
	for (auto st : dstStrands) {
		uint32 nSize = st->pos.size();
		vpsBuffer->copyData((uint8*)(st->pos.data()), nSize*sizeof(Vec3f), offset*sizeof(Vec3f));
		//for(int i =0; i < st->jointInfos[0].size();++)

		uint32 tmpOffset = 0;
		if (nSize >= 2) {
			for (int i = 0; i < nSize - 1; ++i) {
				tlRawData[(indexOffset + i) * 2] = i + offset;
				tlRawData[(indexOffset + i) * 2 + 1] = i + offset + 1;
				tmpOffset++;
			}
			indexOffset += tmpOffset;
		}
		offset += nSize;
	}
	
	auto pId = jointIds->rawData<int16>();
	auto pWeight = jointWeight->rawData<float32>();
	offset = 0;
	for (auto st : srcStrands) {
		const auto& jInfo = st->jointInfo;
		auto nSize = jInfo.jointId.size();
		jointIds->copyData((uint8*)(jInfo.jointId.data()), nSize * sizeof(Vec4i16), offset * sizeof(Vec4i16));
		jointWeight->copyData((uint8*)jInfo.jointWeights.data(), nSize * sizeof(Vec4f), offset * sizeof(Vec4f));
		offset += nSize;
	}

	if (bsortHair) {
		tlBuffer = sortHair(vpsBuffer, tlBuffer);
	}

	auto disInfo = std::make_shared<DataBlock>(allVex * sizeof(float), DType::Float32);
	offset = 0;
	float* pDisInfo = disInfo->rawData<float>();

	int nStrand = 0;
	for (auto st : srcStrands) {

		std::vector<float> tDis(st->pos.size());
		tDis[0] = 0;

		for (int i = 1; i < st->pos.size(); ++i) {
			Vec3f delta(st->pos[i].x - st->pos[i - 1].x, st->pos[i].y - st->pos[i - 1].y, st->pos[i].z - st->pos[i - 1].z);
			float tl = sqrt(delta.x*delta.x + delta.y*delta.y + delta.z*delta.z);
			tDis[i] = tDis[i - 1] + tl;
		}
		for (int i = 0; i < st->pos.size(); ++i) {
			if (i < 20)
				pDisInfo[offset + i] = 1.0;
			else
				pDisInfo[offset + i] =1.0 - (i - 20) * 0.1;

		}
		offset += st->pos.size();
		nStrand++;
	}

	ret["vps"] = vpsBuffer;
	ret["tl"] = tlBuffer;
	ret["jointId"] = jointIds;
	ret["jointWeight"] = jointWeight;
	ret["disInfo"] = disInfo;
	
	return ret;
}
END_P3D_NS