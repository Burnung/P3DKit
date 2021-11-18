#pragma once
#include "common/RenderCommon.h"
#include "p3dVertexBuffer.h"
#include "Shader.h"
#include "renderable.h"
#include "geo/comp.h"
//#include "mater"
#include "P3DMaterial.hpp"
BEGIN_P3D_NS

class UserGeoGPU :public Renderable {
public:
	static std::shared_ptr<UserGeoGPU> make(ApplyType type);
public:
	UserGeoGPU() = default;
	virtual	~UserGeoGPU() {};
	void virtual clear() {};
public:
	virtual void addVertexData(pVertexBuffer pbuffer, const std::string& attriName,uint32 stride,uint32 offset = 0) = 0;
	virtual void addVertexData(pVertexBuffer pbuffer, int attriLoc,uint32 stride,uint32 offset = 0) = 0;
	virtual void addElementBuffer(pVertexBuffer pbuffer, GeoTYPE type = GEO_TYPE_TRIS) = 0;
	virtual void setShader(pShader tShader) = 0;
	virtual pShader getShader() = 0;
	virtual void commit() = 0;
	virtual void commitPure() = 0;
	virtual void draw() = 0;
	virtual void drawPure() = 0;
	virtual void update(pDataBlock pData, const std::string& attriName) = 0;
	void addVertexData(pDataBlock pData, const std::string& attriName, uint32 stride, uint32 offset = 0);
	void addVertexData(pDataBlock pData, int attriLoc, uint32 stride, uint32 offset = 0);
	void addElementBuffer(pDataBlock pData, GeoTYPE type = GEO_TYPE_TRIS);
	virtual void setModelMat(const Mat4f& tm) override { modelMat = tm; }
	virtual void update(pDataBlock pData) = 0;
	void fromComp(pComp tComp);
	virtual void setMaterial(pMaterial tMat) ;
public:
	std::string name;
protected:
	Mat4f modelMat = Mat4f::getIdentity();
	pSkinInfo skinInfo = nullptr;
	pMaterial tMaterial = nullptr;

};
typedef std::shared_ptr<UserGeoGPU> pUserGeoGPU;




END_P3D_NS

