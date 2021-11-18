#include "meshDataIO.h"

BEGIN_P3D_NS
using namespace std;

pComp MeshDataIO::loadFromFile(const std::string& fpath) {
	auto postName = Utils::splitString(fpath, '.').back();
	if (postName == "obj")
		return loadObj(fpath);
	else if (postName == "ply")
		return loadPly(fpath);
	return nullptr;
}

pComp MeshDataIO::loadObjSrc(const std::string& fpath) {
	auto lines = Utils::readFileLines(fpath);
	vector<float32> vps;
	vector<float32> tmpuvs;
	vector<float32> tmpvns;
	
	vector<uint32> tl;
	for (auto l : lines) {
		auto tl = Utils::splitString(l);
		if (tl[0] == "v"){
			for (uint8 i = 1; i <= 3; ++i) {
				vps.push_back(Utils::string2Num(tl[i]));
			}
		}
		else if (tl[0] == "vt") {
			for (uint8 i = 1; i <= 2; ++i)
				tmpuvs.push_back(Utils::string2Num(tl[i]));
		}
		else if (tl[0] == "vn") {
			for (uint8 i = 1; i <= 3; ++i)
				tmpvns.push_back(Utils::string2Num(tl[i]));
		}
	}
	vector<int32> tls;
	vector<int32> uvtls;
	vector<int32> vntls;


	for (auto l : lines) {
		auto ll = Utils::splitString(l);
		if (ll[0] == "f") {
			for (int i = 0; i < 3; ++i) {
				auto ff = Utils::splitString(ll[i + 1], '/');
				auto ti = Utils::string2Num<uint32>(ff.front()) - 1;
				tls.push_back(ti);
				if (ff.size() >= 3) {
					auto tn = Utils::string2Num<uint32>(ff[2]) - 1;
					vntls.push_back(tn);
				}
				if (ff.size() >= 2) {
					auto tu = Utils::string2Num<uint32>(ff[1]) - 1;
					uvtls.push_back(tu);
				}
			}
		}		
	}

	auto vpsBlock = std::make_shared<DataBlock>(vps.size()* sizeof(float32),DType::Float32);
	vpsBlock->setFromMem((uint8*)(vps.data()), vps.size() * sizeof(float));

	
	auto tlBlock = std::make_shared<DataBlock>(tls.size() * sizeof(uint32),DType::Uint32);
	tlBlock->setFromMem((uint8*)(tls.data()), tls.size() * sizeof(uint32));

	
	pDataBlock uvsBlock = nullptr;
	if (tmpuvs.size()>0) {
		uvsBlock = std::make_shared<DataBlock>(tmpuvs.size() * sizeof(float32), DType::Float32);
		uvsBlock->setFromMem((uint8*)(tmpuvs.data()), tmpuvs.size() * sizeof(float32));
	}

	pDataBlock vnsBlock = nullptr;
	if (tmpvns.size() > 0) {
		vnsBlock = std::make_shared<DataBlock>(tmpvns.size() * sizeof(float32), DType::Float32);
		vnsBlock->setFromMem((uint8*)(tmpvns.data()), tmpvns.size() * sizeof(float32));
	}

	auto comp = std::make_shared<P3DComp>(vpsBlock,tlBlock,uvsBlock,vnsBlock);

	if (uvtls.size() == tls.size()) {
		pDataBlock uvtlBlock = std::make_shared<DataBlock>(uvtls.size() * sizeof(uint32), DType::Uint32);
		uvtlBlock->setFromMem((uint8*)(uvtls.data()), uvtls.size() * sizeof(uint32));
		comp->uvtl = uvtlBlock;
	}

	if (vntls.size() == tls.size()) {
		pDataBlock vntlBlock = std::make_shared<DataBlock>(vntls.size() * sizeof(uint32), DType::Uint32);
		vntlBlock->setFromMem((uint8*)(vntls.data()), vntls.size() * sizeof(uint32));
		comp->vntl = vntlBlock;
	}

	return comp;


}
pComp MeshDataIO::loadObj(const std::string& fpath) {

	auto lines = Utils::readFileLines(fpath);
	vector<float32> vps;
	vector<float32> tmpuvs;
	vector<float32> tmpvns;
	
	vector<uint32> tl;
	for (auto l : lines) {
		auto tl = Utils::splitString(l);
		if (tl[0] == "v"){
			for (uint8 i = 1; i <= 3; ++i) {
				vps.push_back(Utils::string2Num(tl[i]));
			}
		}
		else if (tl[0] == "vt") {
			for (uint8 i = 1; i <= 2; ++i)
				tmpuvs.push_back(Utils::string2Num(tl[i]));
		}
		else if (tl[0] == "vn") {
			for (uint8 i = 1; i <= 3; ++i)
				tmpvns.push_back(Utils::string2Num(tl[i]));
		}
	}
	bool needVn = !tmpvns.empty();
	uint32 vNum = vps.size() / 3;
	uint32 nTri = 0;
	vector<float32> uvs(vNum * 2,-1.0f);
	vector<float32> vns(vNum * 3, -1.0f);
	std::vector<uint32> newVertIdx;
	uint32 srcNum = vNum;

	for (auto l : lines) {
		auto ll = Utils::splitString(l);
		if (ll[0] == "f") {
			for (int i = 0; i < 3; ++i) {
				auto ff = Utils::splitString(ll[i + 1], '/');
				auto ti = Utils::string2Num<uint32>(ff.front()) - 1;
				int tv = -1;
				int tn = -1;
				if (needVn && ff.size() >= 3)
					tn = Utils::string2Num<int32>(ff[2]) - 1;
				if (tmpuvs.size() > 0) {
					tv = Utils::string2Num<int32>(ff[1]) - 1;
					if ((uvs[ti * 2] != -1 && uvs[ti * 2] != tmpuvs[tv * 2]) || (uvs[ti * 2 + 1] != -1 && uvs[ti * 2 + 1] != tmpuvs[tv * 2 + 1])
						|| (needVn && ((vns[ti*3+2]!=-1 && vns[ti*3+2]!= tmpvns[tn*3])||(vns[ti*2]!=-1 && vns[ti*3]!= tmpvns[tn*3]) || (vns[ti*3+1]!=-1 && vns[tn*3+1]!=tmpvns[ti*3+1])))) {
						uvs.push_back(tmpuvs[tv * 2]);
						uvs.push_back(tmpuvs[tv * 2 + 1]);

						vps.push_back(vps[ti * 3]);
						vps.push_back(vps[ti * 3 + 1]);
						vps.push_back(vps[ti * 3 + 2]);
						if (needVn && tn >= 0) {
							vns.push_back(tmpvns[tn * 3]);
							vns.push_back(tmpvns[tn * 3 + 1]);
							vns.push_back(tmpvns[tn * 3 + 2]);
						}
						tl.push_back(vNum);
						++vNum;
						newVertIdx.push_back(ti);
					}
					else {
						if (needVn && tn >= 0) {
							vns[ti * 3] = tmpvns[tn * 3];
							vns[ti * 3+1] = tmpvns[tn * 3+1];
							vns[ti * 3+2] = tmpvns[tn * 3+2];
						}
						uvs[ti * 2] = tmpuvs[tv * 2];
						uvs[ti * 2 + 1] = tmpuvs[tv * 2 + 1];
						tl.push_back(ti);
					}
				}
				else {
					tl.push_back(ti);
				}
			}
			++nTri;
		}
	}

	std::vector<std::vector<uint32>> dupIdxMapping;
	for (int i = 0; i < newVertIdx.size(); ++i) {
		auto idx_old = newVertIdx[i];
		auto idx_new = i + srcNum;
		bool bFind = false;
		for (int k = 0; k < dupIdxMapping.size(); ++k) {
			if (dupIdxMapping[k][0] == idx_old) {
				dupIdxMapping[k].push_back(idx_new);
				bFind = true;
				break;
			}
		}
		if (!bFind)
			dupIdxMapping.push_back({ idx_old,idx_new });
	}

	auto vpsBlock = std::make_shared<DataBlock>(vNum * 3 * sizeof(float32),DType::Float32);
	vpsBlock->setFromMem((uint8*)(vps.data()), vps.size() * sizeof(float));

	
	auto tlBlock = std::make_shared<DataBlock>(nTri * 3 * sizeof(uint32),DType::Uint32);
	tlBlock->setFromMem((uint8*)(tl.data()), tl.size() * sizeof(uint32));

	
	pDataBlock uvsBlock = nullptr;
	if (uvs.size() / 2 == vps.size() / 3) {
		uvsBlock = std::make_shared<DataBlock>(uvs.size() * sizeof(float32), DType::Float32);
		uvsBlock->setFromMem((uint8*)(uvs.data()), uvs.size() * sizeof(float32));
	}

	pDataBlock vnsBlock = nullptr;
	if (vns.size() == vps.size()) {
		vnsBlock = std::make_shared<DataBlock>(vns.size() * sizeof(float32), DType::Float32);
		vnsBlock->setFromMem((uint8*)(vns.data()), vns.size() * sizeof(float32));
	}

	auto comp = std::make_shared<P3DComp>(vpsBlock,tlBlock,uvsBlock,vnsBlock);
	comp->dupIdxMapping = dupIdxMapping;
	return comp;

}

pComp MeshDataIO::loadPly(const std::string& fpath) {

	int64 nVertics;
	int position;
	bool isMesh = false;

	std::string line;
	std::string s1, s2;

	ifstream inputPly;
	inputPly.open(fpath, ios::binary);

	if (!inputPly.is_open()) {
		cerr << "Couldn't open " << fpath << '\n';
		exit(1);
	}

	bool	ifColor = 0;
	bool ifNormal = 0;
	while (true)
	{
		getline(inputPly, line);
		if (line.find("end_header") != -1)
		{
			cout << line << endl;
			break;
		}

		else if (line.find("vertex") != -1)
			nVertics= atoi(line.substr(15).c_str());

		else if (line.find("nx") != -1)
			ifNormal = true;

		else if (line.find("red") != -1)
			ifColor = true;

		cout << line << endl;
	}

	std::vector<Vec3f> vps;
	std::vector<Vec3f> vns;
	std::vector<Vec3f> vcs;
	float32 minS = 1e6;
	float32 maxS = -1e6;
	if (ifNormal && ifColor) {
		Vec3f tmpP;
		Vec3<uint8> tmpC;
		for (uint32 i = 0; i < nVertics; ++i) {
			inputPly.read((char*)(tmpP.ptr()), sizeof(Vec3f));
			minS = std::min(minS, tmpP.min());
			maxS = std::max(maxS, tmpP.max());
			vps.push_back(tmpP);
			inputPly.read((char*)(tmpP.ptr()), sizeof(Vec3f));
			vns.push_back(tmpP);
			inputPly.read((char*)(tmpC.ptr()), sizeof(tmpC));
			vcs.push_back(Vec3f((float)(tmpC[0]) / 255.0f, (float)(tmpC[1]) / 255.0f, (float)(tmpC[2]) / 255.0f));
		}
	}
	auto vpsData = std::make_shared<DataBlock>(vps.size() * sizeof(Vec3f), DType::Float32);
	vpsData->copyData((uint8*)(vps.data()), vpsData->getSize(), 0);
	pDataBlock vcsData = nullptr, vnsData = nullptr;

	if (ifNormal && vns.size() == vps.size()) {
		vnsData = std::make_shared<DataBlock>(vns.size() * sizeof(Vec3f), DType::Float32);
		vnsData->copyData((uint8*)(vns.data()), vnsData->getSize(), 0);
	}
	if (ifColor && vcs.size() == vps.size()) {
		vcsData = std::make_shared<DataBlock>(vcs.size() * sizeof(Vec3f), DType::Float32);
		vcsData->copyData((uint8*)(vcs.data()), vcsData->getSize(), 0);
	}

	auto tComp = std::make_shared<P3DComp>(vpsData,nullptr,nullptr,vnsData,vcsData);
	return tComp;
}
END_P3D_NS