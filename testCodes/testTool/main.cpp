
#include<iostream>
#include <omp.h>
#include "render/vrayRender.h" 
#include <string>
#include "engine/P3dEngine.h"
#include <opencv2/opencv.hpp>
#include "geo/meshDataIO.h"
#include "engine/Camera.h"
#include "common/json_inc.h"
#include "HairSmoother.h"
#include "tools/deformEyes/hairFix.h"

using namespace P3D;
using namespace std;

float64 computeStrandDis(shared_ptr<CommonSingleStrand> strand1, shared_ptr<CommonSingleStrand> strand2) {
	float64 dis = 0.5;
	for (const auto& p1 : strand1->pos) {
		for (const auto& p2 : strand2->pos) {
			auto tp = p1 - p2;
			//Vec2f tp2(tp.x, tp.y);
			auto tDis = tp.length();
			if (tDis > dis)
				dis = tDis;
		}
	}
	return dis;
}
void computeStrandGroupDis(std::vector<shared_ptr<CommonSingleStrand>>& strands,std::vector<std::vector<float64>>& disMap) {
	disMap.resize(strands.size());
	for (auto& t : disMap)
		t = vector<float64>(strands.size(), 0);
	uint32 threadNum = 50;
	uint32 meanNum = strands.size() / 50;

	#pragma omp parallel num_threads(threadNum)
	{
		auto id = omp_get_thread_num();
		auto starIdx = id * meanNum;
		auto endIdx = starIdx + meanNum;
		if (starIdx == threadNum - 1)
			endIdx = strands.size();
		for (int i = starIdx; i < endIdx; ++i) {
			for (int j = starIdx + 1; j < strands.size(); ++j) {
				auto dis = computeStrandDis(strands[i], strands[j]);
				disMap[i][j] = dis;
				disMap[j][i] = dis;
			}
		}
	}
}
uint32 findNearest(std::set<uint32>& unIn, std::vector<std::vector<float64>>& disMap) {
	auto threadNum = 50;
	auto meanNum = disMap.size() / threadNum;
	std::vector<uint32> eachIdx(threadNum);
	std::vector<float64> eachDis(threadNum);
	#pragma omp parallel num_threads(threadNum)
	{
		auto id = omp_get_thread_num();
		auto starIdx = id * meanNum;
		auto endIdx = id * meanNum + meanNum;
		if (id == threadNum - 1)
			endIdx = disMap.size();
		int32 useId = -1;
		float64 maxDis = 0.0;
		for (auto i = starIdx; i < endIdx; ++i) {
			if(unIn.find(i) != unIn.end())
				continue;
			float64 tDis = 1000.0;
			for (auto j : unIn) {
				if (disMap[i][j] < tDis)
					tDis = disMap[i][j];
			}
			if (tDis > maxDis) {
				maxDis = tDis;

				useId = i;
			}

		}
		eachIdx[id] = useId;
		eachDis[id] = maxDis;
	}
	float64 maxDis = 0;
	uint32 useId = 0;
	for (auto i = 0; i < threadNum; ++i) {
		if (eachDis[i] > maxDis) {
			maxDis = eachDis[i];
			useId = eachIdx[i];
		}
	}
	return useId;

}
std::shared_ptr<HairModel> getSimpleHair(std::shared_ptr<HairModel> srcHair,uint32 num) {
	auto retHair = make_shared<HairModel>();
	retHair->srcStrands.reserve(num);
	retHair->srcStrands.push_back(srcHair->srcStrands[0]);
	vector<vector<float64> > disMap;
	computeStrandGroupDis(srcHair->srcStrands, disMap);
	cout << "dis Map ok" << endl;
	set<uint32> unIn = { 0 };
	while (retHair->srcStrands.size() < num) {
		auto useId = findNearest(unIn, disMap);
		unIn.insert(useId);
		retHair->srcStrands.push_back(srcHair->srcStrands[useId]);
	}
	cout << "pick ok ,strand size is " << retHair->srcStrands.size() << endl;
	return retHair;
}

float32 calcStrandLength(std::shared_ptr<CommonSingleStrand> s) {
	float32 ret = 0.0;
	for (uint32 i = 1; i < s->pos.size();++i) {
		ret += (s->pos[i] - s->pos[i - 1]).length();
	}
	return ret;
}
std::shared_ptr<HairModel> getLongest(std::shared_ptr<HairModel> srcHair, uint num) {
	auto retHair = make_shared<HairModel>();
	std::vector<float32> lengths(num,0);
	std::vector<int32> idxs(num,-1);
	for (uint32 i = 0; i < srcHair->srcStrands.size(); ++i) {
		auto tl = calcStrandLength(srcHair->srcStrands[i]);
		if(tl < lengths[0])
			continue;
		int j = 0;
		while (j < num - 1 && lengths[j] < tl) {
			lengths[j] = lengths[j + 1];
			idxs[j] = idxs[j + 1];
			j++;
		}
		lengths[j] = tl;
		idxs[j] = i;

	}
	for (auto idx : idxs)
		retHair->srcStrands.push_back(srcHair->srcStrands[idx]);
	return retHair;
	
}
/*
std::shared_ptr<CommonSingleStrand> simpleStrand(std::shared_ptr<CommonSingleStrand> sd, int num) {

}
*/
float32 calcStrandDisAcc(std::shared_ptr<CommonSingleStrand> strand1, std::shared_ptr<CommonSingleStrand> strand2) {
	float32 ret = 0;
	for (const auto& p1 : strand1->pos) {
		float32 td = 1e6;
		for (const auto& p2 : strand2->pos) {
			auto tl = (p1 - p2).length();
			if (tl < td)
				td = tl;
		}
		ret += td;
	}
	return ret;
}
std::pair<int32,float32> getNearestPointOfStrand(std::shared_ptr<CommonSingleStrand> strand, const P3D::Vec3f& p1) {
	auto minDis = 1e5;
	int32 id = 0;
	for (int i = 0; i < strand->pos.size(); ++i) {
		auto td = (strand->pos[i] - p1).length();
		if (td < minDis) {
			minDis = td;
			id = i;
		}
	}
	return { id,minDis };
}

float calcHitOfLine(const P3D::Vec3f& srcP1, const P3D::Vec3f& srcP2, const P3D::Vec3f& dstP) {
	auto lVec = srcP2 - srcP1;
	auto ret = (dstP - srcP1).dot(lVec) / (lVec.dot(lVec));
	return ret;
}
/*
void bindStrandJoint2(std::shared_ptr<CommonSingleStrand> srcStrand,const std::vector<P3D::P3D::Vec3f>& jointPos_,int id,int sId, int eId) {
	const auto& srcPos = srcStrand->pos;
	auto& jointId = srcStrand->jointInfos[id].jointId;
	auto& jointWeight = srcStrand->jointInfos[id].jointWeights;
	jointId.resize(srcPos.size());
	jointWeight.resize(srcPos.size());
	if (sId == eId) {
		for (int i = 0; i < jointId.size(); i++) {
			jointId[i] = sId;
			jointWeight[i] = 1.0;
		}
		return;
	}
	if (sId > eId) {
		swap(sId, eId);
	}
	auto lastId = jointId.size() - 1;

	auto srcLength = calcStrandLength(srcStrand);
	auto jointPos = jointPos_;
	
	auto hit1 = calcHitOfLine(jointPos[sId], jointPos[sId+1], srcPos[0]);
	auto jointLen = 0.0;
	int32 sOffset = 0;
	int32 eOffset = 0;
	if (hit1 > 0.0 && hit1 < 1.0) {
		jointId[0] = sId;
		jointWeight[0] = 1.0 - hit1;
		jointPos[sId] = jointPos[sId] + (jointPos[sId + 1] - jointPos[sId]) * hit1;
	}
	else {
		if (sId == 0) {
			jointWeight[0] = 1.0;
			jointId[0] = sId;
		}
		else {
			auto hit2 = calcHitOfLine(jointPos[sId - 1], jointPos[sId], srcPos[0]);
			if (hit2 > 0.0 && hit2 < 1.0) {
				jointId[0] = sId - 1;
				jointWeight[0] = 1.0 - hit2;
				auto tPos = jointPos[sId - 1] + (jointPos[sId] - jointPos[sId - 1])*hit2;
				jointPos.insert(jointPos.begin() + sId, tPos);
				eId++;
				sOffset = -1;
			}
			else {
				jointId[0] = sId;
				jointWeight[0] = 1.0;
			}
		}
	}
	hit1 = calcHitOfLine(jointPos[eId - 1], jointPos[eId], srcPos.back());
	if (hit1 > 0.0 && hit1 < 1.0) {
		jointId[lastId] = eId - 1+sOffset;
		jointWeight[lastId] = 1.0 - hit1;
		jointPos[eId] = jointPos[eId - 1] + (jointPos[eId] - jointPos[eId - 1]) * hit1;
	}
	else {
		if (eId == jointPos.size() - 1) {
			jointId[lastId] = eId + sOffset;
			jointWeight[lastId] = 1.0;
		}
		else {
			auto hit2 = calcHitOfLine(jointPos[eId], jointPos[eId + 1], srcPos.back());
			if (hit2 > 0.0 && hit2 < 1.0) {
				jointId[lastId] = eId+sOffset;
				jointWeight[lastId] = 1.0 - hit2;
				auto tPos = jointPos[eId] + (jointPos[eId + 1] - jointPos[eId])*hit2;
				jointPos.insert(jointPos.begin() + eId + 1, tPos);
				eId += 1;
			//	eOffset--;
			}
			else {
				jointId[lastId] = eId + sOffset;
				jointWeight[lastId] = 1.0;
			}
		}
	}
	if (jointId.back() == 10) {
		auto t = 0;
	}
	std::vector<float32> jointLs(eId - sId + 1);
	for (int i = sId+1; i <= eId; ++i) {
		jointLen += (jointPos[i] - jointPos[i - 1]).length();
		jointLs[i - sId] = jointLen;
	}
	auto nowLen = 0.0;
	int tId = 0;
	for (int i = 1; i < lastId; ++i) {
		nowLen += (srcPos[i] - srcPos[i - 1]).length();
		auto rt = nowLen / srcLength;
		auto dl = rt * jointLen;
		while (dl > jointLs[tId])
			++tId;
		auto deltD = dl - jointLs[tId - 1];
		auto tr = deltD / (jointLs[tId] - jointLs[tId - 1]);
		if (tId == 1) {
			jointId[i] = sId + tId - 1+sOffset;
			jointWeight[i] = (1.0 - tr) *(jointWeight[0]);
		}
		else if (tId == jointLs.size() - 1) {
			jointId[i] = min(sId + tId - 1 + sOffset + eOffset,9);
			if (jointId[i] == jointId.back())
				jointWeight[i] = 1.0 - (tr * (1.0 - jointWeight.back()));
			else
				jointWeight[i] = 1 - tr;
		}
		else {
			jointId[i] = sId + tId - 1 + sOffset;
			jointWeight[i] = 1 - tr;
		}
		if (jointId[i] < jointId[i - 1] || (jointId[i] == jointId[i - 1] && jointWeight[i] > jointWeight[i - 1])) {
			int aaaaa = 1;
		}
		if (jointId[i] - jointId[i - 1] > 1) {
			int aaaaa = 2;
		}

	}
	
}
*/
/*
void bindJoint2(P3D::pHairModel srcHair, pHairModel jointHair) {

	std::vector<array<int32, 2> > strandIdx(srcHair->srcStrands.size(), {-1,-1});
	for (int i = 0; i < strandIdx.size(); ++i) {
		float tDis1 = 1e6;
		float tDis2 = 1e6;
		for (int j = 0; j < jointHair->srcStrands.size(); ++j) {
			auto td = calcStrandDisAcc(srcHair->srcStrands[i], jointHair->srcStrands[j]);
			if (td < tDis2) {
				tDis2 = td;
				strandIdx[i][1] = j;
				if (tDis2 < tDis1) {
					swap(tDis1, tDis2);
					swap(strandIdx[i][0], strandIdx[i][1]);
				}
			}
		}
	}

	for (int i = 0; i < strandIdx.size(); ++i) {
		auto srcStrand = srcHair->srcStrands[i];
		auto jointStrand1 = jointHair->srcStrands[strandIdx[i][0]];
		auto min1 = getNearestPointOfStrand(jointStrand1, srcStrand->pos[0]);
		auto max1 = getNearestPointOfStrand(jointStrand1, srcStrand->pos.back());
		bindStrandJoint2(srcStrand, jointStrand1->pos,0, std::get<0>(min1), std::get<0>(max1));
		auto jointStrand2 = jointHair->srcStrands[strandIdx[i][1]];
		auto min2 = getNearestPointOfStrand(jointStrand2, srcStrand->pos[0]);
		auto max2 = getNearestPointOfStrand(jointStrand2, srcStrand->pos.back());
		bindStrandJoint2(srcStrand, jointStrand2->pos,1, std::get<0>(min2), std::get<0>(max2));
		auto td1 = calcStrandDisAcc(srcStrand, jointStrand1);
		auto td2 = calcStrandDisAcc(srcStrand, jointStrand2);
		auto w1 = td2*td2 / (td1*td1 + td2*td2);
		srcStrand->jointInfos[0].id = strandIdx[i][0];
		srcStrand->jointInfos[0].weight = w1;
		srcStrand->jointInfos[1].id = strandIdx[i][1];
		srcStrand->jointInfos[1].weight = 1.0 - w1;
	}

}


void bindStrandJoint1(std::shared_ptr<CommonSingleStrand> srcStrand, std::shared_ptr<CommonSingleStrand> jointStrand,int32 id) {
	auto& jointInfo = srcStrand->jointInfos[id];
	const auto& jointPos = jointStrand->pos;
	const auto& srcPos = srcStrand->pos;
	jointInfo.jointId.resize(srcStrand->pos.size());
	jointInfo.jointWeights.resize(srcStrand->pos.size());

	for (int j = 0; j < srcStrand->pos.size(); ++j) {
		auto minDis = getNearestPointOfStrand(jointStrand, srcStrand->pos[j]);
		auto nearId = std::get<0>(minDis);
		if (nearId == jointPos.size() - 1) {
			auto hitT = calcHitOfLine(jointPos[nearId], jointPos[nearId - 1], srcPos[j]);
			if (hitT > 0.0 && hitT < 1.0) {
				jointInfo.jointId[j] = nearId - 1;
				jointInfo.jointWeights[j] = hitT;
			}
			else {
				jointInfo.jointId[j] = nearId;
				jointInfo.jointWeights[j] = 1.0;
			}
		}
		else if (nearId == 0) {
			auto hitT = calcHitOfLine(jointPos[nearId], jointPos[nearId + 1], srcPos[j]);
			if (hitT > 0.0 && hitT < 1.0) {
				jointInfo.jointId[j] = nearId;
				jointInfo.jointWeights[j] = 1.0 - hitT;
			}
			else {
				jointInfo.jointId[j] = nearId;
				jointInfo.jointWeights[j] = 1.0;
			}
		}
		else {
			auto hitT1 = calcHitOfLine(jointPos[nearId], jointPos[nearId - 1], srcPos[j]);
			if (hitT1 > 0 && hitT1 < 1.0) {
				jointInfo.jointId[j] = nearId - 1;
				jointInfo.jointWeights[j] = hitT1;
			}
			else {
				auto hitT2 = calcHitOfLine(jointPos[nearId], jointPos[nearId + 1], srcPos[j]);
				if (hitT2 > 0 && hitT2 < 1.0) {
					jointInfo.jointId[j] = nearId;
					jointInfo.jointWeights[j] = 1.0 - hitT2;
				}
				else {
					jointInfo.jointId[j] = nearId;
					jointInfo.jointWeights[j] = 1.0;
				}
			}
		}
	}
	
}
*/
//void bind
std::pair<Vec4i16, P3D::Vec4f> calcPointJoint(const P3D::Vec3f& srcPos,const std::vector<P3D::Vec3f>& joints) {
	std::vector<int16> idx;
	std::vector<float32> dis;
	int nCount = 4;
	float32 maxDis = 1000.0f;
	for (int i = 0; i < joints.size(); ++i) {
		auto tDis = (joints[i] - srcPos).length();
		if (idx.size() < nCount || tDis < maxDis) {
			int j = 0;
			while (j < idx.size() && dis[j] < tDis)
				++j;
			idx.insert(idx.begin() + j, i);
			dis.insert(dis.begin() + j, tDis);
		}
		if (idx.size() > nCount)
			idx.pop_back();
		maxDis = dis.back();
	}
	P3D::Vec4f disV(dis);
	float32 smoothV = 0.2;
	disV = disV + P3D::Vec4f(smoothV,smoothV,smoothV,smoothV);
	P3D::Vec4f inverDis(1.0 / disV[0], 1.0 / disV[1], 1.0 / disV[2], 1.0 / disV[3]);
	auto inverDis2 = inverDis * inverDis;
	auto ws = inverDis2 / inverDis2.sum();
	Vec4i16 idxV(idx);
	return { idxV,ws };

}
void bindJoint(std::shared_ptr<HairModel> srcHair, std::shared_ptr<HairModel> jointHair) {
	std::vector<P3D::Vec3f> allJoints;
	for (auto st : jointHair->srcStrands) {
		for (const auto&tp : st->pos)
			allJoints.push_back(tp);
	}

	for (auto st : srcHair->srcStrands) {
		auto& jointInfo = st->jointInfo;
		jointInfo.jointId.resize(st->pos.size());
		jointInfo.jointWeights.resize(st->pos.size());
		for (int i = 0; i < st->pos.size(); ++i) {
			auto jt = calcPointJoint(st->pos[i], allJoints);
			jointInfo.jointId[i] = std::get<0>(jt);
			jointInfo.jointWeights[i] = std::get<1>(jt);
		}
		//break;
	}
	
}

void testJsonJoints() {
	auto jointPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testNet\redshift\bone\joint_pos.json)";
	auto srcPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testNet\menglu_yellow_v2\hair.data)";
	auto dstPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testNet\menglu_yellow_v2\hair_joints.txt)";
	auto toupiPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testNet\menglu_yellow\toupi_yellow.obj)";

	//auto toupi = MeshDataIO::instance().loadObj(toupiPath);
	//Utils::writeFileData(R"(D:\Work\Projects\ProNew\Vray\tmp\testNet\menglu_yellow\tp_vps.data)", toupi->vps->rawData<char>(),toupi->vps->getSize());
	//Utils::writeFileData(R"(D:\Work\Projects\ProNew\Vray\tmp\testNet\menglu_yellow\tp_tl.data)", toupi->tl->rawData<char>(),toupi->tl->getSize());

	
	auto hairJoint = std::make_shared<HairModel>();
	hairJoint->loadFromJson(jointPath);
	auto srcHair = std::make_shared<HairModel>();
	srcHair->loadFromFile(srcPath);
	bindJoint(srcHair, hairJoint);
	srcHair->dumpJointInfo(dstPath);

	auto t = srcHair->getGPUData();
	

	/*
	auto jsonStr = Utils::readFileString(jointPath);
	auto jointRoot = ksjson::parse(jsonStr);
	cout << jointRoot.size() << endl;
	std::vector<std::vector<Mat4f>> allMats;
	for (int i = 0; i < jointRoot.size(); ++i) {
		std::vector<Mat4f> tMats;
		tMats.reserve(jointRoot[i].size());
		for (int j = 0; j < jointRoot[i].size(); ++j) {
			std::vector<float> tArray = jointRoot[i][j];
			tMats.push_back(Mat4f((float*)tArray.data()));

		}
		allMats.push_back(tMats);
	}
	*/
	int a = 0;
}
void testHairGrow() {
	auto headObj = R"(D:\Work\Projects\ProNew\Vray\tmp\hairGrow\head.obj)";
	auto rootPosF = R"(D:\Work\Projects\ProNew\Vray\tmp\hairGrow\rootPositions.txt)";
	auto dstPath = R"(D:\Work\Projects\ProNew\Vray\tmp\hairGrow\dst_hair.data)";
	
	auto vPath = R"(D:\Work\Projects\ProNew\Vray\tmp\hairGrow\tV.npy)";
	auto fPath = R"(D:\Work\Projects\ProNew\Vray\tmp\hairGrow\tF.npy)";
	auto vfCfgPath = R"(D:\Work\Projects\ProNew\Vray\tmp\hairGrow\vf_info.npz)";
	
	/*auto vfCfg = NdUtils::readNpz(vfCfgPath);
	auto grid_size = vfCfg["grid_size"].asFloat();
	auto grid_num = vfCfg["grid_num"].asInt();
	auto min_AABB = vfCfg["min_AABB"].asFloat();
	auto max_AABB = vfCfg["max_AABB"].asFloat();*/

	auto hairSmoother = std::make_shared<CHairSmoother>();
	hairSmoother->init(rootPosF);
	//hairSmoother->setGridSize()
	auto vA = NdUtils::readNpy(vPath).asFloat();
	auto vF = NdUtils::readNpy(fPath).asFloat();
	//hairSmoother->setGridSize(grid_size[{0}], { min_AABB[{0}],min_AABB[{1}],min_AABB[{2}] }, { max_AABB[{0}],max_AABB[{1}],max_AABB[{2}] });
	hairSmoother->setGridSize(4.649479, {-300.82266,1113.1345,-300.27167}, {294.31067,2005.8346,294.86163 });
	hairSmoother->setOrientationField(vF,vA);
	hairSmoother->compNewStrands(dstPath);
}
void testHairFix() {
	auto headPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testTool\ori_head.obj)";
	auto hairPath = R"(D:\Work\Projects\ProNew\Vray\tmp\testTool\ret_kmean_all_hair.data)";
	std::vector<float> tVec = { -0.12561*10.0f, 0.2657551*10.0f, -3.7895575*10.0f };
	std::vector<float> rotVec = { 0.9998099, -0.00781299, -0.01786306, 0.00689411, 0.99867815, -0.05093528, 0.0182374, 0.05080245, 0.99854225 };
	auto  hairFix = std::make_shared<HairFix>();
	hairFix->init(headPath);
	hairFix->solve(hairPath,rotVec,tVec);
}
int main(){
    cout<<"hello world"<<endl;
	testHairFix();
	
	auto hairPath = R"(D:\Work\data\Hair\hairstyles\hairstyles\strands00009.data)";

	auto vs = R"(D:\Work\Projects\ProNew\Vray\testCodes\shaders\hair\hair.vert)";
	auto ps = R"(D:\Work\Projects\ProNew\Vray\testCodes\shaders\hair\hair.frag)";
	auto texPath = R"(D:\TMPDATA\DataSwaps\jinlong_fov_50.png)";
	auto quadPath = R"(D:\Work\Projects\ProNew\Vray\testCodes\shaders\objs\quad.obj)";
	auto srcHairPath = R"(D:\Work\Projects\ProNew\Vray\tmp\hairAttri\redshift\hair_mid.data)";
	auto saveHairPath = R"(D:\Work\Projects\ProNew\Vray\tmp\hairAttri\redshift\hair.data)";

	//testHairGrow();
	//testJsonJoints();
	return 0;

	/*
	P3DEngine::instance().init(APP_OPENGL);
	auto win = P3DEngine::instance().createWindow(512, 512);
	auto shader = P3DEngine::instance().createShader();
	shader->loadFromFile(vs, ps);
	auto scene = win->getScene();*/
	auto hairSrc = std::make_shared<HairModel>();
	//hairSrc->loadFromFile(hairPath);
//	hairSrc->loadFromFile(R"(D:\Work\Projects\ProNew\Vray\tmp\testNet\technique\hair.data)");
	hairSrc->loadFromFile(srcHairPath);
	auto simpleHair = getSimpleHair(hairSrc,6000);
	simpleHair->dumpHair(saveHairPath);
	
	//hairSrc->loadFromFile(R"(simple_hair_10_2.data)");
	
	//auto hairsimple = getSimpleHair(hairSrc,100);
	//auto hairsimple = hairSrc;
	//auto hairsimple2 = getLongest(hairsimple,10);
	//hairsimple2->dumpHair("simple_hair_10_2.data",0);

//	auto hairSimple = std::make_shared<HairModel>();
	//hairSimple->loadFromFile("simple_hair_10_4.data");
	//bindJoint2(hairSrc, hairSimple);
	//hairSrc->dumpJointInfo("hair_joint_info.txt");
	//hairSrc->dumpJointInfo("hair_joint_info_10_2.txt");
	//hairSimple->dumpHair("simple_hair_10_4.data", 4);
	//auto hair = std::make_shared<HairModel>();
	//hair->loadFromFile("simple_hair_5000_500.data");
	return 0;

	/*

	auto cvImg = cv::imread(texPath);
	auto tex = P3DEngine::instance().createTexture(cvImg.cols, cvImg.rows, TextureType::TEXTURE_2D, SampleType::SAMPLE_LINER, BUFFER_USE_TYPE::READ_WRITE, PixelFormat::PIXEL_RGB8,1);

	auto fbo = P3DEngine::instance().createRenderFrame(5120, 5120, 1);

	fbo->addRenderTextures(1);
	//	scene->addComp(quad, shader);
	scene->addHair(hair,shader);
	auto modelMat = Mat4f::getTranslate({ 0.0f,-1.55f,0.0f });
	auto modelRot = Mat4f::getRotY(45.0f);
	hair->setModelMat(modelMat * modelRot);
	auto camera = scene->getCamera();
	camera->setCameraType(Camera::CAMERA_TYPE_ORTH);
	camera->setOrthParams(0.9, 0.9, -100.0, 100.0);
	fbo->apply();
	scene->render();
	auto retTex = P3DEngine::instance().createTexture(512, 512, TEXTURE_2D, SAMPLE_LINER, READ_WRITE, PIXEL_RGBA8, 1);
	auto fboTex = fbo->getRenderTexture(0);
	fboTex->render2Texture(retTex); 
	auto retTexCpu = retTex->dump2Buffer(false);

	auto tCPU = fbo->getRenderTextureCpu(0);

	

	auto cvImg4 = cv::Mat(tCPU->width, tCPU->height, tCPU->nchannel==3 ? CV_8UC3:CV_8UC4, tCPU->getRawData());
	auto cvImg5= cv::Mat(retTexCpu->width, retTexCpu->height, retTexCpu->nchannel==3 ? CV_8UC3:CV_8UC4, retTexCpu->getRawData());
	cv::Mat cvImg6;
	cv::resize(cvImg4, cvImg6, { 512,512 }, 0.0, 0.0, cv::INTER_AREA);
	cv::imwrite("test_tex_render2_simple.png", cvImg5);
	cv::imwrite("test_tex_render_simple.png", cvImg4);
	cv::imwrite("test_tex_render_resize_simple.png", cvImg6);
	return 0;
	fbo->apply0();
	while (true) {
		win->render();
	}
	//render->exportScene(R"(tmp\scene_1.vrscene)");

	return 0;*/
}
