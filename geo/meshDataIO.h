
#include "common/P3D_Utils.h"
#include "common/Template_Utils.h"
#include "comp.h"

#pragma once
BEGIN_P3D_NS

class MeshDataIO :public SingletonBase<MeshDataIO>, P3DObject {
	P3D_SINGLETON_DECLEAR(MeshDataIO)
public:
	pComp loadFromFile(const std::string& fpath);
	pComp loadObj(const std::string& fpath);
	pComp loadObjSrc(const std::string& fpath);
	pComp loadPly(const std::string& fpath);

private:
	pDataBlock calcNormals(pDataBlock vps, pDataBlock tl);
};



END_P3D_NS