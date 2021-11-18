#pragma once
#include "common/P3D_Utils.h"
#include "common/RenderCommon.h"
#include "Shader.h"
#include "P3DMaterial.hpp"
BEGIN_P3D_NS
class Renderable :public P3DObject {
public:
	Renderable() = default;
	virtual  ~Renderable() {};
public:
	virtual void setShader(pShader tShader) = 0;
	virtual pShader getShader() = 0;
	virtual void draw() = 0;
	virtual void drawPure() = 0;
	virtual void update(pDataBlock tData, const std::string& attriName) = 0;
	virtual void setModelMat(const Mat4f&)=0;
	virtual void setMaterial(pMaterial) = 0;
	//void setModelMat(const Mat4f& m) { modelMat = m; }

public:

};
typedef std::shared_ptr<Renderable> pRenderable;




END_P3D_NS
