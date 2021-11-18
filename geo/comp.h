#include "common/P3D_Utils.h"
#include "common/mathIncludes.h"
#pragma once
BEGIN_P3D_NS
class P3DComp : public P3DObject {
public:
struct SkinInfo{
	std::string compName;
	std::vector<Vec4f> bId;
	std::vector<Vec4f> bWs;
	std::vector<Mat4f> clusterMats;
	std::vector<Mat4f> flattenMats;
};
public:
	P3DComp() = default;
	P3DComp(pDataBlock _vps, pDataBlock _tl, pDataBlock _uvs, pDataBlock _vns,pDataBlock _vcs=nullptr,pDataBlock _tanget=nullptr,pDataBlock _bt=nullptr);
	~P3DComp() = default;
	//static std::shared_ptr<P3DComp> fromNdArray(PNdArrayF vpsData,PNdArrayF tlsDat)
public:
	void addBS(pDataBlock tvps,const std::string& tName="default");
	void addBS(const std::string& tName, std::vector<pDataBlock> bses,const std::vector<float32>& wsSeg);
	void addSkinInfo(std::shared_ptr<SkinInfo> tSkinInfo) {
		skinInfo = tSkinInfo;
	}
	void applayBS(const std::vector<float32>&ws);
	//void render();
	void setModelMat(const Mat4f& mat);

	void caclNormals(pDataBlock tVps, pDataBlock pTls, pDataBlock dstVns);
	//bool initGpu();
	void initInfo();
	pDataBlock getSrcVps();
	void recalculNormals();

	void splitVertex();
private:
	void applySingleBS(const std::vector<std::pair<float32, pDataBlock>>& bsPairs, float32 w);
public:
	bool isInGPU = false;
	uint32 nVtex, nTri;
	pDataBlock vps = nullptr;
	pDataBlock vns = nullptr;
	pDataBlock uvs = nullptr, uvs2 = nullptr;
	pDataBlock tl = nullptr;
	pDataBlock vcs = nullptr;
	pDataBlock tanget = nullptr;
	pDataBlock bTanget = nullptr;
	pDataBlock uvtl = nullptr, uvtl2 = nullptr;
	pDataBlock vntl = nullptr;

	pDataBlock cache_vps = nullptr;
	pDataBlock cache_vns = nullptr;
	bool hasUpdateBS = false;

	std::vector<std::vector<std::pair<float32,pDataBlock>>> bs;
	std::vector<std::string> bsName;
	std::string name = "comp_base";
	Mat4f modelMat = Mat4f::getIdentity();

	std::vector<std::vector<uint32>> dupIdxMapping;
	std::shared_ptr<SkinInfo> skinInfo;
	//pMeshGPU gpuMesh;

};
typedef std::shared_ptr<P3DComp> pComp;
typedef std::shared_ptr<P3DComp::SkinInfo> pSkinInfo;

END_P3D_NS