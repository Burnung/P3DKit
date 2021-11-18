#pragma once
#include "MeshGpu.h"
#include "common/RenderCommon.h"
#include "OGLCommon.h"
#include "OGLShader.h"
#include "OGLVertexBuffer.h"
BEGIN_P3D_NS

class GLVbo : public MeshGPU {
public:
	virtual ~GLVbo();
	virtual void setShader(pShader tShader);
	virtual pShader getShader()override { return mShader; }
	virtual void init(pDataBlock vps, pDataBlock tl, pDataBlock uvs = nullptr, pDataBlock vns = nullptr);
	virtual void update(pDataBlock vps, pDataBlock uvs = nullptr, pDataBlock vns = nullptr);
	virtual void update(pDataBlock tData, const std::string& attriName);
	virtual void draw();
	virtual void drawPure();
private:
	void release();

private:
	std::array<uint32,LOC_NUM> attrLoc;
	pGLShader mShader;
	pGLVertexBuffer vpsBuffer = nullptr, tlBuffer = nullptr, uvsBuffer = nullptr, vnsBuffer = nullptr;
	uint32 vao;
	uint32 nTri;
	uint32 elementDataType = GL_UNSIGNED_INT;
};
typedef std::shared_ptr<GLVbo> pGLVbo;

END_P3D_NS
