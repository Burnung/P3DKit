#pragma once

#include "common/P3D_Utils.h"
#include "renderable.h"
#include "Shader.h"
BEGIN_P3D_NS
class MeshGPU :public Renderable {
public:
	static std::shared_ptr<MeshGPU> make(uint8 type);
	virtual ~MeshGPU();
	virtual void setShader(pShader)override{};
	//virtual pShader getShader()=override ;
	virtual void init(pDataBlock vps, pDataBlock tl, pDataBlock uvs = nullptr, pDataBlock vns = nullptr) {};
	virtual void update(pDataBlock vps, pDataBlock uvs = nullptr, pDataBlock vns = nullptr) {};
	virtual void update(pDataBlock tData, int locId) {};
	virtual void update(pDataBlock tData, const std::string& attriName){};
	virtual void draw() {};
	virtual void drawPure() {};
	virtual void setModelMat(const Mat4f& m) override{ modelMat = m; }
	virtual void setMaterial(pMaterial tMat) { material = tMat; }
protected:
	Mat4f modelMat;
	pMaterial material;

};

typedef std::shared_ptr<MeshGPU> pMeshGPU;

END_P3D_NS