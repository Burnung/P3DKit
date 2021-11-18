#pragma once
#include "OGLCommon.h"
#include "../userGeoGpu.h"
#include "OGLShader.h"
#include "OGLVertexBuffer.h"

BEGIN_P3D_NS
class OGLUserMeshGPU :public UserGeoGPU {
public:
	OGLUserMeshGPU() {
//const char* ATTRINAME[] = { "InPos","InUv","InVn" };
		using namespace SHADER_KEY_WORDS;
		locMaps[IN_VERT_POS] = 0;
		locMaps[IN_VERT_UV] = 1;
		locMaps[IN_VERT_NORMAL] = 2;
		locMaps[IN_VERT_TANGENT] = 3;
		locMaps[IN_VERT_BITANGETN] = 4;
	}
	virtual ~OGLUserMeshGPU();
public:
	virtual void clear() { release(); }
	void release();
	virtual void addVertexData(pVertexBuffer pbuffer, const std::string& attriName,uint32 stride,uint32 offset = 0);
	virtual void addVertexData(pVertexBuffer pbuffer, int attriLoc,uint32 stride,uint32 offset = 0);
	virtual void addElementBuffer(pVertexBuffer pbuffer, GeoTYPE type = GEO_TYPE_TRIS);
	virtual void setShader(pShader tShader) override;
	virtual pShader getShader() override{ return mShader; }
	virtual void commit();
	virtual void commitPure();
	virtual void draw();
	virtual void drawPure();
	virtual void update(pDataBlock pData);
	//virtual void update(pDataBlock pData);
	virtual void update(pDataBlock pData, const std::string& attriName);
private:
	bool hasCommit = false;
	uint32 mVaoId = 0;
	std::vector<std::string> attriNames;
	std::unordered_map<std::string, int> locMaps;
	std::vector<int> attriLocs;
	std::vector<pVertexBuffer> vertexBuffers;
	std::vector<uint32> strides;
	std::vector<uint32> offsets;
	pVertexBuffer elementBuffer = nullptr;
	uint8 geoType = GEO_TYPE_TRIS;
	uint32 drawNum = 0;
	pGLShader mShader;
};
typedef std::shared_ptr<OGLUserMeshGPU> pGLUserMeshGPU;
END_P3D_NS
