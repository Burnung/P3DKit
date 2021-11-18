#pragma once
#include "OGLCommon.h"
#include "../p3dVertexBuffer.h"
BEGIN_P3D_NS
class OGLVertexBuffer :public VertexBuffer {
public:
	OGLVertexBuffer() = default;
	virtual ~OGLVertexBuffer();
public:
	virtual bool init(VertexBufferType type, VertexBufferUsage usage,DType baseDataType, pDataBlock pData = nullptr);
	virtual void release();
	virtual bool update(pDataBlock pData,uint32 offset = 0);
	virtual bool setData(pDataBlock pData);
	virtual uint32 getGPUBaseDataType();
	virtual pDataBlock readData();
	virtual void applay();
	virtual void applay(uint32 binding);
	virtual void applay0();

private:
	uint32 vboId = 0;
};

typedef std::shared_ptr<OGLVertexBuffer> pGLVertexBuffer;


END_P3D_NS
